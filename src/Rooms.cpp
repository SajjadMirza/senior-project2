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

void Hanoi::select(GLFWwindow *window, Entity *last_selected_entity)
{
    if (state_t == ACTIVE) {
        if (last_selected_entity != NULL) {
            if (last_selected_entity->getName() == "Switch_1" && last_selected_entity->selected == true) {    
                if (selected == -1) {
                    selected = 0;
                    selection_helper();
                    last_selected_entity->selected = false;
                }
            }
            else if (last_selected_entity->getName() == "Switch_2" && last_selected_entity->selected == true) {
                if (selected == -1) {
                    selected = 1;
                    selection_helper();
                    last_selected_entity->selected = false;
                }
            }
            else if (last_selected_entity->getName() == "Switch_3" && last_selected_entity->selected == true) {
                if (selected == -1) {
                    selected = 2;
                    selection_helper();
                    last_selected_entity->selected = false;
                }
            }

        selected = -2;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            selected = -1;
        } 
    }
}

void Hanoi::select(GLFWwindow *window)
{
    if (state_t == ACTIVE) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("Switch 1");
                selected = 0;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("Switch 2");
                selected = 1;
                selection_helper();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
            if (selected == -1) {
                LOG("Switch 3");
                selected = 2;
                selection_helper();
            }
        }
        selected = -2;

        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE &&
            glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_RELEASE &&
            glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_RELEASE) {
            selected = -1;
            LOG("off");
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
        for (int i = 0; i < boundaries.size(); ++i) {
            boundaries[i].setPosition(vec3(0, 0, 0));
        }
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
    
    term_idx = -1;
    term_idx_s1 = -1;
    term_idx_s2 = -1;
    term_idx_s3 = -1;
    term_idx_s4 = -1;
    term_idx_s5 = -1;
    root = false;

    for (int i = 0; i < 5; ++i) {
        exit_list.push_back(false);
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
}

Comp::~Comp()
{
    
}

int Comp::select(Entity *last_selected_entity)
{
    if (state_t == ACTIVE || state_t == SUCCESS) {
        if (last_selected_entity != NULL && last_selected_entity->selected) {
            if (last_selected_entity->getName() == "Computer") {
                return LABTOP;
            }
            else if (last_selected_entity->getName() == "computer_server_1") {
                return COMP_SERVER_1;
            }
            else if (last_selected_entity->getName() == "computer_server_2") {
                return COMP_SERVER_2;
            }
            else if (last_selected_entity->getName() == "computer_server_3") {
                return COMP_SERVER_3;
            }
            else if (last_selected_entity->getName() == "computer_server_4") {
                return COMP_SERVER_4;
            }
            else if (last_selected_entity->getName() == "computer_server_5") {
                return COMP_SERVER_5;
            }
            else {
                return 0;
            }
        }
        else {
            return 0;
        }
    }
}

int Comp::terminal_idx_h(int disable_controls)
{
    if (term_idx == -1 && disable_controls == LABTOP) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "home.txt") {
                term_idx = i;
                return i;
            }
        }
    }
    else if (term_idx_s1 == -1 && disable_controls == COMP_SERVER_1) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "access_question_1.txt") {
                term_idx_s1 = i;
                return i;
            }
        }
    }
    else if (term_idx_s2 == -1 && disable_controls == COMP_SERVER_2) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "access_question_2.txt") {
                term_idx_s2 = i;
                return i;
            }
        }
    }
    else if (term_idx_s3 == -1 && disable_controls == COMP_SERVER_3) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "access_question_3.txt") {
                term_idx_s3 = i;
                return i;
            }
        }
    }
    else if (term_idx_s4 == -1 && disable_controls == COMP_SERVER_4) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "access_question_4.txt") {
                term_idx_s4 = i;
                return i;
            }
        }
    }
    else if (term_idx_s5 == -1 && disable_controls == COMP_SERVER_5) {
        for (int i = 0; i < lt_screen_list.size(); ++i) {
            if (lt_screen_list[i].name == "access_question_5.txt") {
                term_idx_s5 = i;
                return i;
            }
        }
    }

    if (disable_controls == LABTOP) {
        return term_idx; 
    }
    if (disable_controls == COMP_SERVER_1) {
        return term_idx_s1; 
    }
    if (disable_controls == COMP_SERVER_2) {
        return term_idx_s2; 
    }
    if (disable_controls == COMP_SERVER_3) {
        return term_idx_s3; 
    }
    if (disable_controls == COMP_SERVER_4) {
        return term_idx_s4; 
    }
    if (disable_controls == COMP_SERVER_5) {
        return term_idx_s5; 
    }

}

int Comp::terminal_idx(int disable_controls, int num) 
{
    std::string search;
    switch(disable_controls) 
    {
        case LABTOP:
            search = lt_screen_list[term_idx].name;
            break;
        case COMP_SERVER_1:
            search = lt_screen_list[term_idx_s1].name;
            break;
        case COMP_SERVER_2:
            search = lt_screen_list[term_idx_s2].name;
            break;
        case COMP_SERVER_3:
            search = lt_screen_list[term_idx_s3].name;
            break;
        case COMP_SERVER_4:
            search = lt_screen_list[term_idx_s4].name;
            break;
        case COMP_SERVER_5:
            search = lt_screen_list[term_idx_s5].name;
            break;
    }

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
    else if (search == "access_question_1.txt") {
        return server_answer(num, COMP_SERVER_1);
    }
    else if (search == "access_question_2.txt") {
        return server_answer(num, COMP_SERVER_2);
    }
    else if (search == "access_question_3.txt") {
        return server_answer(num, COMP_SERVER_3);
    }
    else if (search == "access_question_4.txt") {
        return server_answer(num, COMP_SERVER_4);
    }
    else if (search == "access_question_5.txt") {
        return server_answer(num, COMP_SERVER_5);
    }
    return term_idx;
}

int Comp::up_level(int disable_controls)
{
    std::string search = "";

    int num;
    switch(disable_controls) 
    {
        case LABTOP:
            num = term_idx;
            break;
        case COMP_SERVER_1:
            num = term_idx_s1;
            break;
        case COMP_SERVER_2:
            num = term_idx_s2;
            break;
        case COMP_SERVER_3:
            num = term_idx_s3;
            break;
        case COMP_SERVER_4:
            num = term_idx_s4;
            break;
        case COMP_SERVER_5:
            num = term_idx_s5;
            break;
    }

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (i == num) {
            search = lt_screen_list[i].name;
            i = lt_screen_list.size();
        }
    }

    if (search == "access_fail.txt") {
        switch(disable_controls) 
        {
            case COMP_SERVER_1:
                search = "access_question_1.txt";
                break;
            case COMP_SERVER_2:
                search = "access_question_2.txt";
                break;
            case COMP_SERVER_3:
                search = "access_question_3.txt";
                break;
            case COMP_SERVER_4:
                search = "access_question_4.txt";
                break;
            case COMP_SERVER_5:
                search = "access_question_5.txt";
                break;
        }
    }
    else if (search == "root_facility_purpose.txt" ||
        search == "root_employee.txt" ||
        search == "root_message.txt" ||
        search == "root_experiment.txt" ||
        search == "root_lights.txt" ||
        search == "root_map.txt") 
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
            switch(disable_controls) 
            {
                case LABTOP:
                    term_idx = i;
                    break;
                case COMP_SERVER_1:
                    term_idx_s1 = i;
                    break;
                case COMP_SERVER_2:
                    term_idx_s2 = i;
                    break;
                case COMP_SERVER_3:
                    term_idx_s3 = i;
                    break;
                case COMP_SERVER_4:
                    term_idx_s4 = i;
                    break;
                case COMP_SERVER_5:
                    term_idx_s5 = i;
                    break;
            }
            return i;
        }
    }
}

void Comp::done(int disable_controls)
{
    bool esc = true;

    if (disable_controls == 0 && state_t == Room::State::ACTIVE) {
        for (int i = 0; i < exit_list.size(); ++i) {
            if (exit_list[i] == false) {
                esc = false;
                i = exit_list.size();
            }
        }

        if (esc) {
            state_t = SUCCESS;
            for (int i = 0; i < boundaries.size(); ++i) {
                boundaries[i].setPosition(vec3(0, 0, 0));
            }
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
        case 8:
            search = "root_map.txt";

            for (int i = 0; i < entities.size(); ++i) {
                if (entities[i].getName() == "Map") {
                    vec3 temp = entities[i].getPosition();
                    entities[i].setPosition(vec3(temp(0), 0.45, temp(2)));
                }
            }

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

int Comp::server_answer(int num, int sev)
{
    std::string search = "";
    std::vector<bool> answers;
    switch(sev) 
    {
        case COMP_SERVER_1:
            answers = { false, false, true, false, false, false };
            break;
        case COMP_SERVER_2:
            answers = { false, false, false, true, false, false };
            break;
        case COMP_SERVER_3:
            answers = { false, false, false, false, false, true };
            break;
        case COMP_SERVER_4:
            answers = { true, false, false, false, false, false };
            break;
        case COMP_SERVER_5:
            answers = { false, true, false, false };
            break;
    }

    if (num - 1 < answers.size()) { 
        if (answers[num - 1] == true) {
            search = "access_succeed.txt";
            exit_list[sev - COMP_SERVER_1] = true;
        }
        else {
            search = "access_fail.txt";
        }
    }
    

    for (int i = 0; i < lt_screen_list.size(); ++i) {
        if (lt_screen_list[i].name == search) {
            switch(sev) 
            {
                case COMP_SERVER_1:
                    term_idx_s1 = i; 
                    break;
                case COMP_SERVER_2:
                    term_idx_s2 = i;         
                    break;
                case COMP_SERVER_3:
                    term_idx_s3 = i;         
                    break;
                case COMP_SERVER_4:
                    term_idx_s4 = i;         
                    break;
                case COMP_SERVER_5:
                    term_idx_s5 = i;         
                    break;
            }
            return i;
        }
    }

    switch(sev) 
    {
        case COMP_SERVER_1:
            return term_idx_s1; 
            break;
        case COMP_SERVER_2:
            return term_idx_s2;         
            break;
        case COMP_SERVER_3:
            return term_idx_s3;         
            break;
        case COMP_SERVER_4:
            return term_idx_s4;         
            break;
        case COMP_SERVER_5:
            return term_idx_s5;         
            break;
    }
}

Lounge::Lounge() : Room()
{
    room_t = LOUNGE;
    yaml_bounds = "resources/lounge_bounds.yaml";
    yaml_lounge = "resources/lounge.yaml";

    init_entities_R(&boundaries, yaml_bounds);
    init_entities_R(&entities, yaml_lounge);

    triggerPos = vec2(38, 12);

    Light_state = LIGHT_W;
}

Lounge::~Lounge()
{

}

void Lounge::select(Entity *last_selected_entity) 
{
    if (last_selected_entity != NULL) {
        if (last_selected_entity->getName() == "book_special" && last_selected_entity->selected == true) {
            for (int i = 0; i < entities.size(); ++i) {
                if (entities[i].getName() == "book_special") {
                    entities[i].setPosition(vec3(0, 0, 0));
                    last_selected_entity->selected = false;
                    return;
                }
            }
        }
        else if (last_selected_entity->getName() == "switch_special" && last_selected_entity->selected == true) {
            for (int i = 0; i < entities.size(); ++i) {
                if (entities[i].getName() == "switch_special") {
                    Light_state = (Light_state + 1) % 4;

                    /* Switch Portraits */
                    if (Light_state == 3 || Light_state == 0) {
                        int inc = 0;
                        if (Light_state == 3) {
                            inc = 1;
                        }

                        if (Light_state == 0) {
                            inc = -1;
                        }

                        for (int i = 0; i < entities.size(); ++i) {
                            vec3 temp;
                            if (entities[i].getName() == "Picture_1") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0), temp(1), temp(2) + inc));
                            }
                            else if (entities[i].getName() == "Picture_2") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) + inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_3") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) + inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_4") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) + inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_5") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0), temp(1), temp(2) + inc));
                            }
                            else if (entities[i].getName() == "Picture_1_special") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0), temp(1), temp(2) - inc));
                            }
                            else if (entities[i].getName() == "Picture_2_special") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) - inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_3_special") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) - inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_4_special") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0) - inc, temp(1), temp(2)));
                            }
                            else if (entities[i].getName() == "Picture_5_special") {
                                temp = entities[i].getPosition();
                                entities[i].setPosition(vec3(temp(0), temp(1), temp(2) - inc));
                            }
                        }
                    }

                    last_selected_entity->selected = false;
                    return;
                }
            }
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

