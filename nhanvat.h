#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm> // std::clamp

struct Ball {
    float x, y;   // Vị trí của bóng
    float vx, vy; // Vận tốc của bóng

    Ball(float startX, float startY) {
        x = startX;
        y = startY;
        vx = 0.0f;
        vy = 0.0f;
    }

    void updatePosition(float dt) {
        x += vx * dt;
        y += vy * dt;
    }
};
