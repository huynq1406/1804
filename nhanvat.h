#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "defs.h"
#include "graphic.h"
#include "Ball.h"

using namespace std;

class Game {
public:
    Graphics graphics;
    Ball* ball = nullptr;
    float hole_x, hole_y, hole_radius;
    float obs_x, obs_y, obs_w, obs_h;
    bool running = true;
    bool isDragging = false;
    int start_x = 0, start_y = 0;

    Game() : hole_x(700.0f), hole_y(500.0f), hole_radius(15.0f), obs_x(400.0f), obs_y(250.0f), obs_w(50.0f), obs_h(100.0f) {}

    bool init() {
        graphics.init();
        ball = new Ball(100.0f, 300.0f, graphics.renderer, graphics.golfBallTexture);
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
            if (dx != 0) ball.vx = -ball.vx * 1.01;
            if (dy != 0) ball.vy = -ball.vy * 1.01;
            return true;
        }
        return false;
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

    void update(float deltaTime) {
        ball->updatePosition(deltaTime);
        checkWallCollision(*ball);
        if (checkObstacleCollision(*ball, obs_x, obs_y, obs_w, obs_h)) {
            ball->vx = -ball->vx;
            ball->vy = -ball->vy;
        }
        if (checkHoleCollision()) {
            ball->vx = ball->vy = 0;
            std::cout << "Bóng vào lỗ! Hoàn thành màn chơi!" << std::endl;
            SDL_Delay(2000);
            running = false;
        }
    }

    void render() {
        SDL_SetRenderDrawColor(graphics.renderer, 0, 128, 0, 255);
        SDL_RenderClear(graphics.renderer);

        SDL_Texture* bgTexture = graphics.loadTexture("New Piskel.png");
        if (!bgTexture) {
            std::cerr << "Không thể tải background: " << IMG_GetError() << std::endl;
        } else {
            SDL_RenderCopy(graphics.renderer, bgTexture, NULL, NULL);
            SDL_DestroyTexture(bgTexture);
        }

        if (ball->texture) {
            SDL_Rect ballRect = { (int)(ball->x - BALL_RADIUS), (int)(ball->y - BALL_RADIUS), (int)(BALL_RADIUS * 2), (int)(BALL_RADIUS * 2) };
            SDL_RenderCopy(graphics.renderer, ball->texture, NULL, &ballRect);
        } else {
            ball->drawCircle((int)BALL_RADIUS);
        }

        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
        SDL_Rect holeRect = { (int)(hole_x - hole_radius), (int)(hole_y - hole_radius), (int)(hole_radius * 2), (int)(hole_radius * 2) };
        SDL_RenderFillRect(graphics.renderer, &holeRect);

        SDL_SetRenderDrawColor(graphics.renderer, 255, 0, 0, 255);
        SDL_Rect obstacleRect = { (int)obs_x, (int)obs_y, (int)obs_w, (int)obs_h };
        SDL_RenderFillRect(graphics.renderer, &obstacleRect);

        SDL_RenderPresent(graphics.renderer);
    }

    void cleanUp() {
        delete ball;
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
                handleMouseInput(event);
            }
            update(0.08f);
            render();
            SDL_Delay(16);
        }
        cleanUp();
    }
};
