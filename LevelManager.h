#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H
#include <SDL.h>
#include "graphic.h"

// Forward declaration thay vì include nhanvat.h
class Game;

class LevelManager {
public:
    int currentLevel = 1;

    void loadLevel(Game &game) {
        game.obstacles.clear();

        if (currentLevel == 1) {
            game.hole_x = 700.0f;
            game.hole_y = 500.0f;
            game.obstacles.push_back({600.0f, 200.0f, 50.0f, 200.0f});
        } else if (currentLevel == 2) {
            game.hole_x = 600.0f;
            game.hole_y = 400.0f;
            game.obstacles.push_back({400.0f, 250.0f, 200.0f, 50.0f});
        } else if (currentLevel == 3) {
            game.hole_x = 500.0f;
            game.hole_y = 300.0f;
            game.obstacles.push_back({300.0f, 300.0f, 100.0f, 200.0f});
        }
    }

    bool isLastLevel() {
        return currentLevel >= 3;
    }

    void nextLevel() {
        currentLevel++;
    }
};
#endif // LEVELMANAGER_H
