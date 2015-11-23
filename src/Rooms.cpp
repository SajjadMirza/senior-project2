#include <Rooms.hpp>
#include <resources.hpp>
#include <cmath>
#include <GameMath.hpp>
#include <Camera.hpp>
#include <log.hpp>
#include <EntityDatabase.hpp>


extern EntityDatabase entityDatabase;

#include <string>
#include <fstream>
#include <streambuf>

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

    computer_idx = -1;
    term_idx = -1;
    root = false;

    int i = 0;
    for (auto it = entities.begin(); it != entities.end(); it++) {
        Entity e = *it;
        if (e.getName() == "Computer") {
            computer_idx = i;
        }
        ++i;
    }

    triggerPos = vec2(20, 6);

    /* Manually setup textures */
    std::string header = "resources/text/";

    boost::filesystem::path p(header);
    std::string ext = ".txt";
    std::vector<boost::filesystem::path> files;

    get_all(p, ext, files);

    for (int i = 0; i < files.size(); ++i) {
        std::string file_name = header + files[i].string();

        std::ifstream t(file_name);

        labtop_screen ls;

        ls.name = file_name.substr(header.size(), file_name.size());

        std::string txt((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

        ls.terminal_txt = txt;

        lt_screen_list.push_back(ls);
    }

    /*for(int i = 0; i < lt_screen_list.size(); ++i) {
        LOG("Number: " << i);
        LOG("Name: " << lt_screen_list[i].name);
        LOG("Text: \n" << lt_screen_list[i].terminal_txt);
        LOG("End of current file");
    }*/
}

Comp::~Comp()
{
    
}

int Comp::select(GLFWwindow *window)
{
    if (state_t == ACTIVE || state_t == SUCCESS) {
        if (entities[computer_idx].selected) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

int Comp::terminal_idx()
{
    if (term_idx == -1) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "home.txt") {
                term_idx = i;
                return i;
            }
        }
    }
    else {
        return term_idx;
    }
}

int Comp::terminal_idx(int num) 
{
    std::string search = lt_screen_list[term_idx].name;
    if (search == "home.txt") {
        return home_idx(num);
    }
    else if (search == "access_manual_contents.txt") {
        return access_idx(num);
    }
    else if (search == "root.txt") {
        return root_idx(num);
    }
    else if (search == "root_succeed.txt") {
        return home_root_idx(num);
    }
    return term_idx;
}

int Comp::up_level()
{
    std::string search = "";

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (i == term_idx) {
            search = lt_screen_list[i].name;
            i = lt_screen_list.size();
        }
    }

    if (search == "root_facility_purpose.txt" ||
        search == "root_employee.txt" ||
        search == "root_message.txt" ||
        search == "root_experiment.txt" ||
        search == "root_lights.txt") 
    {
        search = "root_succeed.txt";
    }
    else if (search == "facility_purpose.txt" ||
        search == "employee.txt" ||
        search == "map.txt" ||
        search == "access_manual_contents.txt" ||
        search == "message.txt" ||
        search == "root.txt") 
    {
        if (!root) {
            search = "home.txt";
        }
        else {
            search = "root_succeed.txt";
        }
    }
    else if (search == "access_manual_1.txt" ||
             search == "access_manual_2.txt" ||
             search == "access_manual_3.txt" ||
             search == "access_manual_4.txt" ||
             search == "access_manual_5.txt")
    {
        search = "access_manual_contents.txt";
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            term_idx = i;
            return i;
        }
    }
}

int Comp::home_idx(int num)
{
    std::string search = "";
    switch(num) 
    {
        case 1:
            search = "facility_purpose.txt";
            break;
        case 2:
            search = "employee.txt";
            break;
        case 3:
            search = "map.txt";
            break;
        case 4:
            search = "access_manual_contents.txt";
            break;
        case 5:
            search = "message.txt";
            break;
        case 6:
            search = "root.txt";
            break;
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            term_idx = i;
            return i;
        }
    }

    return term_idx;   
}

int Comp::access_idx(int num)
{
    std::string search = "";
    switch(num) 
    {
        case 1:
            search = "access_manual_1.txt";
            break;
        case 2:
            search = "access_manual_2.txt";
            break;
        case 3:
            search = "access_manual_3.txt";
            break;
        case 4:
            search = "access_manual_4.txt";
            break;
        case 5:
            search = "access_manual_5.txt";
            break;
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            term_idx = i;
            return i;
        }
    }

    return term_idx;   
}

int Comp::root_idx(int num)
{
    std::string search = "";
    switch(num) 
    {
        case 1:
            search = "root_fail.txt";
            break;
        case 2:
            search = "root_fail.txt";
            break;
        case 3:
            search = "root_fail.txt";
            break;
        case 4:
            search = "root_succeed.txt";
            root = true;
            break;
        case 5:
            search = "root_fail.txt";
            break;
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            term_idx = i;
            return i;
        }
    }

    return term_idx;   
}

int Comp::home_root_idx(int num)
{
    std::string search = "";
    switch(num) 
    {
        case 1:
            search = "root_facility_purpose.txt";
            break;
        case 2:
            search = "root_employee.txt";
            break;
        case 3:
            search = "map.txt";
            break;
        case 4:
            search = "access_manual_contents.txt";
            break;
        case 5:
            search = "root_message.txt";
            break;
        case 6:
            search = "root_experiment.txt";
            break;
        case 7:
            search = "root_lights.txt";
            break;
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            term_idx = i;
            return i;
        }
    }

    return term_idx;   
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

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
void get_all(const boost::filesystem::path& root, const std::string& ext, std::vector<boost::filesystem::path>& ret)
{
    if(!boost::filesystem::exists(root) || !boost::filesystem::is_directory(root)) return;

    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;

    while(it != endit)
    {
        if(boost::filesystem::is_regular_file(*it) && it->path().extension() == ext) ret.push_back(it->path().filename());
        ++it;

    }

}

