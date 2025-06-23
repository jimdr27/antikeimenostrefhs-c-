#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "Item.h"
#include <vector>
#include <string>

class Game {
private:
    Map map;
    Player gregorakis;
    Player asimenia;
    Key key;
    Trap trap1, trap2;
    int turns_left;
    bool game_over;
    std::string end_message;

    Point* visible_key_pos;
    Point* visible_cage_pos;

    void setup(const std::string& map_file);
    void render();
    void update();
    void checkInteractions();
    void checkEndConditions();
    void happyEnd();
    void showGameOver();
	bool heroesReunited;


    bool isGame_over() const { return game_over; }
    std::string getMessage() const { return end_message; }


public:
    Game(const std::string& map_file);
    ~Game();
    void run();
};

#endif // GAME_H
