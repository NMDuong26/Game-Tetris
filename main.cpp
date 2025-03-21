#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
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

// Lớp chính của trò chơi Tetris
class Tetris {
private:
    // Đường dẫn đến các hình ảnh nút
    const char* startButtonImagePath = "start.png";
    const char* instructionsButtonImagePath = "instructions.png";
    const char* exitButtonImagePath = "exit.png";
    const char* soundOnImagePath = "sound_on.png";
    const char* soundOffImagePath = "sound_off.png";
    const char* pauseButtonImagePath = "pause.png";
    const char* continueImagePath = "continue.png";

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

    // Vẽ menu chính
    void renderMenu() {
        // Vẽ hình ảnh nền menu
        if (menuBackgroundTexture) {
            SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, nullptr); // Vẽ background
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nếu không có background, đặt màu nền là đen
            SDL_RenderClear(renderer);
        }

        // Vẽ các nút
        SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButton);
        SDL_RenderCopy(renderer, instructionsButtonTexture, nullptr, &instructionsButton);
        SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButton);
        SDL_RenderCopy(renderer, isSoundOn ? soundOnTexture : soundOffTexture, nullptr, &soundButton);

        SDL_RenderPresent(renderer); // Cập nhật màn hình
    }

    // Vẽ văn bản lên màn hình
    void renderText(const char* text, int x, int y, SDL_Color color) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, color); // Tạo surface từ văn bản
        if (!surface) {
            printf("Failed to render text: %s\n", TTF_GetError());
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // Tạo texture từ surface
        if (!texture) {
            printf("Failed to create texture: %s\n", SDL_GetError());
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect rect = {x, y, surface->w, surface->h}; // Vị trí và kích thước văn bản
        SDL_RenderCopy(renderer, texture, nullptr, &rect); // Vẽ texture lên renderer

        // Giải phóng bộ nhớ
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Kiểm tra xem chuột có nhấp vào nút không
    bool checkButtonClick(int mouseX, int mouseY, SDL_Rect button) {
        return (mouseX >= button.x && mouseX <= button.x + button.w &&
                mouseY >= button.y && mouseY <= button.y + button.h);
    }

    // Hiển thị màn hình hướng dẫn
    void renderInstructions() {
        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Hiển thị hình ảnh hướng dẫn
        if (instructionsTexture) {
            SDL_Rect instructionsRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-38}; // Hiển thị toàn màn hình
            SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
        } else {
            // Nếu không tải được hình ảnh, hiển thị thông báo lỗi
            SDL_Color textColor = {255, 255, 255, 255};
            renderText("Failed to load instructions image!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, textColor);
        }

        // Hiển thị nút quay lại (Exit) ở góc phải màn hình
        SDL_Rect backButton = {SCREEN_WIDTH - 110, SCREEN_HEIGHT - 60, 100, 50}; // Vị trí góc phải dưới
        SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &backButton);

        SDL_RenderPresent(renderer);

        // Chờ người dùng nhấp vào nút quay lại
        SDL_Event event;
        while (true) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) return;
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    if (checkButtonClick(mouseX, mouseY, backButton)) {
                        return;
                    }
                }
            }
        }
    }

public:
    // Constructor: Khởi tạo các thành phần của trò chơi
    Tetris() :  window(nullptr), renderer(nullptr), backgroundTexture(nullptr), backgroundMusic(nullptr),
                   moveSound(nullptr), clearSound(nullptr), speed(500), gameOver(false), inMenu(true),
                   font(nullptr), isSoundOn(true), isContinue(true) {
        grid = vector<vector<int>>(ROWS, vector<int>(COLS, 0));

        // Khởi tạo SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            printf("Failed to initialize SDL: %s\n", SDL_GetError());
            return;
        }

        // Khởi tạo SDL_image
        if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
            printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
            return;
        }

        // Khởi tạo SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("Failed to initialize SDL_mixer: %s\n", Mix_GetError());
            return;
        }

        // Khởi tạo SDL_ttf
        if (TTF_Init() == -1) {
            printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
            return;
        }

        // Tải font chữ
        font = TTF_OpenFont("ariblk.ttf", 24); // Đường dẫn đến file font
        if (!font) {
            printf("Failed to load font: %s\n", TTF_GetError());
            return;
        }

        // Tạo cửa sổ và renderer
        window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            printf("Failed to create window: %s\n", SDL_GetError());
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            printf("Failed to create renderer: %s\n", SDL_GetError());
            return;
        }

        // Tải ảnh cho các nút
        startButtonTexture = loadTexture(startButtonImagePath, renderer);
        instructionsButtonTexture = loadTexture(instructionsButtonImagePath, renderer);
        exitButtonTexture = loadTexture(exitButtonImagePath, renderer);
        soundOnTexture = loadTexture(soundOnImagePath, renderer);
        soundOffTexture = loadTexture(soundOffImagePath, renderer);
        pauseButtonTexture = loadTexture(pauseButtonImagePath, renderer);
        continueTexture = loadTexture(continueImagePath, renderer);

        // Kiểm tra xem các texture có được tải thành công không
        if (!startButtonTexture || !instructionsButtonTexture || !exitButtonTexture ||
            !soundOnTexture || !soundOffTexture || !pauseButtonTexture || !continueTexture) {
            printf("Failed to load button textures!\n");
        }
        // Tải ảnh cho phần hướng dẫn
        instructionsTexture = loadTexture("instructions_background.png", renderer);
        if (!instructionsTexture) {
            printf("Failed to load instructions image!\n");
        }

        // Tải hình nền và âm thanh
        loadMenuBackground("menu_background.png");
        loadBackground("image.png");
        loadSounds();
        spawnPiece();
        loadHighScore(); // Đọc điểm cao nhất khi khởi động game
    }

    // Hàm tải texture từ file
    SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
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

    // Destructor: Giải phóng bộ nhớ và tài nguyên
    ~Tetris() {
        TTF_CloseFont(font); // Giải phóng font
        TTF_Quit(); // Đóng SDL_ttf
        SDL_DestroyTexture(menuBackgroundTexture);
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(instructionsTexture);
        SDL_DestroyTexture(startButtonTexture);
        SDL_DestroyTexture(instructionsButtonTexture);
        SDL_DestroyTexture(exitButtonTexture);
        SDL_DestroyTexture(soundOnTexture);
        SDL_DestroyTexture(soundOffTexture);
        SDL_DestroyTexture(pauseButtonTexture);
        SDL_DestroyTexture(continueTexture);
        Mix_FreeMusic(backgroundMusic);
        Mix_FreeChunk(moveSound);
        Mix_FreeChunk(clearSound);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        saveHighScore(); // Lưu điểm cao nhất khi thoát game
    }

    // Tải hình nền menu
    void loadMenuBackground(const char* filePath) {
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

    // Tải hình nền game
    void loadBackground(const char* filePath) {
        SDL_Surface* loadedSurface = IMG_Load(filePath);
        if (!loadedSurface) {
            printf("Failed to load background: %s\n", IMG_GetError());
            return;
        }
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }

    // Tải âm thanh
    void loadSounds() {
        backgroundMusic = Mix_LoadMUS("background_music.mp3");
        moveSound = Mix_LoadWAV("move_sound.wav");
        clearSound = Mix_LoadWAV("clear_line.wav");
        if (!backgroundMusic || !moveSound || !clearSound) {
            printf("Failed to load sounds: %s\n", Mix_GetError());
        }
    }

    // Tạo khối mới
    void spawnPiece() {
        shapeIndex = rand() % 7;
        currentPiece.clear();
        for (int i = 0; i < 4; i++)
            currentPiece.push_back({SHAPES[shapeIndex][i][0] + COLS / 2 - 1, SHAPES[shapeIndex][i][1]});
        if (collides()) gameOver = true; // Nếu va chạm, đặt gameOver = true
    }

    // Vẽ màn hình game over
    void renderGameOver() {
        // Tải hình ảnh game over
        SDL_Surface* gameOverSurface = IMG_Load("game_over.png");
        if (!gameOverSurface) {
            printf("Failed to load game over image: %s\n", IMG_GetError());
            return;
        }

        SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        if (!gameOverTexture) {
            printf("Failed to create game over texture: %s\n", SDL_GetError());
            SDL_FreeSurface(gameOverSurface);
            return;
        }

        // Giải phóng surface
        SDL_FreeSurface(gameOverSurface);

        // Xóa màn hình và vẽ hình ảnh game over
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu nền là đen
        SDL_RenderClear(renderer);

        // Lấy kích thước hình ảnh
        int imgWidth = gameOverSurface->w;
        int imgHeight = gameOverSurface->h;

        // Tính toán vị trí để hiển thị hình ảnh ở giữa màn hình
        int x = (SCREEN_WIDTH - imgWidth) / 2;
        int y = (SCREEN_HEIGHT - imgHeight) / 2;

        // Vẽ hình ảnh game over
        SDL_Rect destRect = {x, y, imgWidth, imgHeight};
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, &destRect);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);

        // Giải phóng texture
        SDL_DestroyTexture(gameOverTexture);

        // Dừng 3 giây trước khi thoát
        SDL_Delay(3000);

        // Thoát khỏi game
        return;
    }

    // Kiểm tra va chạm
    bool collides(int dx = 0, int dy = 1, vector<Block> testPiece = {}) {
        if (testPiece.empty()) testPiece = currentPiece;
        for (auto& block : testPiece) {
            int x = block.x + dx, y = block.y + dy;
            if (x < 0 || x >= COLS || y >= ROWS || (y >= 0 && grid[y][x])) return true;
        }
        return false;
    }

    // Hợp nhất khối vào lưới
    void mergePiece() { for (auto& block : currentPiece) grid[block.y][block.x] = shapeIndex + 1; }

    // Đọc điểm cao nhất từ file
    void loadHighScore() {
        FILE* file = fopen("highscore.txt", "r");
        if (file) {
            fscanf(file, "%d", &highScore); // Đọc điểm cao nhất từ file
            fclose(file);
        } else {
            highScore = 0; // Nếu file không tồn tại, đặt điểm cao nhất là 0
        }
    }

    // Lưu điểm cao nhất vào file
    void saveHighScore() {
        FILE* file = fopen("highscore.txt", "w");
        if (file) {
            fprintf(file, "%d", highScore); // Lưu điểm cao nhất vào file
            fclose(file);
        }
    }

    // Xóa các hàng đã đầy
    void clearLines() {
        for (int y = ROWS - 1; y >= 0; y--) {
            bool full = true; // Giả sử hàng đầy
            // Kiểm tra xem hàng có đầy hay không
            for (int x : grid[y]) if (!x) { full = false; break; }
            // Nếu hàng đầy thì xóa hàng và di chuyển các hàng ở trên xuống
            if (full) {
                for (int yy = y; yy > 0; yy--) grid[yy] = grid[yy - 1]; // Di chuyển hàng phía trên xuống
                grid[0] = vector<int>(COLS, 0); // Đặt hàng trên cùng thành trống
                speed = max(100, speed - 20);   // Tăng tốc độ rơi (tùy chọn)
                if(isSoundOn) Mix_PlayChannel(-1, clearSound, 0);  // Phát âm thanh khi xóa hàng

                // Tăng điểm hiện tại
                currentScore += 100; // Thưởng 100 điểm cho mỗi hàng xóa

                y++; // Kiểm tra lại hàng hiện tại vì các hàng đã di chuyển xuống
            }
        }
    }

    // Di chuyển khối
    void movePiece(int dx) {
        // Tạo một bản sao của khối hiện tại để kiểm tra va chạm
        vector<Block> testPiece = currentPiece;
        for (auto& block : testPiece) {
            block.x += dx; // Di chuyển khối theo hướng dx (trái hoặc phải)
        }

        // Kiểm tra xem khối có va chạm với tường hoặc các khối khác không
        bool canMove = true;
        for (auto& block : testPiece) {
            if (block.x < 0 || block.x >= COLS || (block.y >= 0 && grid[block.y][block.x])) {
                canMove = false; // Nếu có va chạm, không di chuyển
                break;
            }
        }

        // Nếu không có va chạm, di chuyển khối hiện tại
        if (canMove) {
            for (auto& block : currentPiece) {
                block.x += dx;
            }
            if (isSoundOn) Mix_PlayChannel(-1, moveSound, 0); // Phát âm thanh nếu âm thanh được bật
        }
    }

    // Cập nhật trạng thái game
    void update() {
        if (!collides()) {
            for (auto& block : currentPiece) block.y++; // Di chuyển khối xuống
        } else {
            mergePiece(); // Hợp nhất khối vào lưới
            clearLines(); // Xóa các dòng đã hoàn thành
            spawnPiece(); // Tạo khối mới
        }

        // Cập nhật điểm cao nhất
        if (currentScore >= highScore) {
            highScore = currentScore;
            saveHighScore(); // Lưu điểm cao nhất vào file
        }
    }

    // Vẽ trò chơi
    void render() {
        renderBackground(); // Vẽ nền

        // Vẽ lưới
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (grid[y][x]) {
                    drawBlock(x, y, COLORS[grid[y][x] - 1]); // Vẽ các khối đã hợp nhất
                }
            }
        }

        // Vẽ khối hiện tại
        for (auto& block : currentPiece) {
            drawBlock(block.x, block.y, COLORS[shapeIndex]);
        }
        if (!inMenu) {
            SDL_RenderCopy(renderer, isContinue ? continueTexture : pauseButtonTexture, nullptr, &pauseButton);
        }
        SDL_RenderCopy(renderer, isSoundOn ? soundOnTexture : soundOffTexture, nullptr, &soundButton);

        // Hiển thị điểm hiện tại và điểm cao nhất
        SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng
        char scoreText[40];
        sprintf(scoreText, "Score: %d", currentScore); // Điểm hiện tại
        renderText(scoreText, 10, 50, textColor);

        sprintf(scoreText, "High Score: %d", highScore); // Điểm cao nhất
        renderText(scoreText, 10, 80, textColor);

        SDL_RenderPresent(renderer); // Cập nhật màn hình
    }

    // Vẽ nền
    void renderBackground() {
        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr); // Vẽ texture nền
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen nếu không có texture
            SDL_RenderClear(renderer);
        }
    }

    // Vẽ một khối
    void drawBlock(int x, int y, SDL_Color color) {
        SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect); // Vẽ khối

        // Vẽ viền
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }

    // Xử lý sự kiện trong menu
    void handleMenuEvents(SDL_Event& event) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;

            if (checkButtonClick(mouseX, mouseY, startButton)) {
                inMenu = false; // Bắt đầu game
                currentScore = 0; // Reset điểm hiện tại
            } else if (checkButtonClick(mouseX, mouseY, instructionsButton)) {
                renderInstructions(); // Hiển thị hướng dẫn
            } else if (checkButtonClick(mouseX, mouseY, exitButton)) {
                gameOver = true; // Thoát game
            } else if (checkButtonClick(mouseX, mouseY, soundButton)) {
                isSoundOn = !isSoundOn;
                if (isSoundOn) Mix_ResumeMusic(); // Bật nhạc
                else Mix_PauseMusic(); // Tắt nhạc
            }
        }
    }

    // Xử lý sự kiện trong game
    void handleGameEvents(SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT: movePiece(-1); break;
                case SDLK_RIGHT: movePiece(1); break;
                case SDLK_DOWN: dropPiece(); break;
                case SDLK_UP: rotatePiece(); break;
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = event.button.x;
            int mouseY = event.button.y;

            if (checkButtonClick(mouseX, mouseY, pauseButton)) {
                isContinue = !isContinue; // Đảo trạng thái continue/pause

                // Kiểm tra cả hai điều kiện: isSoundOn và isContinue
                if (isSoundOn && isContinue) {
                    Mix_ResumeMusic(); // Bật nhạc nếu cả hai điều kiện đều đúng
                } else {
                    Mix_PauseMusic(); // Tắt nhạc nếu một trong hai điều kiện sai
                }

                render(); // Render lại màn hình để cập nhật nút pause/continue
            }

            if (checkButtonClick(mouseX, mouseY, soundButton)) {
                isSoundOn = !isSoundOn;
                if (isSoundOn && isContinue) {
                    Mix_ResumeMusic(); // Bật nhạc
                } else {
                    Mix_PauseMusic(); // Tắt nhạc
                }
                render(); // Render lại màn hình để cập nhật nút bật/tắt tiếng
            }
        }
    }

    // Thả khối xuống nhanh
    void dropPiece() {
        while (!collides()) {
            for (auto& block : currentPiece) block.y++;
        }
        mergePiece();
        clearLines();
        spawnPiece();

        if (isSoundOn) Mix_PlayChannel(-1, moveSound, 0); // Phát âm thanh khi khối rơi

        currentScore += 10; // Điểm thưởng
    }

    // Xoay khối
    void rotatePiece() {
        int pivotX = currentPiece[1].x, pivotY = currentPiece[1].y;
        vector<Block> rotated;
        for (auto& block : currentPiece) {
            int newX = pivotX - (block.y - pivotY);
            int newY = pivotY + (block.x - pivotX);
            rotated.push_back({newX, newY});
        }
        if (!collides(0, 0, rotated)) {
            currentPiece = rotated;
        }
    }

    // Chạy trò chơi
    void run() {
        SDL_Event event;
        Uint32 lastTick = SDL_GetTicks();
        Mix_PlayMusic(backgroundMusic, -1); // Bật nhạc nền

        while (!gameOver) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    gameOver = true;
                    return;
                }

                if (inMenu) {
                    handleMenuEvents(event); // Xử lý sự kiện menu
                } else {
                    handleGameEvents(event); // Xử lý sự kiện game
                }
            }

            if (inMenu) {
                renderMenu(); // Vẽ menu nếu đang ở trong menu
            } else if (isContinue) {
                Uint32 currentTick = SDL_GetTicks();
                if (currentTick - lastTick > speed) {
                    lastTick = currentTick;
                    update(); // Cập nhật game
                }
                render(); // Vẽ game
            }

            if (gameOver) {
                renderGameOver(); // Hiển thị màn hình game over
                SDL_Delay(3000); // Dừng 3 giây
                return;
            }
        }
    }
};

// Hàm main
int main(int argc, char* argv[]) {
    srand(time(0)); // Khởi tạo seed cho hàm rand()
    Tetris game;
    game.run();
    return 0;
}
