#ifndef _ROOMS_HPP_
#define _ROOMS_HPP_

#include <common.hpp>
#include <draw/Text.hpp>
#include <Entity.hpp>

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

    State curr;
    RoomType room_t;
    std::vector<Entity> boundaries; 
    std::vector<Entity> entities;
    std::string yaml_bounds;
private:

};

class Hanoi : public Room
{
public:
    Hanoi();
    ~Hanoi();

private:
    std::string yaml_hanoi;

};

static void init_entities_R(std::vector<Entity> *entities, std::string model_config_file);

#endif