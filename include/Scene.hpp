#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <vector>
#include <Entity.hpp>

typedef std::vector<Entity> EntityVector;
typedef std::vector<Entity*> EntityPtrVector;

struct Scene {
    std::vector<EntityVector*> entityVectors;
    std::vector<EntityPtrVector*> entityPtrVectors;
}

#endif
