#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm> // std::clamp

class Ball {
public:
    float x, y;
    float vx, vy;
    float amping;
    SDL_Renderer* renderer; // Thêm renderer làm biến thành viên
    SDL_Texture* texture;
    Ball(float startX, float startY, SDL_Renderer* rend, const char* filePath) : x(startX), y(startY), vx(0), vy(0), amping(0.99f),renderer(rend), texture(nullptr) {}
    void updatePosition(float deltaTime) {
        x += vx * deltaTime;
        y += vy * deltaTime;
        vx *= amping; // Giảm vận tốc theo hệ số giảm chấn
        vy *= amping; // Giảm vận tốc theo hệ số giảm chấn

        // Dừng bóng khi vận tốc rất nhỏ
        if (std::abs(vx) < 0.01f) vx = 0;
        if (std::abs(vy) < 0.01f) vy = 0;
    }
    Ball(SDL_Renderer* rend, const char* filePath) {
        renderer = rend;
        SDL_Surface* surface = IMG_Load("3d-animated-golf-ball-videohive-download-7751583-free-hunterae-com-5-removebg-preview.png"); // Tải tệp PNG
        if (!surface) {
            printf("Không thể tải hình ảnh: %s\n", IMG_GetError());
            return;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            printf("Không thể tạo texture: %s\n", SDL_GetError());
        }
    }

    Ball() {
        if (texture){
        SDL_DestroyTexture(texture);
        }
    }
};

class Game {
public:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Ball ball;
    float hole_x, hole_y, hole_radius;
    float obs_x, obs_y, obs_w, obs_h;
    bool running = true;
    bool isDragging = false;
    int start_x = 0, start_y = 0;

    Game() : ball(100.0f, 300.0f, nullptr, "3d-animated-golf-ball-videohive-download-7751583-free-hunterae-com-5-removebg-preview.png"), hole_x(700.0f), hole_y(500.0f), hole_radius(15.0f), obs_x(400.0f), obs_y(250.0f), obs_w(50.0f), obs_h(100.0f) {}

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }
        if (!IMG_Init(IMG_INIT_PNG)) { // Khởi tạo SDL_image với hỗ trợ PNG
            std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
            return false;
        }
        window = SDL_CreateWindow("Golf Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        return true;
    }
    void checkWallCollision(Ball &ball) {
    // Kiểm tra va chạm với tường trái và phải
    if (ball.x - BALL_RADIUS <= 0 || ball.x + BALL_RADIUS >= SCREEN_WIDTH) {
        ball.vx = -ball.vx; // Đảo ngược vận tốc theo X
        ball.x = std::clamp(ball.x, BALL_RADIUS, SCREEN_WIDTH - BALL_RADIUS);
    }

    // Kiểm tra va chạm với tường trên và dưới
    if (ball.y - BALL_RADIUS <= 0 || ball.y + BALL_RADIUS >= SCREEN_HEIGHT) {
        ball.vy = -ball.vy; // Đảo ngược vận tốc theo Y
        ball.y = std::clamp(ball.y, BALL_RADIUS, SCREEN_HEIGHT - BALL_RADIUS);
    }
}

    bool checkObstacleCollision(Ball &ball, float obs_x, float obs_y, float obs_w, float obs_h) {
    // Tìm tọa độ gần nhất trên chướng ngại vật
    float closestX = std::clamp(ball.x, obs_x, obs_x + obs_w);
    float closestY = std::clamp(ball.y, obs_y, obs_y + obs_h);

    // Tính khoảng cách giữa quả bóng và điểm gần nhất
    float dx = ball.x - closestX;
    float dy = ball.y - closestY;
    float distanceSquared = dx * dx + dy * dy;

    // Kiểm tra xem khoảng cách có nhỏ hơn bán kính của quả bóng hay không
    if (distanceSquared < (BALL_RADIUS * BALL_RADIUS)) {
        // Đảo ngược vận tốc khi va chạm xảy ra
        if (dx != 0) ball.vx = -ball.vx*1.01;
        if (dy != 0) ball.vy = -ball.vy*1.01;
        return true;
    }
    return false;
}

    bool checkHoleCollision() {
        float dx = ball.x - hole_x;
        float dy = ball.y - hole_y;
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

                    // Tính toán lực dựa vào khoảng cách kéo chuột
                    ball.vx = (start_x - end_x) * 0.5f;
                    ball.vy = (start_y - end_y) * 0.5f;
                }
                break;
        }
    }

    void update(float deltaTime) {
        ball.updatePosition(deltaTime); // Giả sử deltaTime = 16ms

        // Kiểm tra va chạm
        checkWallCollision(ball);
        if (checkObstacleCollision(ball, obs_x, obs_y, obs_w, obs_h)) {
            ball.vx = -ball.vx;
            ball.vy = -ball.vy;
        }
        if (checkHoleCollision()) {
            ball.vx = ball.vy = 0;
            std::cout << "Bóng vào lỗ! Hoàn thành màn chơi!" << std::endl;
            SDL_Delay(2000); // Chờ 2 giây trước khi thoát
            running = false;
        }
    }

    void render() {
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
    }

    void cleanUp() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
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

            update(0.08f); // Giả sử deltaTime = 16ms
            render();
            SDL_Delay(16); // Giả lập 60 FPS
        }

        cleanUp();
    }
};
