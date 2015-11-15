#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#include <Gbuffer.hpp>
#include <vector>

class DeferredRenderer
{
    Gbuffer gbuffer;
    std::vector<Entity*> objects;
public:
    void attachEntity(Entity *entity);
    void attachEntity
    void render();
};

}

#endif
