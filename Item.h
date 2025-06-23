#ifndef ITEM_H
#define ITEM_H

#include "Entity.h"

class Key : public Entity {
public:
    Key(int y = -1, int x = -1);
};

class Trap : public Entity {
private:
    bool is_active;

public:
    Trap(int y = -1, int x = -1);
    bool isActive() const;
    void springTrap();
};

#endif // ITEM_H
