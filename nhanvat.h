#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include "defs.h"
#include "graphic.h"
#include "Ball.h"
#include "obstacles.h"
#include "IntroScreen.h"
using namespace std;

class Game {
public:
    vector <Obstacles> obstacles;
    Graphics graphics;
    Ball* ball = nullptr;
    IntroScreen* introScreen = nullptr;
    float hole_x, hole_y, hole_radius;
    float obs_x, obs_y, obs_w, obs_h;
    bool running = true;
    bool isDragging = false;
    int start_x = 0, start_y = 0;

    Game() : hole_x(700.0f), hole_y(500.0f), hole_radius(15.0f), obs_x(400.0f), obs_y(250.0f), obs_w(50.0f), obs_h(100.0f) {
        graphics.init(); // Initialize graphics first
        introScreen = new IntroScreen(&graphics, "intro.png"); // Initialize introScreen after graphics
    }
    ~Game() {
        delete introScreen; // Clean up introScreen
    }

    bool init() {
        graphics.init();
        ball = new Ball(100.0f, 300.0f, graphics.renderer, graphics.golfBallTexture);
        obstacles.push_back({600.0f, 400.0f, 50.0f, 200.0f});
        obstacles.push_back({600.0f, 350.0f, 200.0f, 50.0f});
        obstacles.push_back({600.0f, 450.0f, 50.0f, 150.0f});
        obstacles.push_back({200.0f, 550.0f, 400.0f, 50.0f});
        obstacles.push_back({0.0f, 750.0f, 800.0f, 50.0f});
        obstacles.push_back({200.0f, 200.0f, 50.0f, 350.0f});
        obstacles.push_back({0.0f, 200.0f, 200.0f, 50.0f});
        return true;
    }

    void checkWallCollision(Ball &ball) {
        if (ball.x - BALL_RADIUS <= 0 || ball.x + BALL_RADIUS >= SCREEN_WIDTH) {
            ball.vx = -ball.vx;
            ball.x = std::clamp(ball.x, BALL_RADIUS, SCREEN_WIDTH - BALL_RADIUS);
        }
        if (ball.y - BALL_RADIUS <= 0 || ball.y + BALL_RADIUS >= SCREEN_HEIGHT) {
            ball.vy = -ball.vy;
            ball.y = std::clamp(ball.y, BALL_RADIUS, SCREEN_HEIGHT - BALL_RADIUS);
        }
    }

    bool checkObstacleCollision(Ball &ball, float obs_x, float obs_y, float obs_w, float obs_h) {
        float closestX = std::clamp(ball.x, obs_x, obs_x + obs_w);
        float closestY = std::clamp(ball.y, obs_y, obs_y + obs_h);
        float dx = ball.x - closestX;
        float dy = ball.y - closestY;
        float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < (BALL_RADIUS * BALL_RADIUS)) {
            float distance = std::sqrt(distanceSquared);
            float overlap = BALL_RADIUS - distance;
            float normalX = dx / distance;
            float normalY = dy / distance;

            ball.x += normalX * overlap;
            ball.y += normalY * overlap;

            float dotProduct = ball.vx * normalX + ball.vy * normalY;
            ball.vx -= 2 * dotProduct * normalX;
            ball.vy -= 2 * dotProduct * normalY;

            ball.vx *= 0.9f;
            ball.vy *= 0.9f;

            return true;
        }
        return false;
    }

    void update(float deltaTime) {
        ball->updatePosition(deltaTime);
        checkWallCollision(*ball);

        for (auto &obstacle : obstacles) {
            checkObstacleCollision(*ball, obstacle.x, obstacle.y, obstacle.w, obstacle.h);
        }

        if (checkHoleCollision()) {
            ball->vx = ball->vy = 0;
            std::cout << "Ball potted!" << std::endl << "Level Complete!";
            SDL_Delay(1000);
            running = false;
        }
    }

    bool checkHoleCollision() {
        float dx = ball->x - hole_x;
        float dy = ball->y - hole_y;
        float distanceSquared = dx * dx + dy * dy;
        return distanceSquared < (hole_radius * hole_radius);
    }

    void handleMouseInput(SDL_Event &event) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isDragging = true;
                    start_x = event.button.x;
                    start_y = event.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT && isDragging) {
                    isDragging = false;
                    int end_x = event.button.x;
                    int end_y = event.button.y;
                    ball->vx = (start_x - end_x) * 0.5f;
                    ball->vy = (start_y - end_y) * 0.5f;
                }
                break;
        }
    }

    void render() {
        if (!introScreen->startGame) {
            introScreen->render(graphics.renderer);
            return;
        }

        SDL_SetRenderDrawColor(graphics.renderer, 0, 128, 0, 255);
        SDL_RenderClear(graphics.renderer);

        if (graphics.backgroundTexture) {
            SDL_RenderCopy(graphics.renderer, graphics.backgroundTexture, NULL, NULL);
        }

        if (ball->texture) {
            SDL_Rect ballRect = { (int)(ball->x - BALL_RADIUS), (int)(ball->y - BALL_RADIUS), (int)(BALL_RADIUS * 2), (int)(BALL_RADIUS * 2) };
            SDL_RenderCopy(graphics.renderer, ball->texture, NULL, &ballRect);
        } else {
            ball->drawCircle((int)BALL_RADIUS);
        }

        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
        graphics.drawCircle((int)BALL_RADIUS, hole_x, hole_y);

        SDL_SetRenderDrawColor(graphics.renderer, 165, 42, 42, 255);
        for (const auto& obstacle : obstacles) {
            SDL_Rect obstacleRect = { (int)obstacle.x, (int)obstacle.y, (int)obstacle.w, (int)obstacle.h };
            SDL_RenderFillRect(graphics.renderer, &obstacleRect);
        }

        SDL_RenderPresent(graphics.renderer);
    }

    void cleanUp() {
        delete ball;
        delete introScreen;
        graphics.quit();
    }

    void run() {
        if (!init()) {
            return;
        }

        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (!introScreen->startGame) {
                    introScreen->handleEvent(event);
                    if (introScreen->quitGame) {
                        running = false;
                    }
                } else {
                    handleMouseInput(event);
                }
            }

            if (introScreen->startGame) {
                update(0.08f);
            }
            render();
            SDL_Delay(16);
        }

        cleanUp();
    }
};
