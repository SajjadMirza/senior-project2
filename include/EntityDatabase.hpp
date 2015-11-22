#ifndef _ENTITY_DATABASE_HPP_
#define _ENTITY_DATABASE_HPP_

#include <Entity.hpp>

#include <map>

class EntityDatabase {
    static EntityID id_counter;
    std::map<EntityID, Entity*> db;
public:
    void registerEntity(Entity *entity);
    Entity *lookup(EntityID id);
};

#endif
