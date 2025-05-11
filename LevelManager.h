#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H
#include <SDL.h>
#include "graphic.h"

// Forward declaration của class Game
class Game;

class LevelManager {
public:
    int currentLevel = 1;

    void loadLevel(Game &game);
    bool isLastLevel();
    void nextLevel();
    void reset();
};
#endif // LEVELMANAGER_H
