#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include "Point.h"

class Map {
private:
    std::vector<std::string> layout;
    int height;
    int width;
    Point ladderPosition;

public:
    Map();
    bool loadFromFile(const std::string& filename);
    int getHeight() const;
    int getWidth() const;
    char getChar(int y, int x) const;
    bool isWall(const Point& p) const;
    Point getLadderPosition() const;
    void setChar(const Point& p, char ch);
    std::vector<Point> getEmptySpaces() const;
    const std::vector<std::string>& getLayout() const;
};

#endif // MAP_H
