#include <iostream>
#include <stdexcept>
#include <ncurses.h>

#include "Game.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <map_file.txt>" << std::endl;
        return 1;
    }

    try {
        Game game(argv[1]);
        game.run();
    } catch (const std::runtime_error& e) {

        if (isendwin() == FALSE) {
            endwin();
        }
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}