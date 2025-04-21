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
    SDL_Texture* homeTexture;
    Mix_Music* backgroundMusic;
    Mix_Chunk* moveSound;
    Mix_Chunk* clearSound;
    TTF_Font* font;
    SDL_Texture* menuBackgroundTexture;

    bool showExitConfirm = false;
    SDL_Texture* returnTexture;
    SDL_Texture* exitConfirmTexture;

    // Texture, âm thanh bom
    SDL_Texture* iceTexture;
    SDL_Texture* bombTexture;         // Texture bom
    SDL_Texture* explosionTexture;
    SDL_Texture* icenoTexture;
    Mix_Chunk* explosionSound;
    Mix_Chunk* iceSound;

    vector<Bomb> bombs;          // Danh sách bom

    // Lưới trò chơi và khối hiện tại
    vector<vector<int>> grid;
    vector<Block> currentPiece;

    int speed, shapeIndex; // Tốc độ, chỉ số chọn khối.
    int currentScore = 0; // Điểm hiện tại
    int highScore = 0;    // Điểm cao nhất
    bool gameOver;
    bool inMenu;
    bool isSoundOn; // Trạng thái âm thanh (bật/tắt)
    bool isPaused; // Trạng thái tạm dừng

    bool iceEffectActive; // Trạng thái tuyết rơi
    Uint32 iceEffectEndTime;
    float originalSpeed;

    // Hiệu ứng tuyết
    struct Snowflake { float x, y, speed; };
    vector<Snowflake> snowflakes;
    bool isSnowing;
    Uint32 snowStartTime;
    SDL_Texture* snowTexture;

    // Vị trí và kích thước các nút
    SDL_Rect startButton = {SCREEN_WIDTH / 2 - 100, 270, 200, 80};
    SDL_Rect instructionsButton = {SCREEN_WIDTH / 2 - 100, 360, 200, 80};
    SDL_Rect exitButton = {SCREEN_WIDTH / 2 - 100, 450, 200, 60};
    SDL_Rect soundButton = {SCREEN_WIDTH - 1000, 10, 60, 30};
    SDL_Rect pauseButton = {SCREEN_WIDTH - 100, 20, 60, 60};

    SDL_Rect returnButton = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 80};
    SDL_Rect exitConfirmButton = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 200, 80};

public:
    Tetris();
    SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
    ~Tetris();
    void renderButton(SDL_Rect button, SDL_Texture* texture, int mouseX, int mouseY); // Vẽ các nút có hiệu ứng hover
    void renderMenu();
    void renderText(const char* text, int x, int y, SDL_Color color);
    bool checkButtonClick(int mouseX, int mouseY, SDL_Rect button);
    bool checkButtonHover(int mouseX, int mouseY, SDL_Rect button);
    void renderInstructions();
    void renderExitConfirm();
    void renderPauseDialog();
    void returnToMainMenu();
    void resetGame();
    void renderGameOver();
    bool collides(int dx = 0, int dy = 1, vector<Block> testPiece = {}); // Kiểm tra va chạm
    void mergePiece(); // Hợp khối vào lưới
    void clearLines(); // Xóa hàng đã đầy
    void movePiece(int dx); // Di chuyển trái phải
    void dropPiece(); // Di chuyển nhanh xuống
    void rotatePiece(); // Xoay khối 90 độ
    void update();
    void renderTextWithEffect(const char* text, int x, int y, SDL_Color color, bool withGlow);
    void render();
    void renderBackground();

    void drawBlock(int x, int y, SDL_Color color); // Vẽ các khối
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
    void explodeAirBomb(bool isSpaceKey);

    void activateIceEffect(Uint32 duration); //Hiệu ứng tuyết
    void initSnow(); // Tạo tuyết
    void updateSnow();
    void renderSnow();
};

#endif // TETRIS_H
