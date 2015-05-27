#include <draw/Drawable.hpp>

#include <stack>
#include <utility>
#include <vector>

#include <resources.hpp>



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

    // copy transformation matrix
    assimp_to_eigen_matrix(&(n->transform), node->mTransformation);

    // copy meshes
    n->meshes.reserve(node->mNumMeshes);
    for (int i = 0; i < node->mNumMeshes; i++) {
        draw::Shape s;
        aiMesh *mesh = scene->mMeshes[i];
        s.init(parent_drawable->texs, *mesh);
        n->meshes.push_back(s);
    }

    // recurse
    n->children.reserve(node->mNumChildren);
    for (int i = 0; i < node->mNumChildren; i++) {
        aiNode *child = node->mChildren[i];
        Node *c = deepcopy_assimp_tree(parent_drawable, child, scene);
        n->children.push_back(c);
    }

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

        // load Assimp scene from file
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(config.directory+"/"+config.file,
                                                 aiProcess_CalcTangentSpace | 
                                                 aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType);
        if (!scene) {
            // do some error
            std::cerr << "NO SCENE FOUND! ABORT!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // copy Assimp scene structure to Drawable
        const aiNode *rootAiNode = scene->mRootNode;
        root = deepcopy_assimp_tree(this, rootAiNode, scene);
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

    static void draw_node(Node *current, Program *prog, MatrixStack *P, MatrixStack *MV, Camera *cam) {
        MV->pushMatrix();
        MV->multMatrix(current->transform);
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
        glUniform1i(prog->getUniform("mode"), 't'); 
        for (auto it = current->meshes.begin(); it != current->meshes.end(); it++) {
            it->draw(prog->getAttribute("vertPos"),
                     prog->getAttribute("vertNor"),
                     prog->getAttribute("vertTex"),
                     prog->getUniform("texture0"));
        }

        for (auto it = current->children.begin(); it != current->children.end(); it++) {
            draw_node(*it, prog, P, MV, cam);
        }
        MV->popMatrix();
    }

    void Drawable::draw(Program *prog, MatrixStack *P, MatrixStack *MV, Camera *cam) {
        draw_node(root, prog, P, MV, cam);
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
};
