#include "Entity.h"

Entity::Entity(int y, int x, char sym) : position{y, x}, symbol(sym) {}

Entity::~Entity() {}

Point Entity::getPosition() const { return position; }

char Entity::getSymbol() const { return symbol; }

void Entity::setPosition(const Point& newPos) { position = newPos; }
