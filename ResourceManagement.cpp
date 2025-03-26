#include "Tetris.h"

SDL_Texture* Tetris::loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (!loadedSurface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    if (!texture) {
        printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    }
    return texture;
}

void Tetris::loadMenuBackground(const char* filePath) {
    SDL_Surface* loadedSurface = IMG_Load(filePath); // Tải hình ảnh từ file
    if (!loadedSurface) {
        printf("Failed to load menu background: %s\n", IMG_GetError());
        return;
    }
    menuBackgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface); // Tạo texture từ surface
    if (!menuBackgroundTexture) {
        printf("Failed to create menu background texture: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface); // Giải phóng surface
}

void Tetris::loadBackground(const char* filePath) {
    SDL_Surface* loadedSurface = IMG_Load(filePath);
    if (!loadedSurface) {
        printf("Failed to load background: %s\n", IMG_GetError());
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
}

void Tetris::loadSounds() {
    backgroundMusic = Mix_LoadMUS("background_music.mp3");
    moveSound = Mix_LoadWAV("move_sound.wav");
    clearSound = Mix_LoadWAV("clear_line.wav");
    explosionSound = Mix_LoadWAV("explosion.wav");
    iceSound = Mix_LoadWAV("ice_sound.wav");
    if (!backgroundMusic || !moveSound || !clearSound || !explosionSound || !iceSound) {
        printf("Failed to load sounds: %s\n", Mix_GetError());
    }
}

void Tetris::loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &highScore); // Đọc điểm cao nhất từ file
        fclose(file);
    } else {
        highScore = 0; // Nếu file không tồn tại, đặt điểm cao nhất là 0
    }
}

void Tetris::saveHighScore() {
    FILE* file = fopen("highscore.txt", "w");
    if (file) {
        fprintf(file, "%d", highScore); // Lưu điểm cao nhất vào file
        fclose(file);
    }
}
