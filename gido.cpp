#include "Untitled2.h"
// Lê Ngọc Quang
void Graphic::quitSDL(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
// Hoàng Sỹ Hùng
void Graphic::presentScene(){
    SDL_RenderPresent(renderer);
}
//Chu Đức Ánh
void Graphic::prepareScene(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

//Nguyễn Hải Đăng
void Graphic::initSDL (int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        logErrorAndExit("SDL_INIT", SDL_GetError());
    }

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)){
        logErrorAndExit("IMG_INIT", SDL_GetError());
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL){
        logErrorAndExit("SDL_WINDOW", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        logErrorAndExit("SDL_RENDERER", SDL_GetError());
    }
}
//Nguyễn Lê Dũng
void Graphic::logErrorAndExit(const char* msg, const char* error){
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s :%s", msg, error);
    SDL_Quit();
}
//Bui Thi Ha Thu
void Graphic::renderTexture(SDL_Texture *texture, int x , int y , int w , int h ) {
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_QueryTexture (texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy (renderer, texture, NULL, &dest);
}
//Duong Tuan Anh
SDL_Texture* Graphic::loadTexture(const char* filename) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filename);
    if(texture==nullptr) {
        logErrorAndExit("Fail to load texture.", IMG_GetError());
    }
    return texture;
}
