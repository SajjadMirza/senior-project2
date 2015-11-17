#include <Rooms.hpp>
#include <resources.hpp>
#include <cmath>
#include <GameMath.hpp>
#include <Camera.hpp>
#include <log.hpp>

Level::Level()
{
    num_rooms = 0;
}

Level::~Level()
{
    for (int i; i < num_rooms; ++i) {
        delete rooms[i];
    }
}

void Level::initLevelOne()
{
    rooms.push_back(new Hanoi());

    num_rooms = 1;
}

Room::Room()
{
    room_t = NONE;
    state_t = INACTIVE;
    yaml_bounds = "resources/hanoi_bounds.yaml";

    init_entities_R(&boundaries, yaml_bounds);
}

Room::~Room()
{

}

void Room::triggerRoom(vec2 comp) 
{
    if (comp == triggerPos && state_t == INACTIVE) {
        state_t = ACTIVE;
        LOG("ROOM HANOI NOW ACTIVE!!!");
    }
}

Hanoi::Hanoi() : Room()
{
    room_t = HANOI;
    yaml_hanoi = "resources/hanoi.yaml";

    triggerPos = vec2(9, 26);

    init_entities_R(&entities, yaml_hanoi);

    std::sort(entities.begin(), entities.end(), sortHanoi);

    int i = 0, inc = 0;
    for (auto it = entities.begin(); it != entities.end(); it++) {
        tube temp;
        temp.index = i++;
        temp.pos = it->getPosition();
        temp.size = it->getScale();
        temp.selected = 0;

        pos_z.push_back(temp.pos(2) - inc++);
        pos_y.push_back(temp.pos(1));

        tube_loc.push_back(std::stack<tube>());

        tube_loc[0].push(temp);
    }
    selected = -1;
    select_idx = -1;
    size = i;

    if (size != 3) {
        LOG("ERROR, SIZE FOR HANOI PUZZLE MUST BE 3");
        exit(-1);
    }
}

Hanoi::~Hanoi()
{

}

void Hanoi::selection_helper() {
    if (select_idx != -1) {
        LOG("UNSELECT TOP");
        tube_loc[select_idx].top().selected = 0;
        entities[tube_loc[select_idx].top().index].selected = false;
        if (tube_loc[selected].size() < SIZE) {
            LOG("AND MOVED");
            tube_loc[selected].push(tube_loc[select_idx].top());
            tube_loc[select_idx].pop();
            newPos(selected);
        }
        select_idx = -1;                    
    }
    else {
        select_idx = selected;

        if (!tube_loc[select_idx].empty()) {
            LOG("SELECTED TOP");
            tube_loc[select_idx].top().selected = 1;
            entities[tube_loc[select_idx].top().index].selected = true;
        }
        else {
            LOG("NO TOP");
            select_idx = -1;
        }
    }
}

void Hanoi::newPos(int idx_z)
{
    int idx_y = tube_loc[idx_z].size() - 1;
    vec3 old_pos = tube_loc[idx_z].top().pos;

    tube_loc[idx_z].top().pos = vec3(old_pos(0), pos_y[idx_y], pos_z[idx_z]);
    entities[tube_loc[idx_z].top().index].setPosition(tube_loc[idx_z].top().pos);
}

void Hanoi::select(GLFWwindow *window)
{
    if (state_t == ACTIVE) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("PRESSED [");
                selected = 0;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("PRESSED ]");
                selected = 1;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("PRESSED \\");                
                selected = 2;
                selection_helper();
            }
        }
        selected = -2;

        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE &&
            glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_RELEASE &&
            glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_RELEASE) {
            selected = -1;
        } 
    }
}

static void init_entities_R(std::vector<Entity> *entities, std::string model_config_file) 
{
    std::vector<ModelConfig> configs;
    resource::load_model_configs(&configs, model_config_file);
    
    for (auto it = configs.begin(); it != configs.end(); it++) {
        draw::Drawable *drawable = new draw::Drawable(*it);
        Entity e(drawable);
        e.calculate_center_and_radius();
        
        if (it->radius_override) {
            std::cout << "radius override detected" << std::endl;
            e.setRadius(it->radius_override.get());
        }
        
        if (it->use_position_center_override) {
            e.setCenter(Eigen::Vector3f(0,0,0));
        }

        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        Eigen::Quaternionf qrot = Eigen::Quaternionf::Identity();
        
        if (it->transforms.xrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.xrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX());
            qrot = q * qrot;
        }

        if (it->transforms.yrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.yrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY());
            qrot = q * qrot;
        }

        if (it->transforms.zrot != 0.0f) {
            std::cout << "multiplied by z rotation" << std::endl;
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.zrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ());
            qrot = q * qrot;
        }
        rot.topLeftCorner<3,3>() = qrot.toRotationMatrix();
        e.setRotationMatrix(rot);

        if (it->transforms.scale != 0.0f) {
            e.setScale(it->transforms.scale);
        }

        Eigen::Vector3f pos(0,0,0);

        if (it->transforms.xpos != 0.0f) {
            pos(0) = it->transforms.xpos;
        }

        if (it->transforms.ypos != 0.0f) {
            pos(1) = it->transforms.ypos;
        }

        if (it->transforms.zpos != 0.0f) {
            pos(2) = it->transforms.zpos;
        }

        e.setPosition(pos);
        e.setName(it->model);
        
        entities->push_back(e);
    }
}