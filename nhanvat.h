#include <SDL.h>
 #include <iostream>
 #include <cmath>
 #include <algorithm>
 #include <vector>
 #include "defs.h"
 #include "graphic.h"
 #include "Ball.h"
 #include "obstacles.h"
 using namespace std;

 class Game {
 public:
     vector <Obstacles> obstacles;
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
     obstacles.push_back({600.0f, 400.0f, 50.0f, 200.0f});
     obstacles.push_back({600.0f, 350.0f, 200.0f, 50.0f});
     obstacles.push_back({600.0f, 450.0f, 50.0f, 150.0f}); // Chướng ngại vật mới
     obstacles.push_back({200.0f, 550.0f, 400.0f, 50.0f}); // Chướng ngại vật mới
     obstacles.push_back({0.0f, 700.0f, 800.0f, 100.0f}); // Chướng ngại vật mới
     obstacles.push_back({200.0f, 200.0f, 50.0f, 350.0f}); // Chướng ngại vật mới
     obstacles.push_back({0.0f, 200.0f, 200.0f, 50.0f}); // Chướng ngại vật mới
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
     // Tìm điểm gần nhất từ tâm bóng tới vùng chướng ngại vật
     float closestX = std::clamp(ball.x, obs_x, obs_x + obs_w);
     float closestY = std::clamp(ball.y, obs_y, obs_y + obs_h);

     // Tính khoảng cách từ tâm bóng tới điểm gần nhất
     float dx = ball.x - closestX;
     float dy = ball.y - closestY;

     // Tính bình phương khoảng cách
     float distanceSquared = dx * dx + dy * dy;

     // Kiểm tra nếu khoảng cách nhỏ hơn bán kính bóng (va chạm xảy ra)
     if (distanceSquared < (BALL_RADIUS * BALL_RADIUS)) {
         // Đẩy bóng ra ngoài để tránh chồng lấn
         float distance = std::sqrt(distanceSquared);
         float overlap = BALL_RADIUS - distance;
         float normalX = dx / distance; // Vector pháp tuyến tại điểm va chạm (chuẩn hóa)
         float normalY = dy / distance;

         ball.x += normalX * overlap; // Đẩy bóng ra khỏi chướng ngại vật
         ball.y += normalY * overlap;

         // Tính vận tốc phản xạ
         float dotProduct = ball.vx * normalX + ball.vy * normalY; // Tích vô hướng giữa vận tốc và pháp tuyến
         ball.vx -= 2 * dotProduct * normalX; // Thành phần phản xạ theo trục x
         ball.vy -= 2 * dotProduct * normalY; // Thành phần phản xạ theo trục y

         // Giảm vận tốc do ma sát
         ball.vx *= 0.9f; // Giảm 10% vận tốc trên trục x
         ball.vy *= 0.9f; // Giảm 10% vận tốc trên trục y

         return true; // Có va chạm
     }

     return false; // Không có va chạm
 }
 void update(float deltaTime) {
     ball->updatePosition(deltaTime);
     checkWallCollision(*ball);

     // Kiểm tra va chạm với tất cả chướng ngại vật
     for (auto &obstacle : obstacles) {
         checkObstacleCollision(*ball, obstacle.x, obstacle.y, obstacle.w, obstacle.h);
     }

     if (checkHoleCollision()) {
         ball->vx = ball->vy = 0;
         std::cout << "Bóng vào lỗ! Hoàn thành màn chơi!" << std::endl;
         SDL_Delay(2000);
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
     SDL_SetRenderDrawColor(graphics.renderer, 0, 128, 0,255);
     SDL_RenderClear(graphics.renderer);

     // Sử dụng backgroundTexture đã được tải sẵn trong graphics.init()
     if (graphics.backgroundTexture) {
         SDL_RenderCopy(graphics.renderer, graphics.backgroundTexture, NULL, NULL);
     }

     if (ball->texture) {
         SDL_Rect ballRect = { (int)(ball->x - BALL_RADIUS), (int)(ball->y - BALL_RADIUS), (int)(BALL_RADIUS * 2), (int)(BALL_RADIUS * 2) };
         SDL_RenderCopy(graphics.renderer, ball->texture, NULL, &ballRect);
     } else {
         ball->drawCircle((int)BALL_RADIUS);
     }
     SDL_SetRenderDrawColor(graphics.renderer, 0, 0, 0, 255); // Màu đen cho lỗ gôn
     graphics.drawCircle((int)BALL_RADIUS, hole_x, hole_y);
     SDL_SetRenderDrawColor(graphics.renderer, 165, 42, 42, 255); // Màu nâu
     for (const auto& obstacle : obstacles) {
     SDL_Rect obstacleRect = { (int)obstacle.x, (int)obstacle.y, (int)obstacle.w, (int)obstacle.h };
     SDL_RenderFillRect(graphics.renderer, &obstacleRect);
 }

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
