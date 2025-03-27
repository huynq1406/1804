#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include "defs.h"
#include "graphic.h"
#include "nhanvat.h"

using namespace std;
void checkWallCollision(Ball &ball) {
    if (ball.x - BALL_RADIUS <= 0 || ball.x + BALL_RADIUS >= SCREEN_WIDTH) {
        ball.vx = -ball.vx; // Đảo ngược vận tốc theo X
        ball.x = std::clamp(ball.x, BALL_RADIUS, SCREEN_WIDTH - BALL_RADIUS);
    }

    if (ball.y - BALL_RADIUS <= 0 || ball.y + BALL_RADIUS >= SCREEN_HEIGHT) {
        ball.vy = -ball.vy; // Đảo ngược vận tốc theo Y
        ball.y = std::clamp(ball.y, BALL_RADIUS, SCREEN_HEIGHT - BALL_RADIUS);
    }
}
bool checkObstacleCollision(Ball &ball, float obs_x, float obs_y, float obs_w, float obs_h) {
    float closestX = std::clamp(ball.x, obs_x, obs_x + obs_w);
    float closestY = std::clamp(ball.y, obs_y, obs_y + obs_h);

    float dx = ball.x - closestX;
    float dy = ball.y - closestY;
    float distanceSquared = dx * dx + dy * dy;

    return distanceSquared < (BALL_RADIUS * BALL_RADIUS);
}
bool checkHoleCollision(Ball &ball, float hole_x, float hole_y, float hole_radius) {
    float dx = ball.x - hole_x;
    float dy = ball.y - hole_y;
    float distanceSquared = dx * dx + dy * dy;
    return distanceSquared < (hole_radius * hole_radius);
}
void handleMouseInput(SDL_Event &event, Ball &ball, bool &isDragging, int &start_x, int &start_y) {
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

                // Tính toán lực dựa vào khoảng cách kéo chuột
                ball.vx = (start_x - end_x) * 0.1f;
                ball.vy = (start_y - end_y) * 0.1f;
            }
            break;
    }
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Golf Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Ball ball(100.0f, 300.0f);
    float hole_x = 700.0f, hole_y = 500.0f, hole_radius = 15.0f;
    float obs_x = 400.0f, obs_y = 250.0f, obs_w = 50.0f, obs_h = 100.0f;

    bool running = true;
    bool isDragging = false;
    int start_x = 0, start_y = 0;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            handleMouseInput(event, ball, isDragging, start_x, start_y);
        }

        // Cập nhật vị trí bóng
        ball.updatePosition(0.016f); // Giả sử deltaTime = 16ms

        // Kiểm tra va chạm
        checkWallCollision(ball);
        if (checkObstacleCollision(ball, obs_x, obs_y, obs_w, obs_h)) {
            ball.vx = -ball.vx;
            ball.vy = -ball.vy;
        }
        if (checkHoleCollision(ball, hole_x, hole_y, hole_radius)) {
            ball.vx = ball.vy = 0;
            std::cout << "Bóng vào lỗ! Hoàn thành màn chơi!" << std::endl;
            SDL_Delay(2000); // Chờ 2 giây trước khi thoát
            running = false;
        }

        // Vẽ màn hình
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderClear(renderer);

        // Vẽ bóng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect ballRect = { (int)(ball.x - BALL_RADIUS), (int)(ball.y - BALL_RADIUS), (int)(BALL_RADIUS * 2), (int)(BALL_RADIUS * 2) };
        SDL_RenderFillRect(renderer, &ballRect);

        // Vẽ lỗ golf
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect holeRect = { (int)(hole_x - hole_radius), (int)(hole_y - hole_radius), (int)(hole_radius * 2), (int)(hole_radius * 2) };
        SDL_RenderFillRect(renderer, &holeRect);

        // Vẽ chướng ngại vật
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect obstacleRect = { (int)obs_x, (int)obs_y, (int)obs_w, (int)obs_h };
        SDL_RenderFillRect(renderer, &obstacleRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Giả lập 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
