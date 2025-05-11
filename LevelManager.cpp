#include <iostream>
#include "LevelManager.h"
#include "nhanvat.h"

void LevelManager::loadLevel(Game &game) {
    game.obstacles.clear();
    game.shotCount = 0; // Reset số lần đánh

    switch (currentLevel) {
        case 1:
            game.hole_x = 700.0f;
            game.hole_y = 500.0f;
            game.maxShots = 5;
            game.obstacles.push_back({600.0f, 400.0f, 50.0f, 200.0f});
            game.obstacles.push_back({600.0f, 350.0f, 200.0f, 50.0f});
            game.obstacles.push_back({600.0f, 450.0f, 50.0f, 150.0f});
            game.obstacles.push_back({200.0f, 550.0f, 400.0f, 50.0f});
            game.obstacles.push_back({0.0f, 750.0f, 800.0f, 50.0f});
            game.obstacles.push_back({200.0f, 200.0f, 50.0f, 350.0f});
            game.obstacles.push_back({0.0f, 200.0f, 200.0f, 50.0f});
            game.obstacles.push_back({400.0f, 600.0f, 50.0f, 100.0f});
            game.obstacles.push_back({250.0f, 650.0f, 50.0f, 100.0f});

            game.obstacles.push_back({600.0f, 300.0f, 100.0f, 200.0f, true, false, 30.0f, {165, 42, 42, 255}}); // Xoay

            break;
        case 2:
            game.hole_x = 700.0f;
            game.hole_y = 500.0f;
            game.maxShots = 6;
            game.obstacles.push_back({600.0f, 400.0f, 50.0f, 200.0f});
            game.obstacles.push_back({600.0f, 350.0f, 200.0f, 50.0f});
            game.obstacles.push_back({600.0f, 450.0f, 50.0f, 150.0f});
            game.obstacles.push_back({200.0f, 550.0f, 400.0f, 50.0f});
            game.obstacles.push_back({0.0f, 750.0f, 800.0f, 50.0f});
            game.obstacles.push_back({200.0f, 200.0f, 50.0f, 350.0f});
            game.obstacles.push_back({0.0f, 200.0f, 200.0f, 50.0f});
            game.obstacles.push_back({600.0f, 600.0f, 50.0f, 100.0f});
            game.obstacles.push_back({100.0f, 500.0f, 100.0f, 50.0f});
            game.obstacles.push_back({0.0f, 400.0f, 100.0f, 50.0f});
            game.obstacles.push_back({350.0f, 650.0f, 50.0f, 100.0f});
            break;
        case 3:
            game.hole_x = 500.0f;
            game.hole_y = 300.0f;
            game.maxShots = 7;
            game.obstacles.push_back({0.0f, 150.0f, 200.0f, 100.0f});
            game.obstacles.push_back({150.0f, 150.0f, 50.0f, 350.0f});
            game.obstacles.push_back({200.0f, 550.0f, 150.0f, 50.0f});
            game.obstacles.push_back({600.0f, 300.0f, 100.0f, 200.0f, true, false, 0.0f, {165, 42, 42, 255}}); // Xoay
            game.obstacles.push_back({300.0f, 300.0f, 100.0f, 200.0f, true, false, 60.0f, {165, 42, 42, 255}}); // Xoay
            game.obstacles.push_back({400.0f, 150.0f, 200.0f, 50.0f, false, true, 0.0f, {0, 0, 255, 255}}); // Làm chậm
            break;

        default:
            game.running = false;
            break;
    }
}

bool LevelManager::isLastLevel() {
    return currentLevel >= 3;
}

void LevelManager::nextLevel() {
    currentLevel++;
}

void LevelManager::reset() {
    currentLevel = 1;
}
