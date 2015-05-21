#include <Entity.hpp>
#include <algorithm>

Entity::Entity() : pos(0, 0, 0), 
                   rot(Eigen::Matrix4f::Identity()),
                   drawable(NULL) {}

Entity::Entity(draw::Drawable *d) : pos(0, 0, 0),
                                   rot(Eigen::Matrix4f::Identity()),
                                    drawable(d) {}

Entity::~Entity() {}

void Entity::setRotation(float angle, Eigen::Vector3f axis) {
    assert(0);
}

void Entity::setRotationMatrix(Eigen::Matrix4f mat) {
    rot = mat;
}

void Entity::setPosition(Eigen::Vector3f position) {
    pos = position;
}


const Eigen::Matrix4f& Entity::getRotation() const {
    return rot;
}

void Entity::move(Eigen::Vector3f translation) {assert(0);} 
void Entity::rotate(float angle, Eigen::Vector3f axis) {assert(0);}

void Entity::attachDrawable(draw::Drawable *drawable) {
   this->drawable = drawable;
}

draw::Drawable& Entity::getDrawable() {
    return *drawable;
}

void Entity::clearDrawable() {
   this->drawable = NULL;
}

static const float y_filter_range = 0.25;





static Eigen::Vector3f modelspace_center(const draw::Shape *s) {
    Eigen::Vector3f c(0,0,0);

    for (auto it = s->vertices.begin(); it != s->vertices.end(); it++) {
        float x = *it++;
        float y = *it++;
        float z = *it;
        Eigen::Vector3f v(x,y,z);
        c+= v;
        std::cout << z << std::endl;
    }

    int num_v = s->vertices.size() / 3;
    c = c/num_v;

    std::cout << "modelspace center = " << c << std::endl;
    return c;
}

static float modelspace_radius(const draw::Shape *s, Eigen::Vector3f c) {
    float r = 0;

    for (auto it = s->vertices.begin(); it != s->vertices.end(); it++) {
        float x = *it++;
        float y = *it++;
        float z = *it;
        Eigen::Vector3f v(x,y,z);
        r = std::max(r, (v - c).norm());
    }

    return r;
}

void Entity::calculate_center_and_radius() {
    // find the first mesh
    const draw::Shape *shape = drawable->find_first_shape();

    center = modelspace_center(shape);
    radius = modelspace_radius(shape, center);
/*
    center = Eigen::Vector3f(0,0,0);
    
    for (auto it = shape->indices.begin(); it != shape->indices.end(); it++) {
        uint ix = *it++;
        uint iy = *it++;
        uint iz = *it;
        float x = shape->vertices[ix];
        float y = shape->vertices[iy];
        float z = shape->vertices[iz];
        Eigen::Vector4f vertex(x,y,z, 1);
        Eigen::RowVector4f pos_row(pos(0), pos(1), pos(2), 1);
        Eigen::Vector4f pos_col(pos(0), pos(1), pos(2), 1);
        //Eigen::Vector4f temp = rot  * vertex;

        Eigen::Matrix4f new_pos = (Eigen::Matrix4f::Identity());
        new_pos(0,3) = pos_col(0);
        new_pos(1,3) = pos_col(1);
        new_pos(2,3) = pos_col(2);

        Eigen::Vector4f world_space_vertex = vertex.transpose() * (rot * new_pos);
        Eigen::Vector3f world_vec3 = world_space_vertex.head<3>();
        float world_y = world_space_vertex(1);
        
        if (world_y < current_y + y_filter_range &&
            world_y > current_y - y_filter_range) {
            center += world_vec3;
        }

        center = center / (shape->indices.size() / 3);

//        std::cout << drawable->name + " " << center << std::endl;

    }
*/


/*    for (auto it = shape->indices.begin(); it != shape->indices.end(); it++) {
        uint ix = *it++;
        uint iy = *it++;
        uint iz = *it;
        float x = shape->vertices[ix];
        float y = shape->vertices[iy];
        float z = shape->vertices[iz];
        Eigen::Vector4f vertex(x,y,z, 1);
        Eigen::RowVector4f pos_row(pos(0), pos(1), pos(2), 1);
        Eigen::Vector4f pos_col(pos(0), pos(1), pos(2), 1);
        //Eigen::Vector4f temp = rot  * vertex;

        Eigen::Matrix4f new_pos = (Eigen::Matrix4f::Identity());
        new_pos(0,3) = pos_col(0);
        new_pos(1,3) = pos_col(1);
        new_pos(2,3) = pos_col(2);

        Eigen::Vector4f world_space_vertex = vertex.transpose() * (rot * new_pos);
        Eigen::Vector3f world_vec3 = world_space_vertex.head<3>();
        float world_y = world_space_vertex(1);
        
        if (world_y < current_y + y_filter_range &&
            world_y > current_y - y_filter_range) {
            radius = std::max<float>(radius,
\                                     (world_vec3 - center).norm());
        }
        }*/
}

float Entity::getRadius() const {
    return radius;
}

Eigen::Vector3f Entity::getCenterWorld() const {
    Eigen::Matrix4f tmat(Eigen::Matrix4f::Identity());
    for (int i = 0; i < 3; i++) {
        tmat(i,3) = pos(i);
    }

    Eigen::Vector4f c4(center(0), center(1), center(2), 1);
    Eigen::Vector3f result = (tmat * c4).head<3>();

    return result;
}
