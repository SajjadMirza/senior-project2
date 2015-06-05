#include <Camera.hpp>

Camera::Camera() :
    aspect(1.0f),
    fovy(45.0f/180.0f*M_PI),
    znear(0.1f),
    zfar(1000.0f),
    rotations(0.0, 0.0),
    translations(-4.0f, -0.7f, -27.0f),
    rfactor(0.005f),
    tfactor(0.05f),
    sfactor(0.005f),
    state(Camera::NOTHING)
{
}

Camera::~Camera()
{
	
}

void Camera::mouseReleased() {
    state = Camera::NOTHING;
}

void Camera::modifierReleased() {
    if (state != Camera::NOTHING) {
        state = Camera::ROTATE;
    }
}

void Camera::mouseClicked(int x, int y, bool shift, bool ctrl, bool alt)
{
    mousePrev << x, y;
    if(shift) {
        state = Camera::TRANSLATE;
    } else if(ctrl) {
        state = Camera::SCALE;
    } else {
        state = Camera::ROTATE;
    }
}

void Camera::mouseMoved(int x, int y)
{
    Eigen::Vector2f mouseCurr(x, y);
    Eigen::Vector2f dv = mouseCurr - mousePrev;
    switch(state) {
    case Camera::ROTATE:
        rotations += rfactor * dv;
        break;
    case Camera::TRANSLATE:
        translations(0) += tfactor * dv(0);
        translations(1) -= tfactor * dv(1);
        break;
    case Camera::SCALE:
        translations(2) *= (1.0f - sfactor * dv(1));
        break;
    }
    mousePrev = mouseCurr;
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
    P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
    MV->rotate(rotations(1), Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    MV->rotate(rotations(0), Eigen::Vector3f(0.0f, 1.0f, 0.0f));
    MV->translate(translations);
}

void Camera::move(char c, const std::vector<Entity> &entities,
                  const Map &map, int col, int row) {
    Eigen::Quaternionf q1;
    Eigen::Vector3f axis1(0.0f, 1.0f, 0.0f);
    q1 = Eigen::AngleAxisf(rotations(0), axis1); 
    Eigen::Matrix4f R1 = Eigen::Matrix4f::Identity();
    R1.block<3,3>(0,0) = q1.toRotationMatrix();
    Eigen::Vector3f last_valid_location = translations;

    Eigen::Vector3f point;
    switch (c) {
    case 's':
        point = Eigen::Vector3f(0.0f, 0.0f, 1.0f);
        break;
    case 'd':
        point = Eigen::Vector3f(-1.0f, 0.0f, 0.0f);
        break;
    case 'w':
        point = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
        break;
    case 'a':
        point = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
        break;
    default:
        assert(0);
    }


    point = R1.block<3,3>(0,0) * point;      
    	
    translations(0) += (point(0) * tfactor);     
    translations(2) -= (point(2) * tfactor);

    if (col >= 0 && row >= 0) {
        Neighbors neighbors = map.getNearbyWalls(col, row);
        bool reset_x = false, reset_z = false;
        
        if ((neighbors.up && this->collides(*neighbors.up)) ||
            (neighbors.down && this->collides(*neighbors.down))) {
//            std::cout << "COLLIDES Z" << std::endl;
            reset_z = true;
        }
        
        if ((neighbors.left && this->collides(*neighbors.left)) ||
            (neighbors.right && this->collides(*neighbors.right))) {
//            std::cout << "COLLIDES X" << std::endl;
            reset_x = true;
        }

        for (auto it = entities.begin(); it != entities.end(); it++) {
            if (this->collides(*it)) {
                reset_x = reset_z = true;
                break;
            }
        }

        if (reset_x || reset_z) {
            translations = last_valid_location;

            if (reset_x) {
                point(0) = 0;
            }

            if (reset_z) {
                point(2) = 0;
            }

            translations(0) += (point(0) * tfactor);     
            translations(2) -= (point(2) * tfactor);
        }
    }

}

static float sq(float x) {
    return x*x;
}

bool Camera::collides(const Entity &e) {
//    return false;
    float cam_rad = collisionRadius();

    if (e.useBoundingBox()) {
        //      return true;
        BoundingBox bb = e.getBoundingBox();
        bb.box.min += e.getPosition();
        bb.box.max += e.getPosition();
        Eigen::Vector3f our_pos = -translations;
        if (bb.contains(our_pos)) {
            return true;
        }

        Eigen::Vector3f displacement = (bb.box.center() - our_pos);
        float distance = displacement.norm();
        Eigen::Vector3f direction = displacement.normalized();

        if (bb.contains(direction * cam_rad + our_pos)) {
            return true;
        }

        return false;
        
    }
    else {
        Eigen::Vector3f their_pos = e.getCenterWorld();
        Eigen::Vector3f our_pos = translations;
        their_pos(1) = 0;
        our_pos(1) = 0;
        Eigen::Vector3f delta = -their_pos - our_pos;
        return std::abs(delta.norm()) < cam_rad + e.getRadius();
    }
    
    
    /*
    std::cout << "object pos" << std::endl;
    std::cout << their_pos << std::endl;
    std::cout << "cam pos" << std::endl;
    std::cout << our_pos << std::endl;
    std::cout << " dist = " << std::abs(delta.norm()) << std::endl;
    */

    
}
