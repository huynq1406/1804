#ifndef BALL_H
#define BALL_H

#include "graphic.h"
class Ball {
public:
    float x, y;
    float vx, vy;
    float amping;
    SDL_Renderer* renderer;
    SDL_Texture* texture; // Lưu golfBallTexture

   Ball(Graphics &graphics, const char *imagePath, int startX, int startY)
        : x(startX), y(startY) {
        texture = graphics.loadTexture(imagePath); // Tải texture từ hình ảnh
        if (texture == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Failed to load ball texture");
        }
    }

    ~Ball() {
        // Không hủy texture vì nó thuộc về Graphics
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
