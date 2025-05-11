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
#include "LevelManager.h"
using namespace std;

class Game {
public:
    vector<Obstacles> obstacles;
    Graphics graphics;
    Ball* ball = nullptr;
    IntroScreen* introScreen = nullptr;
    LevelManager levelManager;
    float hole_x, hole_y, hole_radius;
    float obs_x, obs_y, obs_w, obs_h;
    bool running = true;
    bool isDragging = false;
    bool exceededShots = false;
    int start_x = 0, start_y = 0;
    int shotCount = 0;
    int maxShots = 5; // Mặc định 5 cú đánh tối đa
    bool levelCompleted = false;
    bool gameOver = false;
    SDL_Rect continueButton = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 80};
    float dragDistance = 0.0f; // Khoảng cách kéo chuột
    const float MAX_DRAG_DISTANCE = 100.0f; // Giới hạn khoảng cách kéo tối đa
    const float POWER_SCALE = 0.5f; // Tỷ lệ lực dựa trên khoảng cách kéo

    Game() : hole_x(700.0f), hole_y(500.0f), hole_radius(15.0f), obs_x(400.0f), obs_y(250.0f), obs_w(50.0f), obs_h(100.0f) {
        graphics.init();
        introScreen = new IntroScreen(&graphics, "grass_field.png");
    }
    ~Game() {
        delete introScreen;
    }

    bool init() {
        ball = new Ball(100.0f, 300.0f, graphics.renderer, graphics.golfBallTexture);
        levelManager.loadLevel(*this);
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

    bool checkObstacleCollision(Ball &ball, Obstacles &obstacle) {
        if (obstacle.isRotating) {
            // Handle rotating obstacles
            float centerX = obstacle.x + obstacle.w / 2;
            float centerY = obstacle.y + obstacle.h / 2;
            float ballRelX = ball.x - centerX;
            float ballRelY = ball.y - centerY;

            // Transform ball position to obstacle's rotated coordinate system
            float rad = obstacle.rotationAngle * M_PI / 180.0f;
            float rotatedX = ballRelX * cos(rad) + ballRelY * sin(rad);
            float rotatedY = -ballRelX * sin(rad) + ballRelY * cos(rad);

            // Compute closest point in rotated coordinate system
            float closestX = std::clamp(rotatedX + centerX, obstacle.x, obstacle.x + obstacle.w);
            float closestY = std::clamp(rotatedY + centerY, obstacle.y, obstacle.y + obstacle.h);
            float dx = (rotatedX + centerX) - closestX;
            float dy = (rotatedY + centerY) - closestY;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < (BALL_RADIUS * BALL_RADIUS)) {
                float distance = std::sqrt(distanceSquared);
                float overlap = BALL_RADIUS - distance;
                float normalX = dx / distance;
                float normalY = dy / distance;

                // Transform normal back to world coordinates
                float tempX = normalX * cos(-rad) + normalY * sin(-rad);
                float tempY = -normalX * sin(-rad) + normalY * cos(-rad);
                normalX = tempX;
                normalY = tempY;

                // Resolve collision
                ball.x += normalX * overlap;
                ball.y += normalY * overlap;

                float dotProduct = ball.vx * normalX + ball.vy * normalY;
                ball.vx -= 2 * dotProduct * normalX;
                ball.vy -= 2 * dotProduct * normalY;

                ball.vx *= 0.9f;
                ball.vy *= 0.9f;

                if (obstacle.isSlowing) {
                    ball.vx *= 0.5f;
                    ball.vy *= 0.5f;
                }

                std::cout << "Collision with rotating obstacle at (" << obstacle.x << ", " << obstacle.y << ")" << std::endl;
                return true;
            }
        } else {
            // Handle non-rotating obstacles
            float closestX = std::clamp(ball.x, obstacle.x, obstacle.x + obstacle.w);
            float closestY = std::clamp(ball.y, obstacle.y, obstacle.y + obstacle.h);
            float dx = ball.x - closestX;
            float dy = ball.y - closestY;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < (BALL_RADIUS * BALL_RADIUS)) {
                float distance = std::sqrt(distanceSquared);
                float overlap = BALL_RADIUS - distance;
                float normalX = dx / distance;
                float normalY = dy / distance;

                // Resolve collision
                ball.x += normalX * overlap;
                ball.y += normalY * overlap;

                float dotProduct = ball.vx * normalX + ball.vy * normalY;
                ball.vx -= 2 * dotProduct * normalX;
                ball.vy -= 2 * dotProduct * normalY;

                ball.vx *= 0.9f;
                ball.vy *= 0.9f;

                if (obstacle.isSlowing) {
                    ball.vx *= 0.5f;
                    ball.vy *= 0.5f;
                }

                std::cout << "Collision with non-rotating obstacle at (" << obstacle.x << ", " << obstacle.y << ")" << std::endl;
                return true;
            }
        }
        return false;
    }

    void update(float deltaTime) {
        if (levelCompleted || gameOver) return;

        ball->updatePosition(deltaTime);
        checkWallCollision(*ball);

        for (auto &obstacle : obstacles) {
            if (obstacle.isRotating) {
                obstacle.rotationAngle += obstacle.rotationSpeed * deltaTime;
                obstacle.rotationAngle = fmod(obstacle.rotationAngle, 360.0f); // Giữ góc trong khoảng 0-360
            }
            checkObstacleCollision(*ball, obstacle);
        }

        if (checkHoleCollision()) {
            ball->vx = ball->vy = 0;
            levelCompleted = true;
            std::cout << "Ball potted!" << std::endl << "Level Complete!";
        }

        if (shotCount > maxShots && !levelCompleted) {
            gameOver = true;
        }
    }

    bool checkHoleCollision() {
        float dx = ball->x - hole_x;
        float dy = ball->y - hole_y;
        float distanceSquared = dx * dx + dy * dy;
        return distanceSquared < (hole_radius * hole_radius);
    }

    void drawPowerBar() {
        if (!isDragging) return;

        // Tính toán chiều dài thanh lực dựa trên dragDistance
        float powerRatio = std::min(dragDistance / MAX_DRAG_DISTANCE, 1.0f);
        int barWidth = static_cast<int>(200 * powerRatio); // Chiều dài tối đa của thanh là 100
        int barHeight = 10;
        int barX = static_cast<int>(ball->x + BALL_RADIUS + 10); // Đặt thanh lực bên phải bóng
        int barY = static_cast<int>(ball->y - barHeight / 2);

        // Vẽ nền của thanh lực (màu xám)
        SDL_SetRenderDrawColor(graphics.renderer, 128, 128, 128, 255);
        SDL_Rect backgroundBar = {barX, barY, 280, barHeight};
        SDL_RenderFillRect(graphics.renderer, &backgroundBar);

        // Vẽ thanh lực (màu xanh lá)
        SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255);
        SDL_Rect powerBar = {barX, barY, barWidth, barHeight};
        SDL_RenderFillRect(graphics.renderer, &powerBar);

        if (barWidth == 200){
        SDL_SetRenderDrawColor(graphics.renderer, 255, 0, 0, 255);
        SDL_Rect maxBar = {barX + 200, barY, 80, barHeight};
        SDL_RenderFillRect(graphics.renderer, &maxBar);
        }
    }

    void handleMouseInput(SDL_Event &event) {
        if (levelCompleted) {
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= continueButton.x && x <= continueButton.x + continueButton.w &&
                    y >= continueButton.y && y <= continueButton.y + continueButton.h) {
                    if (!levelManager.isLastLevel()) {
                        levelManager.nextLevel();
                        resetLevel();
                    } else {
                        running = false;
                    }
                }
            }
            return;
        }

        if (gameOver) {
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= continueButton.x && x <= continueButton.x + continueButton.w &&
                    y >= continueButton.y && y <= continueButton.y + continueButton.h) {
                    levelManager.reset(); // Reset về level 1
                    resetLevel();
                }
            }
            return;
        }
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isDragging = true;
                    start_x = event.button.x;
                    start_y = event.button.y;
                }
                break;
            case SDL_MOUSEMOTION:
                if (isDragging) {
                    int current_x, current_y;
                    SDL_GetMouseState(&current_x, &current_y);
                    dragDistance = std::sqrt(std::pow(current_x - start_x, 2) + std::pow(current_y - start_y, 2));
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT && isDragging) {
                    isDragging = false;
                    int end_x = event.button.x;
                    int end_y = event.button.y;
                    float power = std::min(dragDistance / MAX_DRAG_DISTANCE, 1.0f) * POWER_SCALE;
                    ball->vx = (start_x - end_x) * power; // Bắn ngược lại hướng kéo chuột
                    ball->vy = (start_y - end_y) * power;
                    shotCount++;
                    dragDistance = 0.0f;
                }
                break;
        }
    }

    void resetLevel() {
        delete ball;
        ball = new Ball(100.0f, 300.0f, graphics.renderer, graphics.golfBallTexture);
        shotCount = 0;
        levelCompleted = false;
        gameOver = false;
        exceededShots = false;
        levelManager.loadLevel(*this);
        // Cập nhật maxShots dựa trên level
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
            SDL_Rect ballRect = {(int)(ball->x - BALL_RADIUS), (int)(ball->y - BALL_RADIUS), (int)(BALL_RADIUS * 2), (int)(BALL_RADIUS * 2)};
            SDL_RenderCopy(graphics.renderer, ball->texture, NULL, &ballRect);
        } else {
            ball->drawCircle((int)BALL_RADIUS);
        }

        SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
        graphics.drawCircle((int)BALL_RADIUS, hole_x, hole_y);

        SDL_SetRenderDrawColor(graphics.renderer, 165, 42, 42, 255);
         for (const auto& obstacle : obstacles) {
            SDL_SetRenderDrawColor(graphics.renderer, obstacle.color.r, obstacle.color.g, obstacle.color.b, obstacle.color.a);
            if (obstacle.isRotating) {
                float centerX = obstacle.x + obstacle.w / 2;
                float centerY = obstacle.y + obstacle.h / 2;
                float rad = obstacle.rotationAngle * M_PI / 180.0f;
                SDL_Point points[5];
                float halfW = obstacle.w / 2;
                float halfH = obstacle.h / 2;
                points[0] = {static_cast<int>(centerX + (halfW * cos(rad) - halfH * sin(rad))),
                             static_cast<int>(centerY + (halfW * sin(rad) + halfH * cos(rad)))};
                points[1] = {static_cast<int>(centerX + (-halfW * cos(rad) - halfH * sin(rad))),
                             static_cast<int>(centerY + (-halfW * sin(rad) + halfH * cos(rad)))};
                points[2] = {static_cast<int>(centerX + (-halfW * cos(rad) + halfH * sin(rad))),
                             static_cast<int>(centerY + (-halfW * sin(rad) - halfH * cos(rad)))};
                points[3] = {static_cast<int>(centerX + (halfW * cos(rad) + halfH * sin(rad))),
                             static_cast<int>(centerY + (halfW * sin(rad) - halfH * cos(rad)))};
                points[4] = points[0];
                SDL_RenderDrawLines(graphics.renderer, points, 5);
            } else {
                SDL_Rect obstacleRect = {(int)obstacle.x, (int)obstacle.y, (int)obstacle.w, (int)obstacle.h};
                SDL_RenderFillRect(graphics.renderer, &obstacleRect);
            }
        }
        drawPowerBar();

        // Hiển thị số lần đánh và giới hạn
        string shotText = "Shots: " + to_string(shotCount) + " Shots limit: " + to_string(maxShots);
        graphics.renderText(shotText.c_str(), "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 24, {255, 255, 255, 255}, 10, 10);

        if (levelCompleted) {
            SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(graphics.renderer, &overlay);

            if (levelManager.isLastLevel()) {
            graphics.renderText("Congratulations!", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 60, {255, 255, 255, 255}, SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 50);
            SDL_SetRenderDrawColor(graphics.renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(graphics.renderer, &continueButton);
            graphics.renderText("GAME OVER!", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 36, {0, 0, 0, 255}, continueButton.x + 22, continueButton.y + 18);
        } else {
            graphics.renderText("Level Completed", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 60, {255, 255, 255, 255}, SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 50);
            SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(graphics.renderer, &continueButton);
            graphics.renderText("Continue", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 36, {0, 0, 0, 255}, continueButton.x + 22, continueButton.y + 18);
        }
        }

        if (gameOver) {
            SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(graphics.renderer, &overlay);

            graphics.renderText("Game Over! Exceeded Shot Limit", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 48, {255, 0, 0, 255}, SCREEN_WIDTH/2 - 350, SCREEN_HEIGHT/2 - 50);

            SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(graphics.renderer, &continueButton);
            graphics.renderText("Try again", "C:\\Users\\Dell\\Documents\\lap trinh nang cao\\1803\\ARCADECLASSIC.TTF", 36, {0, 0, 0, 255}, continueButton.x + 22, continueButton.y + 18);
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
            } else {
                introScreen->update(0.08f);
            }
            render();
            SDL_Delay(16);
        }

        cleanUp();
    }
};
