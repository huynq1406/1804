#ifndef BALL_H
#define BALL_H

#include "graphic.h"
using namespace std;
class Ball {
public:
    float x, y;
    float vx, vy;
    float amping;
    SDL_Renderer* renderer;
    SDL_Texture* texture; // Lưu golfBallTexture

    Ball(float startX, float startY, SDL_Renderer* rend, SDL_Texture* tex)
        : x(startX), y(startY), vx(0), vy(0), amping(0.98f), renderer(rend), texture(nullptr) {
        if (!texture) {
            std::cerr << "Không thể tải file ảnh: " << IMG_GetError() << endl;
            return;
        }
    }

    ~Ball() {
        if (texture) SDL_DestroyTexture(texture);
    }

    void updatePosition(float deltaTime) {
        x += vx * deltaTime;
        y += vy * deltaTime;
        vx *= amping;
        vy *= amping;
        if (abs(vx) < 0.01f) vx = 0;
        if (abs(vy) < 0.01f) vy = 0;
    }

    void drawCircle(int radius) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }
};

#endif
