#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Map.h"
#include <vector>
#include <string>


class Player : public Entity {
private:
    bool has_key;
    bool is_trapped;
    bool was_trapped = false;
    std::string name;

    std::vector<std::string> known_map;

    Point current_goal;
    std::vector<Point> current_path;

    void findNewPath(const Map& map);

public:
    Player(int y, int x, char sym, std::string player_name);

    void initMemory(int height, int width);
    void updateMemory(const Map& map);

    void decideMove(const Map& map, const Point* key_pos, const Point* cage_pos);

    bool hasKey() const;
    void takeKey();

    bool isTrapped() const;
    void setTrapped(bool state);

    bool wasTrapped() const;

    std::string getName() const;
    const std::vector<std::string>& getKnownMap() const;

    std::vector<Point> findPathBFS(const Point& start, const Point& goal, const std::vector<std::string>& world);
    bool isNextToUnknown(const Point& p);

};

#endif // PLAYER_H

