#include <queue>
#include <map>
#include <algorithm>
#include <set>


#include "Player.h"

Player::Player(int y, int x, char sym, std::string player_name)
    : Entity(y, x, sym), has_key(false), is_trapped(false), name(std::move(player_name)), current_goal{-1, -1} {}

bool Player::hasKey() const { return has_key; }
void Player::takeKey() { has_key = true; }
bool Player::isTrapped() const { return is_trapped; }
void Player::setTrapped(bool state) {
    if (state) {
        was_trapped = true;  // Καταγράφουμε ότι ΠΑΓΙΔΕΥΤΗΚΕ κάποια στιγμή
    }
    is_trapped = state;
}

bool Player::wasTrapped() const {
    return was_trapped;
}

std::string Player::getName() const { return name; }
const std::vector<std::string>& Player::getKnownMap() const { return known_map; }

void Player::initMemory(int height, int width) {
    known_map.assign(height, std::string(width, '?'));
}

void Player::updateMemory(const Map& map) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            Point p = {position.y + dy, position.x + dx};
            if (p.y >= 0 && p.y < (int)known_map.size() &&
                p.x >= 0 && p.x < (int)known_map[0].size()) {
                known_map[p.y][p.x] = map.getChar(p.y, p.x);
            }
        }
    }
}


std::vector<Point> Player::findPathBFS(const Point& start, const Point& goal, const std::vector<std::string>& world) {
    std::queue<Point> q;
    q.push(start);

    std::map<Point, Point> came_from;
    came_from[start] = start;

    int dy[] = {-1, 1, 0, 0};
    int dx[] = {0, 0, -1, 1};

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        if (current == goal) {
            std::vector<Point> path;
            while (current != start) {
                path.push_back(current);
                current = came_from[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            Point next = {current.y + dy[i], current.x + dx[i]};
            if (next.y >= 0 && next.y < (int)world.size() && next.x >= 0 && next.x < (int)world[0].size()
                && world[next.y][next.x] != '*'
                && world[next.y][next.x] != 'C'
                && world[next.y][next.x] != '?'
                && came_from.find(next) == came_from.end()) {
                came_from[next] = current;
                q.push(next);
            }
        }
    }
    return {};
}

void Player::decideMove(const Map& map, const Point* key_pos, const Point* cage_pos) {
    if (is_trapped) return;

    static std::set<Point> visited;

    // Αν αλλάξει στόχος, καθάρισε το path
    if (cage_pos && has_key && *cage_pos != current_goal) {
        current_goal = *cage_pos;
        current_path.clear();
    } else if (key_pos && !has_key && *key_pos != current_goal) {
        current_goal = *key_pos;
        current_path.clear();
    }

    // Αν δεν υπάρχει τρέχων στόχος, προσπάθησε να πας προς άγνωστο σημείο
    if (current_path.empty()) {
        if (current_goal.y != -1) {
            current_path = findPathBFS(position, current_goal, known_map);
            if (current_path.empty()) {
                current_goal = {-1, -1}; // δεν είναι προσβάσιμο
            }
        }

        // Αν δεν έχει στόχο, ψάξε κοντινό άγνωστο σημείο
        if (current_goal.y == -1) {
            Point best_unknown = {-1, -1};
            int min_dist = 10000;

            for (int y = 0; y < (int)known_map.size(); ++y) {
                for (int x = 0; x < (int)known_map[0].size(); ++x) {
                    Point p = {y, x};
                    if (known_map[y][x] == ' ' && isNextToUnknown(p) && visited.find(p) == visited.end()) {
                        int dist = manhattanDistance(position, p);
                        if (dist < min_dist) {
                            min_dist = dist;
                            best_unknown = p;
                        }
                    }
                }
            }

            if (best_unknown.y != -1) {
                current_path = findPathBFS(position, best_unknown, known_map);
            }
        }
    }

    // Εκτέλεσε επόμενο βήμα από το path
    if (!current_path.empty()) {
        Point next_pos = current_path.front();
        current_path.erase(current_path.begin());

        // Αν είναι αποδεκτό, μετακινήσου και σημείωσέ το
        if (known_map[next_pos.y][next_pos.x] != '*' && known_map[next_pos.y][next_pos.x] != 'C') {
            visited.insert(next_pos);
            setPosition(next_pos);
        }

        // Αν έφτασε στον στόχο
        if (position == current_goal) {
            current_goal = {-1, -1};
        }
    } else {
        // fallback: τυχαία κίνηση σε μη τοίχο
        int dy[] = {-1, 1, 0, 0};
        int dx[] = {0, 0, -1, 1};
        std::vector<Point> valid_moves;

        // ... fallback: τυχαία κίνηση ...
        for (int i = 0; i < 4; ++i) {
            Point next = {position.y + dy[i], position.x + dx[i]};
            // Προσθήκη ελέγχου για κλουβί
            bool is_cage = (map.getChar(next.y, next.x) == 'C');
            if (!map.isWall(next) && (!is_cage || has_key)) {
                valid_moves.push_back(next);
                }
            }
            // ...

        if (!valid_moves.empty()) {
            Point randMove = valid_moves[rand() % valid_moves.size()];
            visited.insert(randMove);
            setPosition(randMove);
        }
    }
}

bool Player::isNextToUnknown(const Point& p) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dy == 0 && dx == 0) continue;
            int ny = p.y + dy, nx = p.x + dx;
            if (ny >= 0 && ny < (int)known_map.size() && nx >= 0 && nx < (int)known_map[0].size()) {
                if (known_map[ny][nx] == '?') return true;
            }
        }
    }
    return false;
}
