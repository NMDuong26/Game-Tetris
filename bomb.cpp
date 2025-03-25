// Bomb.cpp
#include "Bomb.h"
#include <SDL_mixer.h>

Bomb::Bomb(int x, int y, SDL_Texture* texture)
    : x(x), y(y), active(true), texture(texture) {
         explosionSound = Mix_LoadWAV("explosion.wav");
    }

void Bomb::update() {
    if (active) {
        y += 1;
    }
    if (exploding && SDL_GetTicks() - explodeStartTime > 300) {
        exploding = false;
        active = false; // Tự hủy sau khi nổ xong
    }
}

void Bomb::render(SDL_Renderer* renderer) const {
    // Chỉ render khi bom đang active HOẶC đang trong trạng thái nổ
    if (active || exploding) {
        SDL_Rect rect = {x * 60, y * 60, 60, 60};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

void Bomb::explode() {
        exploding = true;
        active = false;  // Thêm dòng này để ngừng render bom ngay lập tức
        explodeStartTime = SDL_GetTicks();
    }

void Bomb::deactivate() {
    active = false;
}

bool Bomb::isActive() const { return active; }
int Bomb::getX() const { return x; }
int Bomb::getY() const { return y; }
