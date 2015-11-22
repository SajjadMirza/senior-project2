#include <EntityDatabase.hpp>

#include <log.hpp>

EntityID EntityDatabase::id_counter = 0;



void EntityDatabase::registerEntity(Entity *entity)
{
    if (entity->id == 0) {
        entity->id = ++id_counter;
        LOG("GENERATED NEW ID FOR ENTITY: " << (entity->id) << " @ " << entity);
    }

    db.insert(std::map<EntityID, Entity*>::value_type(entity->id, entity));
    LOG("REGISTERED NEW ENTITY: " << (entity->id) << " "  << entity->getName() << " @ " 
        << entity);
}

Entity *EntityDatabase::lookup(EntityID id)
{
    auto search = db.find(id);
    if (search != db.end()) {
        return search->second;
    }
    else {
        return NULL;
    }
}
