#ifndef POINT_H
#define POINT_H

#include <cmath>

struct Point {
    int y, x;

    bool operator<(const Point& other) const {
        if (y < other.y) return true;
        if (y > other.y) return false;
        return x < other.x;
    }

    bool operator==(const Point& other) const {
        return y == other.y && x == other.x;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

inline int manhattanDistance(const Point& a, const Point& b) {
    return std::abs(a.y - b.y) + std::abs(a.x - b.x);
}

#endif // POINT_H