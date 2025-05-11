#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "graphic.h"

struct Obstacles {
    float x, y, w, h;         // Tọa độ và kích thước của chướng ngại vật
    bool isRotating;          // Xác định chướng ngại vật có xoay hay không
    bool isSlowing;           // Xác định chướng ngại vật có làm chậm hay không
    float rotationAngle;      // Góc xoay hiện tại (độ)
    float rotationSpeed;      // Tốc độ xoay (độ/giây)
    SDL_Color color;          // Màu sắc của chướng ngại vật

    Obstacles(float x, float y, float w, float h, bool isRotating = false, bool isSlowing = false,
              float rotationSpeed = 0.0f, SDL_Color color = {165, 42, 42, 255})
        : x(x), y(y), w(w), h(h), isRotating(isRotating), isSlowing(isSlowing),
          rotationAngle(0.0f), rotationSpeed(rotationSpeed), color(color) {}
};

#endif
