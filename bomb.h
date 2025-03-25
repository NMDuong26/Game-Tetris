#ifndef BOMB_H
#define BOMB_H

#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>

enum BombType { ICE_BOMB, FIRE_BOMB, NORMAL_BOMB };

class Bomb {
public:
    Bomb(int x, int y, SDL_Texture* texture);  // Constructor
    void update();                             // Cập nhật vị trí bom
    void render(SDL_Renderer* renderer) const;      // Vẽ bom
    void explode();                             // Kích hoạt nổ
    bool isActive() const;                     // Kiểm tra bom có đang hoạt động?
    int getX() const;                          // Lấy tọa độ X
    int getY() const;                          // Lấy tọa độ Y
    BombType getType() const;
    void deactivate();
    bool shouldBeRemoved() const {
        return !active && !exploding;
    }
    bool canBeRemoved() const { return shouldBeRemoved(); }
private:
    int x, y;                  // Vị trí bom
    bool active;               // Trạng thái bom
    bool exploding;
    Uint32 explodeStartTime;
    SDL_Texture* texture;      // Hình ảnh bom
    SDL_Texture* explosionTexture; // Hiệu ứng nổ
    Mix_Chunk* explosionSound;  // Thêm dòng này
};

#endif // BOMB_H
