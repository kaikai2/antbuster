#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <hge.h>

using namespace std;

class Entity
{
public:
    Entity(cAni::iAnimResManager &arm) : animResManager(arm), active(true)
    {
    }
    virtual void render(int time)
    {
        time;
    }
    virtual void step(float deltaTime)
    {
        deltaTime;
    }
    bool isActive() const
    {
        return active;
    }
protected:
    cAni::iAnimResManager &animResManager;

    bool active;
private:
    /// not allowed to assign it
    Entity &operator = (const Entity &o)
    {
        o;
        return *this;
    }
};

#endif