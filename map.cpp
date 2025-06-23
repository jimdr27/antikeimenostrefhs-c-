#include <fstream>
#include <stdexcept>
#include <ncurses.h>
#include "Map.h"

Map::Map() : height(0), width(0) {}

bool Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        layout.push_back(line);
        if (width == 0) {
            width = line.length();
        }
        // Βρίσκουμε τη σκάλα
        size_t ladder_x = line.find('L');
        if (ladder_x != std::string::npos) {
            ladderPosition = {static_cast<int>(layout.size() - 1), static_cast<int>(ladder_x)};
        }
    }
    height = layout.size();
    file.close();
    return true;
}

int Map::getHeight() const { return height; }
int Map::getWidth() const { return width; }

char Map::getChar(int y, int x) const {
    if (y >= 0 && y < height && x >= 0 && x < width) {
        return layout[y][x];
    }
    return '#';
}

bool Map::isWall(const Point& p) const {
    return getChar(p.y, p.x) == '*';
}

Point Map::getLadderPosition() const {
    return ladderPosition;
}

void Map::setChar(const Point& p, char ch) {
    if (p.y >= 0 && p.y < height && p.x >= 0 && p.x < width) {
        layout[p.y][p.x] = ch;
    }
}

std::vector<Point> Map::getEmptySpaces() const {
    std::vector<Point> emptySpaces;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layout[y][x] == ' ' || layout[y][x] == 'L') {
                emptySpaces.push_back({y, x});
            }
        }
    }
    return emptySpaces;
}

const std::vector<std::string>& Map::getLayout() const {
    return layout;
}
