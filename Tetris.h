#ifndef TETRIS_H
#define TETRIS_H

#include "Bomb.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<algorithm>
#include <iostream>

using namespace std;

// Kích thước màn hình và khối
const int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 600, BLOCK_SIZE = 50;
const int ROWS = SCREEN_HEIGHT / BLOCK_SIZE, COLS = SCREEN_WIDTH / BLOCK_SIZE;

// Các hình dạng của khối Tetris (I, O, T, L, J, S, Z)
const int SHAPES[7][4][2] = {
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}}, // I
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // O
    {{0, 1}, {1, 1}, {2, 1}, {1, 0}}, // T
    {{0, 2}, {0, 1}, {0, 0}, {1, 0}}, // L
    {{1, 2}, {1, 1}, {1, 0}, {0, 0}}, // J
    {{1, 1}, {2, 1}, {0, 0}, {1, 0}}, // S
    {{0, 1}, {1, 1}, {1, 0}, {2, 0}}  // Z
};

// Màu sắc cho từng loại khối
const SDL_Color COLORS[7] = {
    {0, 255, 255, 255}, // I - Cyan
    {255, 255, 0, 255},   // O - Yellow
    {128, 0, 128, 255},   // T - Purple
    {255, 165, 0, 255},   // L - Orange
    {0, 0, 255, 255},     // J - Blue
    {0, 255, 0, 255},     // S - Green
    {255, 0, 0, 255}      // Z - Red
};

// Cấu trúc đại diện cho một khối trong trò chơi
struct Block { int x, y; };



class Tetris {
private:
    // Con trỏ SDL để quản lý cửa sổ, renderer, texture, âm thanh, font chữ
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;
    SDL_Texture* startButtonTexture;
    SDL_Texture* instructionsButtonTexture;
    SDL_Texture* instructionsTexture;
    SDL_Texture* exitButtonTexture;
    SDL_Texture* soundOnTexture;
    SDL_Texture* soundOffTexture;
    SDL_Texture* pauseButtonTexture;
    SDL_Texture* continueTexture;
    Mix_Music* backgroundMusic;
    Mix_Chunk* moveSound;
    Mix_Chunk* clearSound;
    TTF_Font* font;
    SDL_Texture* menuBackgroundTexture;
    SDL_Texture* bombTexture;         // Texture bom
    SDL_Texture* explosionTexture;    // Texture nổ
    Mix_Chunk* explosionSound;        // Âm thanh nổ

    vector<Bomb> bombs;          // Danh sách bom

    // Lưới trò chơi và khối hiện tại
    vector<vector<int>> grid;
    vector<Block> currentPiece;
    int speed, shapeIndex;
    int currentScore = 0; // Điểm hiện tại
    int highScore = 0;    // Điểm cao nhất
    bool gameOver;
    bool inMenu;
    bool isSoundOn; // Trạng thái âm thanh (bật/tắt)
    bool isContinue;  // Trạng thái tạm dừng

    // Vị trí và kích thước các nút
    SDL_Rect startButton = {SCREEN_WIDTH / 2 - 100, 270, 200, 80};
    SDL_Rect instructionsButton = {SCREEN_WIDTH / 2 - 100, 360, 200, 80};
    SDL_Rect exitButton = {SCREEN_WIDTH / 2 - 100, 450, 200, 60};
    SDL_Rect soundButton = {SCREEN_WIDTH - 1000, 10, 60, 30};
    SDL_Rect pauseButton = {SCREEN_WIDTH - 100, 20, 60, 60};

public:
    Tetris();
    SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
    ~Tetris();
    void renderMenu();
    void renderText(const char* text, int x, int y, SDL_Color color);
    bool checkButtonClick(int mouseX, int mouseY, SDL_Rect button);
    void renderInstructions();
    void renderGameOver();
    bool collides(int dx = 0, int dy = 1, vector<Block> testPiece = {});
    void mergePiece();
    void clearLines();
    void movePiece(int dx);
    void dropPiece();
    void rotatePiece();
    void update();
    void render();
    void renderBackground();
    void drawBlock(int x, int y, SDL_Color color);
    void handleMenuEvents(SDL_Event& event);
    void handleGameEvents(SDL_Event& event);
    void loadHighScore();
    void saveHighScore();
    void loadMenuBackground(const char* filePath);
    void loadBackground(const char* filePath);
    void loadSounds();
    void spawnPiece();
    void run();

    void spawnRandomBomb();           // Tạo bom ngẫu nhiên
    void updateBombs();               // Cập nhật bom
    void renderBombs();               // Vẽ bom
    void explodeBomb(int x, int y);   // Xử lý nổ bom
    void explodeAirBomb();
};

#endif // TETRIS_H
