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
    float bgOffsetX = 0.0f; // Offset for background movement

    IntroScreen(Graphics* g, const char* imagePath = "grass_field.png") : graphics(g), introTexture(nullptr) {
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
    void update(float deltaTime) {
        // Di chuyển nền sang trái với tốc độ 60 pixel mỗi giây
        bgOffsetX -= 40.0f * deltaTime;
        // Lặp lại khi offset vượt quá chiều rộng của texture
        int texW;
        SDL_QueryTexture(introTexture, NULL, NULL, &texW, NULL);
        if (bgOffsetX <= -texW) {
            bgOffsetX += texW; // Lặp lại từ đầu
        }
    }

    void render(SDL_Renderer* renderer) {
        // Clear the screen with a black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render intro image as background
        if (introTexture) {
            int texW, texH;
            if (SDL_QueryTexture(introTexture, NULL, NULL, &texW, &texH) == 0) {
                // Render nhiều bản sao để tạo hiệu ứng lặp
                for (int i = -1; i <= 1; i++) {
                    int x = static_cast<int>(bgOffsetX + i * texW);
                    graphics->renderTexture(introTexture, x, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                }
            } else {
                SDL_Log("Failed to query introTexture dimensions: %s", SDL_GetError());
            }
        } else {
            SDL_Log("introTexture is nullptr");
        }

        SDL_Color white = {255, 255, 255, 255};
        graphics->renderText("Golf Game", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 150, white, SCREEN_WIDTH / 2 - 325, 200);
        // Render "Start Game" button
        SDL_SetRenderDrawColor(renderer, 0, 255, 150, 255); // Green button
        SDL_Rect startButton = {200, 350, 400, 100};
        SDL_RenderFillRect(renderer, &startButton);
        graphics->renderText("Start Game", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 56, white, 250, 370);

        // Render "Quit Game" button
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red button
        SDL_Rect quitButton = {300, 500, 200, 50};
        SDL_RenderFillRect(renderer, &quitButton);
        graphics->renderText("Quit Game", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 30, white, 320, 510);

        // Present the final rendering
        SDL_RenderPresent(renderer);
    }

    void handleEvent(SDL_Event &e) {
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;

            // Check if "Start Game" button is clicked
            if (x >= 200 && x <= 600 && y >= 350 && y <= 450) {
                startGame = true;
            }
            // Check if "Quit Game" button is clicked
            else if (x >= 300 && x <= 500 && y >= 500 && y <= 550) {
                quitGame = true;
            }
        }
    }
};

#endif
