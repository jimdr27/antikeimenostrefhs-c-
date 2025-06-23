#ifndef ENTITY_H
#define ENTITY_H

#include "Point.h"

class Entity {
protected:
    Point position;
    char symbol;

public:
    Entity(int y, int x, char sym);
    virtual ~Entity();

    Point getPosition() const;
    char getSymbol() const;
    void setPosition(const Point& newPos);
};

#endif // ENTITY_H
