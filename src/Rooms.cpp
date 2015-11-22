#include <Rooms.hpp>
#include <resources.hpp>
#include <cmath>
#include <GameMath.hpp>
#include <Camera.hpp>
#include <log.hpp>
#include <EntityDatabase.hpp>


extern EntityDatabase entityDatabase;

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
    rooms.push_back(new Comp());
    rooms.push_back(new Lounge());

    num_rooms = 3;
}

Room::Room()
{
    room_t = NONE;
    state_t = INACTIVE;
}

Room::~Room()
{

}

void Room::triggerRoom(vec2 comp) 
{
    if (comp == triggerPos && state_t == INACTIVE) {
        state_t = ACTIVE;
        switch(room_t) {
            case HANOI:
            LOG("ROOM HANOI NOW ACTIVE!!!");
            break;    
            case COMP:
            LOG("ROOM COMP NOW ACTIVE!!!");
            break;    
            case LOUNGE:
            LOG("ROOM LOUNGE NOW ACTIVE!!!");
            break; 
            default:
            LOG("ROOM UNKNOWN NOW ACTIVE!!!");
            break;
        }
    }
}

Hanoi::Hanoi() : Room()
{
    room_t = HANOI;
    yaml_bounds = "resources/hanoi_bounds.yaml";
    yaml_hanoi = "resources/hanoi.yaml";
    yaml_hanoi_room = "resources/hanoi_room.yaml";
    
    triggerPos = vec2(9, 26);

    init_entities_R(&boundaries, yaml_bounds);
    init_entities_R(&entities, yaml_hanoi_room);
    init_entities_R(&entities_mov, yaml_hanoi);

    std::sort(entities_mov.begin(), entities_mov.end(), sortHanoi);

    int i = 0, inc = 0;
    for (auto it = entities_mov.begin(); it != entities_mov.end(); it++) {
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

void Hanoi::select(GLFWwindow *window)
{
    if (state_t == ACTIVE) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                selected = 0;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                selected = 1;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
            if (selected == -1) {
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

void Hanoi::selection_helper() {
    if (select_idx != -1) {
        tube_loc[select_idx].top().selected = 0;
        entities_mov[tube_loc[select_idx].top().index].selected = false;
        if (tube_loc[selected].size() < SIZE) {
            if (tube_loc[selected].empty() || 
                 tube_loc[selected].top().size > 
                 tube_loc[select_idx].top().size) {

                tube_loc[selected].push(tube_loc[select_idx].top());
                tube_loc[select_idx].pop();
                newPos(selected);
            }
        }
        select_idx = -1;                    
    }
    else {
        select_idx = selected;

        if (!tube_loc[select_idx].empty()) {
            tube_loc[select_idx].top().selected = 1;
            entities_mov[tube_loc[select_idx].top().index].selected = true;
        }
        else {
            select_idx = -1;
        }
    }
}

void Hanoi::newPos(int idx_z)
{
    int idx_y = tube_loc[idx_z].size() - 1;
    vec3 old_pos = tube_loc[idx_z].top().pos;

    tube_loc[idx_z].top().pos = vec3(old_pos(0), pos_y[idx_y], pos_z[idx_z]);
    entities_mov[tube_loc[idx_z].top().index].setPosition(tube_loc[idx_z].top().pos);
}

void Hanoi::done()
{
    int i = size - 1;
    bool esc = false, success = false;
    std::stack<tube> temp;

    while (tube_loc[size-1].size() > 0 && !esc) {
        if (tube_loc[size-1].top().index == i--) {
            temp.push(tube_loc[size-1].top());
            tube_loc[size-1].pop();
            if (tube_loc[size-1].empty() && i == -1) {
                success = true;
            }
        }
        else {
            esc = true;
        }
    }

    while (temp.size() > 0) {
        tube_loc[size-1].push(temp.top());
        temp.pop();
    }

    if (!esc && success) {
        state_t = SUCCESS;
        boundaries.clear();
    }
}

Comp::Comp() : Room()
{
    room_t = COMP;
    yaml_bounds = "resources/comp_bounds.yaml";
    yaml_comp = "resources/comp.yaml";

    init_entities_R(&boundaries, yaml_bounds);
    init_entities_R(&entities, yaml_comp);

    triggerPos = vec2(20, 6);
}

Comp::~Comp()
{

}

Lounge::Lounge() : Room()
{
    room_t = LOUNGE;
    yaml_bounds = "resources/lounge_bounds.yaml";
    yaml_lounge = "resources/lounge.yaml";

    init_entities_R(&boundaries, yaml_bounds);
    init_entities_R(&entities, yaml_lounge);

    triggerPos = vec2(38, 12);
}

Lounge::~Lounge()
{

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

    for (int i = 0; i < entities->size(); i++) {
        entityDatabase.registerEntity(&(*entities)[i]);
    }
}
