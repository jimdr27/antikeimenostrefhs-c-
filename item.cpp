#include "Item.h"
Key::Key(int y, int x) : Entity(y, x, 'K') {}

Trap::Trap(int y, int x) : Entity(y, x, 'T'), is_active(true) {}
bool Trap::isActive() const { return is_active; }
void Trap::springTrap() {
    is_active = false;
    symbol = 'C';
}

