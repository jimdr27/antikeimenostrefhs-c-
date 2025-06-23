#include <ncurses.h>
#include <unistd.h>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include "Game.h"

Game::Game(const std::string& map_file)
    : gregorakis(0, 0, 'G', "Gregorakis"),
      asimenia(0, 0, 'S', "Asimenia"),
      turns_left(1000), game_over(false),
      visible_key_pos(nullptr), visible_cage_pos(nullptr) {


	heroesReunited = false;

    srand(time(NULL));

    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Τοίχοι
    init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Γρηγοράκης
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // Ασημένια
    init_pair(4, COLOR_GREEN, COLOR_BLACK);  // Σκάλα, Κλειδί
    init_pair(5, COLOR_RED, COLOR_BLACK);    // Παγίδες, Κλουβιά
    init_pair(6, COLOR_WHITE, COLOR_BLACK);  // Default

    setup(map_file);
}

Game::~Game() {
    endwin();
}

void Game::setup(const std::string& map_file) {
    if (!map.loadFromFile(map_file)) {
        throw std::runtime_error("Could not load map file: " + map_file);
    }

    std::vector<Point> empty_spaces = map.getEmptySpaces();
    std::random_shuffle(empty_spaces.begin(), empty_spaces.end());

    if (empty_spaces.size() < 5) {
         throw std::runtime_error("Not enough empty space on the map for all items.");
    }

    gregorakis.setPosition(empty_spaces.back());
    empty_spaces.pop_back();

    bool asimenia_placed = false;
    for (size_t i = 0; i < empty_spaces.size(); ++i) {
        Point p = empty_spaces[i];
        if (std::abs(p.y - gregorakis.getPosition().y) >= 7 ||
            std::abs(p.x - gregorakis.getPosition().x) >= 7) {
            asimenia.setPosition(p);
            empty_spaces.erase(empty_spaces.begin() + i);
            asimenia_placed = true;
            break;
        }
    }
    if(!asimenia_placed){
        throw std::runtime_error("Could not place Asimenia with the required distance.");
    }

    key.setPosition(empty_spaces.back());
    empty_spaces.pop_back();

    trap1.setPosition(empty_spaces.back());
    empty_spaces.pop_back();

    trap2.setPosition(empty_spaces.back());
    empty_spaces.pop_back();

    gregorakis.initMemory(map.getHeight(), map.getWidth());
    asimenia.initMemory(map.getHeight(), map.getWidth());
}

void Game::run() {
    while (!game_over) {
        render();
        update();
        usleep(150000);
    }
    showGameOver();
}

void Game::render() {
    clear();

    // Ζωγραφίζουμε τον λαβύρινθο (τοίχοι, σκάλα, διάδρομοι)
    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            char ch = map.getChar(y, x);
            if (ch == '*') {
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ACS_CKBOARD);
                attroff(COLOR_PAIR(1));
            } else if (ch == 'L') {
                attron(COLOR_PAIR(4));
                mvaddch(y, x, 'L');
                attroff(COLOR_PAIR(4));
            } else {
                mvaddch(y, x, ' ');
            }
        }
    }

    // Πλήρης απεικόνιση ΚΛΕΙΔΙΟΥ από την αρχή
    attron(COLOR_PAIR(4));
    mvaddch(key.getPosition().y, key.getPosition().x, key.getSymbol());
    attroff(COLOR_PAIR(4));

    // Πλήρης απεικόνιση ΠΑΓΙΔΩΝ και ΚΛΟΥΒΙΩΝ από την αρχή
    attron(COLOR_PAIR(5));
    mvaddch(trap1.getPosition().y, trap1.getPosition().x, trap1.getSymbol());
    mvaddch(trap2.getPosition().y, trap2.getPosition().x, trap2.getSymbol());
    attroff(COLOR_PAIR(5));

    // Ήρωες
    attron(COLOR_PAIR(2));
    mvaddch(gregorakis.getPosition().y, gregorakis.getPosition().x, gregorakis.getSymbol());
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    mvaddch(asimenia.getPosition().y, asimenia.getPosition().x, asimenia.getSymbol());
    attroff(COLOR_PAIR(3));

    // Στατιστικά
    attron(COLOR_PAIR(6));
    mvprintw(map.getHeight(), 0, "Turns Left: %d", turns_left);
    mvprintw(map.getHeight() + 1, 0, "%s: Pos(%d,%d) Key: %s Trapped: %s",
             gregorakis.getName().c_str(), gregorakis.getPosition().y, gregorakis.getPosition().x,
             gregorakis.hasKey() ? "Yes" : "No", gregorakis.isTrapped() ? "Yes" : "No");
    mvprintw(map.getHeight() + 2, 0, "%s: Pos(%d,%d) Key: %s Trapped: %s",
             asimenia.getName().c_str(), asimenia.getPosition().y, asimenia.getPosition().x,
             asimenia.hasKey() ? "Yes" : "No", asimenia.isTrapped() ? "Yes" : "No");
    attroff(COLOR_PAIR(6));

    refresh();
}


void Game::update() {
    turns_left--;

    gregorakis.updateMemory(map);
    asimenia.updateMemory(map);

    if (!gregorakis.hasKey() && !asimenia.hasKey()){

        Point k_pos = key.getPosition();
        if(manhattanDistance(gregorakis.getPosition(), k_pos) <=1 || manhattanDistance(asimenia.getPosition(), k_pos) <=1)
            visible_key_pos = &k_pos;
    } else {
        visible_key_pos = nullptr;
    }

    Point cage_pos = {-1, -1};
    if (!trap1.isActive()) cage_pos = trap1.getPosition();
    else if (!trap2.isActive()) cage_pos = trap2.getPosition();

    if (cage_pos.y != -1) {
        if(manhattanDistance(gregorakis.getPosition(), cage_pos) <=1 || manhattanDistance(asimenia.getPosition(), cage_pos) <=1)
            visible_cage_pos = &cage_pos;
    } else {
        visible_cage_pos = nullptr;
    }

    gregorakis.decideMove(map, visible_key_pos, visible_cage_pos);
    asimenia.decideMove(map, visible_key_pos, visible_cage_pos);

    checkInteractions();
    checkEndConditions();
}

void Game::checkInteractions() {
    Player* players[] = {&gregorakis, &asimenia};
    Trap* traps[] = {&trap1, &trap2};

    // --- Μέρος 1: Λήψη Κλειδιού και Ενεργοποίηση Παγίδας ---
    // Ελέγχουμε κάθε παίκτη ξεχωριστά για τις δικές του ενέργειες.
    for (Player* p : players) {

        // A. Έλεγχος για λήψη κλειδιού
        // (Η διόρθωση που κάναμε πριν - το κλειδί δεν είναι πλέον κοινό)
        if (!p->hasKey() && p->getPosition() == key.getPosition()) {
            p->takeKey();
        }

        // B. Έλεγχος για ενεργοποίηση παγίδας
        for (Trap* t : traps) {
            if (t->isActive() && p->getPosition() == t->getPosition()) {
                t->springTrap();
                p->setTrapped(true);
            }
        }
    }

    // --- Μέρος 2: Απελευθέρωση από Κλουβί ---
    // Αυτή η λογική ελέγχει την αλληλεπίδραση μεταξύ των δύο παικτών.
    // Δεν χρειάζεται να είναι μέσα στον παραπάνω βρόχο.
    Player* rescuer = &gregorakis;
    Player* trapped = &asimenia;

    for (int i = 0; i < 2; ++i) {
        // Ελέγχουμε όλες τις παγίδες για να βρούμε σε ποια είναι ο 'trapped' παίκτης.
        for (Trap* t : traps) {
            // Αν η παγίδα είναι κλουβί ('C') ΚΑΙ ο 'trapped' παίκτης είναι όντως παγιδευμένος σε αυτή...
            if (!t->isActive() && trapped->isTrapped() && trapped->getPosition() == t->getPosition()) {

                // ...τότε, ελέγχουμε αν ο 'rescuer' είναι στη θέση του κλουβιού και έχει το κλειδί.
                if (rescuer->getPosition() == t->getPosition() && rescuer->hasKey()) {

                    trapped->setTrapped(false);             // Απελευθερώνουμε τον παγιδευμένο.
                    map.setChar(t->getPosition(), ' ');   // Το κλουβί γίνεται ξανά διάδρομος.

                    // Αφού ανοίξαμε το κλουβί, μπορούμε να σταματήσουμε τον έλεγχο (break).
                    break;
                }
            }
        }
        // Ανταλλάσσουμε τους ρόλους και ελέγχουμε ξανά, για την περίπτωση που ο Γρηγοράκης ήταν παγιδευμένος.
        std::swap(rescuer, trapped);
    }
}

void Game::checkEndConditions() {

    //  Έλεγχος αν κάποιος έφτασε στον στόχο (goal) και ΔΕΝ είναι παγιδευμένος
    if (gregorakis.wasTrapped() && !gregorakis.isTrapped()) {
        game_over = true;
        end_message = "Η Ασημένια έσωσε τον Γρηγοράκη! Νίκη!";
    } else if (asimenia.wasTrapped() && !asimenia.isTrapped()) {
        game_over = true;
        end_message = "Ο Γρηγοράκης έσωσε την Ασημένια! Νίκη!";
    }

    //  Αν οι δύο παίκτες συναντηθούν και κανείς δεν είναι παγιδευμένος
    if (gregorakis.getPosition() == asimenia.getPosition() &&
        !gregorakis.isTrapped() && !asimenia.isTrapped()) {
        heroesReunited = true;
        happyEnd();
        game_over = true;
        end_message = "HAPPY END: They found each other! Love conquers all!";
        return;
    }

    if (turns_left <= 0) {
        game_over = true;
        end_message = "GAME OVER: Time ran out. The kingdom is lost.";
        return;
    }

    if (gregorakis.isTrapped() && asimenia.isTrapped()) {
        game_over = true;
        end_message = "GAME OVER: Both heroes are trapped. All hope is lost.";
        return;
    }
}

void Game::happyEnd() {

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            if (map.getChar(y, x) == '*') {
                map.setChar({y, x}, ' ');
                render();
                usleep(5000);
            }
        }
    }

    std::vector<Point> path_g = gregorakis.findPathBFS(gregorakis.getPosition(), map.getLadderPosition(), map.getLayout());
    std::vector<Point> path_a = asimenia.findPathBFS(asimenia.getPosition(), map.getLadderPosition(), map.getLayout());

    size_t max_len = std::max(path_g.size(), path_a.size());
    for(size_t i=0; i < max_len; ++i){
        if(i < path_g.size()) gregorakis.setPosition(path_g[i]);
        if(i < path_a.size()) asimenia.setPosition(path_a[i]);
        render();
        usleep(100000);
    }
}

void Game::showGameOver() {
    render();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(map.getHeight() / 2, (map.getWidth() - end_message.length()) / 2, "%s", end_message.c_str());
    attroff(COLOR_PAIR(5) | A_BOLD);

    mvprintw(map.getHeight() + 1, 0, "%s", end_message.c_str());

    mvprintw(map.getHeight() / 2, (map.getWidth() - end_message.length()) / 2, "%s", end_message.c_str());

    mvprintw(map.getHeight() / 2 + 2, (map.getWidth() - 20) / 2, "Press any key to exit");

    refresh();
    timeout(-1);
    getch();
}
