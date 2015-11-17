#ifndef _ROOMS_HPP_
#define _ROOMS_HPP_

#include <common.hpp>
#include <draw/Text.hpp>
#include <Entity.hpp>
#include <stack>

#define SIZE 3

class Level;
class Room;
class Hanoi;

class Level
{
public:
    Level();
    virtual ~Level();

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
        NONE
    };

    Room();
    virtual ~Room();

    State state_t;
    RoomType room_t;
    std::vector<Entity> boundaries; 
    std::vector<Entity> entities;
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
    std::string yaml_hanoi;
    std::vector<float> pos_z; 
    std::vector<float> pos_y; 
};

static bool sortHanoi(const Entity &a, const Entity &b) { return (a.getScale() > b.getScale()); }
static void init_entities_R(std::vector<Entity> *entities, std::string model_config_file);

#endif