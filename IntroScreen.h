#ifndef INTROSCREEN_H
#define INTROSCREEN_H

#include <SDL.h>
#include "graphic.h"

class IntroScreen {
public:
    bool startGame = false;
    bool quitGame = false;
    Graphics* graphics; // Pointer to Graphics for rendering
    SDL_Texture* introTexture; // Texture for intro image

    IntroScreen(Graphics* g, const char* imagePath = "intro.png") : graphics(g), introTexture(nullptr) {
        // Load intro image
        introTexture = graphics->loadTexture(imagePath);
        if (introTexture == nullptr) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                   "Failed to load intro image '%s': %s", imagePath, IMG_GetError());
    } else {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   "Successfully loaded intro image '%s'", imagePath);
}
    }

    ~IntroScreen() {
        if (introTexture) {
            SDL_DestroyTexture(introTexture);
            introTexture = nullptr;
        }
    }

    void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Blue background
    SDL_RenderClear(renderer);

    // Render intro image
    if (introTexture) {
        int texW, texH;
        if (SDL_QueryTexture(introTexture, NULL, NULL, &texW, &texH) == 0) {
            int x = (SCREEN_WIDTH - texW) / 2;
            graphics->renderTexture(introTexture, x, 100, texW, texH);
        }
    }

    // Render "Start Game" button
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green button
    SDL_Rect startButton = {300, 300, 200, 50};
    SDL_RenderFillRect(renderer, &startButton);
    SDL_Color white = {255, 255, 255, 255};
    graphics->renderText("Start Game", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 24, white, 320, 310);

    // Render "Quit Game" button
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red button
    SDL_Rect quitButton = {300, 400, 200, 50};
    SDL_RenderFillRect(renderer, &quitButton);
    graphics->renderText("Quit Game", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 24, white, 320, 410);

    SDL_RenderPresent(renderer);
}

    void handleEvent(SDL_Event &e) {
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;

            // Check if "Start Game" button is clicked
            if (x >= 300 && x <= 500 && y >= 300 && y <= 350) {
                startGame = true;
            }
            // Check if "Quit Game" button is clicked
            else if (x >= 300 && x <= 500 && y >= 400 && y <= 450) {
                quitGame = true;
            }
        }
    }
};

#endif
