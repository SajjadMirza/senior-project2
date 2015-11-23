#ifndef _ROOMS_HPP_
#define _ROOMS_HPP_

#include <common.hpp>
#include <draw/Text.hpp>
#include <draw/Texture.hpp>
#include <Entity.hpp>
#include <stack>

#include <boost/filesystem.hpp>

#define SIZE 3

class Level;
class Room;
class Hanoi;
class Comp;

class Level
{
public:
    Level();
    ~Level();

    void initLevelOne();
    int getNumRooms() const { return num_rooms; }
    std::vector<Room*> getRooms() const { return rooms; }
private:
    int num_rooms;
    std::vector<Room*> rooms; 
};

class Room
{
public:
    enum State
    {
        INACTIVE,
        ACTIVE,
        SUCCESS,
        FAILURE
    };

    enum RoomType
    {
        HANOI,
        COMP,
        LOUNGE,
        NONE
    };

    Room();
    virtual ~Room();

    State state_t;
    RoomType room_t;
    std::vector<Entity> boundaries; 
    std::vector<Entity> entities;
    std::vector<Entity> entities_mov;
    std::string yaml_bounds;

    vec2 triggerPos;
    void triggerRoom(vec2 comp);
private:

};

class Hanoi : public Room
{
public:
    typedef struct 
    {
        int index;
        vec3 pos;
        float size;
        int selected;
    }tube;

    Hanoi();
    ~Hanoi();

    std::vector< std::stack<tube> > tube_loc;

    void select(GLFWwindow *window);
    void selection_helper();
    void newPos(int idx_z);
    void done();
private:
    int selected;
    int select_idx;
    int size;
    std::string yaml_hanoi_room;
    std::string yaml_hanoi;
    std::vector<float> pos_z; 
    std::vector<float> pos_y; 
};

class Comp : public Room
{
public:
    typedef struct 
    {
        std::string name;
        std::string terminal_txt;
    } labtop_screen;

    Comp();
    ~Comp();

    int select(GLFWwindow *window);
    int terminal_idx();
    int terminal_idx(int num);
    int up_level();

    std::vector<labtop_screen> lt_screen_list;
private:
    int home_idx(int num);
    int access_idx(int num);
    int root_idx(int num);
    int home_root_idx(int num);

    std::string yaml_comp;
    int computer_idx;
    int term_idx;
    bool root;
};

class Lounge : public Room
{
public:
    Lounge();
    ~Lounge();
private:
    std::string yaml_lounge;
};

static bool sortHanoi(const Entity &a, const Entity &b) { return (a.getScale() > b.getScale()); }
static void init_entities_R(std::vector<Entity> *entities, std::string model_config_file);
static void get_all(const boost::filesystem::path& root, const std::string& ext, std::vector<boost::filesystem::path>& ret);

#endif