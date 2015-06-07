#include <Entity.hpp>
#include <algorithm>

Entity::Entity() : name(""),
                   pos(0, 0, 0), 
                   rot(Eigen::Matrix4f::Identity()),
                   drawable(NULL),
                   use_bounding_box(false),
                   selected(false)
{}

Entity::Entity(draw::Drawable *d) : name(""),
                                    pos(0, 0, 0),
                                    rot(Eigen::Matrix4f::Identity()),
                                    drawable(d),
                                    use_bounding_box(false),
                                    selected(false)
{}

Entity::~Entity() {}

void Entity::setName(std::string str) {
    name = str;
}

std::string Entity::getName() {
    return name;
}


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

void Entity::move(Eigen::Vector3f translation) {
    pos += translation;
}

void Entity::rotate(float angle, Eigen::Vector3f axis) {
    assert(0);
}

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
//        std::cout << z << std::endl;
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

const BoundingBox &Entity::getBoundingBox() const {
    return bounding_box;
}


void Entity::generateBoundingBox() {
    const draw::Shape *s = drawable->find_first_shape();
    std::vector< Eigen::Vector3f > verts;
    for (auto it = s->vertices.begin(); it != s->vertices.end(); it++) {
        float x = *it++;
        float y = *it++;
        float z = *it;
        Eigen::Vector3f vec(x,y,z);
        verts.push_back(vec);
    }
    bounding_box = BoundingBox(verts);
}

bool Entity::useBoundingBox() const {
    return use_bounding_box;
}

void Entity::setUseBoundingBox(bool use) {
    use_bounding_box = use;
}
