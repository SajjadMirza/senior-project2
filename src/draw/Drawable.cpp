#include <draw/Drawable.hpp>

#include <stack>
#include <utility>
#include <vector>

#include <resources.hpp>

#include <log.hpp>

using draw::Node;

void assimp_to_eigen_matrix(Eigen::Matrix4f *dst, const aiMatrix4x4 &src) {
    Eigen::Matrix4f &m = *dst;

    /* assimp matrices are row-major so we need to do this backwards */

    m(0,0) = src.a1;
    m(1,0) = src.a2;
    m(2,0) = src.a3;
    m(3,0) = src.a4;
    m(0,1) = src.b1;
    m(1,1) = src.b2;
    m(2,1) = src.b3;
    m(3,1) = src.b4;
    m(0,2) = src.c1;
    m(1,2) = src.c2;
    m(2,2) = src.c3;
    m(3,2) = src.c4;
    m(0,3) = src.d1;
    m(1,3) = src.d2;
    m(2,3) = src.d3;
    m(3,3) = src.d4; // assimp sucks
   
}

Node *deepcopy_assimp_tree(const draw::Drawable *parent_drawable, const aiNode *node, const aiScene *scene) {
    Node *n = new Node();

    LOG("copy 1");
    // copy transformation matrix
    assimp_to_eigen_matrix(&(n->transform), node->mTransformation);

    LOG("copy 2");
    // copy meshes
    n->meshes.reserve(node->mNumMeshes);
    LOG("copy 2.1");
    for (int i = 0; i < node->mNumMeshes; i++) {
        draw::Shape s;
        LOG("copy 2.2");
        aiMesh *mesh = scene->mMeshes[i];
        LOG("copy 2.3");
        s.init(parent_drawable->texs, *mesh);
        LOG("Copy 2.4");
        n->meshes.push_back(s);
        LOG("Copy 2.5");
    }

    LOG("copy 3");
    // recurse
    n->children.reserve(node->mNumChildren);
    for (int i = 0; i < node->mNumChildren; i++) {
        aiNode *child = node->mChildren[i];
        Node *c = deepcopy_assimp_tree(parent_drawable, child, scene);
        n->children.push_back(c);
    }

    LOG("copy 4");
    return n;

}

static void print(std::string type, int count) {
    std::cout << "type " << type << " has " << count << " textures" << std::endl;
}

static void print_texture_type_counts(const aiMaterial *mat) {
    print("diffuse", mat->GetTextureCount(aiTextureType_DIFFUSE));
    print("specular", mat->GetTextureCount(aiTextureType_SPECULAR));
    print("ambient", mat->GetTextureCount(aiTextureType_AMBIENT));
    print("emissive", mat->GetTextureCount(aiTextureType_EMISSIVE));
    print("height", mat->GetTextureCount(aiTextureType_HEIGHT));
    print("normals", mat->GetTextureCount(aiTextureType_NORMALS));
    print("shininess", mat->GetTextureCount(aiTextureType_SHININESS));
    print("opacity", mat->GetTextureCount(aiTextureType_OPACITY));
    print("displacement", mat->GetTextureCount(aiTextureType_DISPLACEMENT));
    print("lightmap", mat->GetTextureCount(aiTextureType_LIGHTMAP));
    print("reflection", mat->GetTextureCount(aiTextureType_REFLECTION));
    print("unknown", mat->GetTextureCount(aiTextureType_UNKNOWN));
    print("none", mat->GetTextureCount(aiTextureType_NONE));
}

namespace draw {

    Drawable::Drawable(const ModelConfig &config) {
        LOG("drawable constructor");
        name = config.model;
        // load textures to Drawable
        const ModelTextureConfig &texconf = config.textures;
        if (!texconf.diffuse.empty()) {
            Texture dt;
            dt.filename = texconf.diffuse;
            dt.type = TexType::DIFFUSE;
            resource::load_texture_from_file(config.directory+"/"+texconf.diffuse,
                                             &dt.tid);
            texs.diffuse = dt;
        }

        if (!texconf.normal.empty()) {
            Texture dt;
            dt.filename = texconf.normal;
            dt.type = TexType::NORMAL;
            resource::load_texture_from_file(config.directory+"/"+texconf.normal,
                                             &dt.tid);
            texs.normal = dt;
        }

        if (!texconf.specular.empty()) {
            Texture dt;
            dt.filename = texconf.specular;
            dt.type = TexType::SPECULAR;
            resource::load_texture_from_file(config.directory+"/"+texconf.specular,
                                             &dt.tid);
            texs.specular = dt;
        }

        LOG("loading scene through assimp");
        LOG(config.directory+"/"+config.file);
        // load Assimp scene from file
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(config.directory+"/"+config.file,
                                                 aiProcess_CalcTangentSpace | 
                                                 aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType);
        LOG("loaded scene through assimp");
        if (!scene) {
            // do some error
            std::cerr << "NO SCENE FOUND! ABORT!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // copy Assimp scene structure to Drawable
        LOG("copy assimp scene structure to drawable");
        const aiNode *rootAiNode = scene->mRootNode;
        root = deepcopy_assimp_tree(this, rootAiNode, scene);
        LOG("done copying scene");
    }
    
    
    Drawable::Drawable(const aiScene *scene, std::string& dir) {
        /* copy the aiScene scene tree into our own scene graph */
        aiNode *rootAiNode = scene->mRootNode;

        std::cout << "num internal textures " << scene->mNumTextures << std::endl;

        // TODO move this loop into its own function
        for (int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial *mat = scene->mMaterials[i];
            aiString filename;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
            std::string name(filename.C_Str());
            std::string file_location = dir + "/" +  name;

            print_texture_type_counts(mat);

            FIBITMAP *img = resource::GenericLoader(file_location.c_str(), 0);

            uint bit_depth = FreeImage_GetBPP(img);
            BYTE *bits = FreeImage_GetBits(img);

            Texture tex;
            tex.filename = name;
            tex.type = TexType::DIFFUSE;
    
            glGenTextures(1, &(tex.tid));

            glBindTexture(GL_TEXTURE_2D, tex.tid);

            int width = FreeImage_GetWidth(img);
            int height = FreeImage_GetHeight(img);

            if (bit_depth == RGB_BITS) {
              
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             RGB_CHANNELS,
                             width,
                             height,
                             0,
                             GL_BGR,
                             GL_UNSIGNED_BYTE,
                             (void*) bits); 
            }
            else if (bit_depth == RGBA_BITS) {
                glTexImage2D(GL_TEXTURE_2D,
                             0, RGBA_CHANNELS,
                             width,
                             height,
                             0,
                             GL_BGRA,
                             GL_UNSIGNED_BYTE,
                             (void*) bits); 
            }
            else {
                std::cerr << "WARNING: " << name << " HAS INVALID BIT DEPTH: " << bit_depth << std::endl;
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             RGB_CHANNELS,
                             width,
                             height,
                             0,
                             GL_BGR,
                             GL_UNSIGNED_BYTE,
                             (void*) bits); 
            }

            // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            // Generate mipmaps, by the way.
             glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            this->textures.insert(TexTable::value_type(tex.filename, tex));
    
            FreeImage_Unload(img);
        }
      


        root = deepcopy_assimp_tree(this, rootAiNode, scene);
    }

    Drawable::~Drawable() {}

    static void draw_node_color(Node *current, Program *color_prog,
                                MatrixStack *P, MatrixStack *MV,
                                Camera *cam) {
        MV->pushMatrix();
        MV->multMatrix(current->transform);
        
        glUniformMatrix4fv(color_prog->getUniform("MV"), 1, GL_FALSE,
                           MV->topMatrix().data());
        
        for (auto it = current->meshes.begin();
             it != current->meshes.end(); it++) {
            it->colorDraw(color_prog->getAttribute("vertPos"));
        }

        for (auto it = current->children.begin();
             it != current->children.end(); it++) {
            draw_node_color(*it, color_prog, P, MV, cam);
        }
        
        MV->popMatrix();
    }

    static void draw_node(Node *current, Program *prog, MatrixStack *P, MatrixStack *MV, Camera *cam, TextureBundle& texs) {
        MV->pushMatrix();
        MV->multMatrix(current->transform);
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
        glUniform1i(prog->getUniform("mode"), 't'); 
        for (auto it = current->meshes.begin(); it != current->meshes.end(); it++) {
            if (texs.normal) {
                glUniform1i(prog->getUniform("uNormFlag"), 1);

                it->draw(prog->getAttribute("vertPos"),
                         prog->getAttribute("vertNor"),
                         prog->getAttribute("vertTex"),
                         prog->getUniform("texture0"),
                         prog->getUniform("textureNorm"));

                glUniform1i(prog->getUniform("uNormFlag"), 0);
            }
            else {
                it->draw(prog->getAttribute("vertPos"),
                         prog->getAttribute("vertNor"),
                         prog->getAttribute("vertTex"),
                         prog->getUniform("texture0"));
            }
        }

        for (auto it = current->children.begin();
             it != current->children.end(); it++) {
            draw_node(*it, prog, P, MV, cam, texs);
        }
        MV->popMatrix();
    }

    void Drawable::drawColor(Program *color_prog, MatrixStack *P, MatrixStack *MV,
                             Camera *cam) {
        draw_node_color(root, color_prog, P, MV, cam);
    }

    void Drawable::draw(Program *prog, MatrixStack *P, MatrixStack *MV,
                        Camera *cam) {
        draw_node(root, prog, P, MV, cam, texs);
    }

    static Shape *find_first(Node *n) {
        if (n->meshes.empty()) {
            for (auto it = n->children.begin(); it != n->children.end(); it++) {
                Node *child = *it;
                Shape *s = find_first(child);
                if (s) {
                    return s;
                }

            }

            return NULL;
        }
        else {
            return &n->meshes.at(0);
        }
    }

    const Shape *Drawable::find_first_shape() {
        Shape *s = find_first(root);
        return s;
    }

    static void draw_node_deferred(Node *current, Program *prog, MatrixStack *M, Camera *cam,
                                   TextureBundle texs)
    {
        M->pushMatrix();
        M->multMatrix(current->transform);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        
        for (auto it = current->meshes.begin(); it != current->meshes.end(); it++) {
            if (texs.normal) {
#if 1
                glUniform1i(prog->getUniform("uCalcTBN"), 1);
                glUniform1i(prog->getUniform("uNormFlag"), 1);
                it->draw(prog->getAttribute("vertPos"),
                         prog->getAttribute("vertNor"),
                         prog->getAttribute("vertTex"),
                         prog->getUniform("texture0"),
                         prog->getUniform("texture_norm"),
                         prog->getUniform("texture_spec"),
                         prog->getAttribute("tangent"),
                         prog->getAttribute("bitangent"));
                glUniform1i(prog->getUniform("uNormFlag"), 0);
                glUniform1i(prog->getUniform("uCalcTBN"), 0);
#else
                it->drawSpec(prog->getAttribute("vertPos"),
                             prog->getAttribute("vertNor"),
                             prog->getAttribute("vertTex"),
                             prog->getUniform("texture0"),
                             prog->getUniform("texture_spec"));
#endif
            }
            else {
                it->drawSpec(prog->getAttribute("vertPos"),
                             prog->getAttribute("vertNor"),
                             prog->getAttribute("vertTex"),
                             prog->getUniform("texture0"),
                             prog->getUniform("texture_spec"));
            }
        }

        for (auto it = current->children.begin(); it != current->children.end(); it++) {
            draw_node_deferred(*it, prog, M, cam, texs);
        }

        M->popMatrix();
    }

    void Drawable::drawDeferred(Program *prog, MatrixStack *M, Camera *cam)
    {
        draw_node_deferred(root, prog, M, cam, texs);
    }

   static void draw_node_light_volume(Node *current, Program *prog, MatrixStack *M, Camera *cam)
    {
        M->pushMatrix();
        //    M->multMatrix(current->transform);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        
        for (auto it = current->meshes.begin(); it != current->meshes.end(); it++) {
            it->drawLightVolume(prog->getAttribute("vertPos"));
        }

        for (auto it = current->children.begin(); it != current->children.end(); it++) {
            draw_node_light_volume(*it, prog, M, cam);
        }

        M->popMatrix();
    }
    
    void Drawable::drawAsLightVolume(Program *prog, MatrixStack *M, Camera *cam)
    {
        //LOG("Drawable::drawAsLightVolume");
        draw_node_light_volume(root, prog, M, cam);
    }

    static void draw_node_depth(Node *current, Program *prog, MatrixStack *M)
    {
        M->pushMatrix();
        M->multMatrix(current->transform);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        
        for (auto it = current->meshes.begin(); it != current->meshes.end(); it++) {
            it->drawDepth(prog->getAttribute("vertPos"));
        }

        for (auto it = current->children.begin(); it != current->children.end(); it++) {
            draw_node_depth(*it, prog, M);
        }

        M->popMatrix();
    }

    void Drawable::drawDepth(Program *prog, MatrixStack *M)
    {
        draw_node_depth(root, prog, M);
    }
};
