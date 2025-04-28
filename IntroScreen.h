#ifndef INTROSCREEN_H
#define INTROSCREEN_H

#include <SDL.h>
#include "graphic.h"
class IntroScreen {
public:
   bool startGame = false;

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Màu nền xanh dương
        SDL_RenderClear(renderer);

        // Hiển thị logo hoặc text (hoặc dùng SDL_ttf để hiển thị chữ)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
        SDL_Rect logoRect = {300, 200, 200, 100};
        SDL_RenderFillRect(renderer, &logoRect);

        SDL_RenderPresent(renderer);
    }

    void handleEvent(SDL_Event &e) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
            startGame = true; // Bắt đầu game khi nhấn Enter
        }
    }
};
#endif
