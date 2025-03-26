#include "Bomb.h"
#include "Tetris.h"

Bomb::Bomb(int x, int y, SDL_Texture* texture, BombType type)
    : x(x), y(y), active(true), exploding(false), explodeStartTime(0),
      type(type), texture(texture) {}

void Bomb::update() {
    if (active) {
        y += 1; // Áp dụng tốc độ
    }
    if (exploding && SDL_GetTicks() - explodeStartTime > 300) {
        exploding = false;
        active = false;
    }
}
void Bomb::render(SDL_Renderer* renderer) const {
    if (active || exploding) {
        SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        // Đổi màu theo loại bom khi nổ
        if (exploding) {
            if (type == ICE_BOMB) {
                SDL_SetTextureColorMod(texture, 150, 200, 255); // Màu xanh băng
            }
        }
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_SetTextureColorMod(texture, 255, 255, 255); // Reset màu
    }
}

void Bomb::explode() {
    exploding = true;
    explodeStartTime = SDL_GetTicks();
    active = false;
}

bool Bomb::isActive() const { return active; }
int Bomb::getX() const { return x; }
int Bomb::getY() const { return y; }
BombType Bomb::getType() const { return type; }
void Bomb::deactivate() { active = false; }
