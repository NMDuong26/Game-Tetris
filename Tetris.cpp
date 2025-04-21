#include "Tetris.h"

// Constructor
Tetris::Tetris() : window(nullptr), renderer(nullptr), backgroundTexture(nullptr), backgroundMusic(nullptr),
                   moveSound(nullptr), clearSound(nullptr), speed(500), gameOver(false), inMenu(true),
                   font(nullptr), isSoundOn(true), isPaused(false) {
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
    startButtonTexture = loadTexture("start.png", renderer);
    instructionsButtonTexture = loadTexture("instructions.png", renderer);
    exitButtonTexture = loadTexture("exit.png", renderer);
    soundOnTexture = loadTexture("sound_on.png", renderer);
    soundOffTexture = loadTexture("sound_off.png", renderer);
    pauseButtonTexture = loadTexture("pause.png", renderer);
    continueTexture = loadTexture("continue.png", renderer);
    homeTexture = loadTexture("home.png", renderer);

    returnTexture = loadTexture("return.png", renderer);
    exitConfirmTexture = loadTexture("evacuation.png", renderer);

    iceTexture = loadTexture("ice_bomb.png", renderer);
    snowTexture = loadTexture("snowflake.png", renderer);
    bombTexture = loadTexture("bomb.png", renderer);
    explosionTexture = loadTexture("explosion.png", renderer);
    icenoTexture = loadTexture("iceno.png", renderer);

    isSnowing = false;
    snowStartTime = 0;

    // Kiểm tra xem các texture có được tải thành công không
    if (!startButtonTexture || !instructionsButtonTexture || !exitButtonTexture ||
        !soundOnTexture || !soundOffTexture || !pauseButtonTexture || !continueTexture || !bombTexture || !explosionTexture
        || !iceTexture || !snowTexture || !icenoTexture || !homeTexture ) {
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
    initSnow();
}

// Destructor
Tetris::~Tetris() {
    TTF_CloseFont(font); // Giải phóng font
    TTF_Quit(); // Đóng SDL_ttf
    // Giải phóng ảnh
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
    SDL_DestroyTexture(homeTexture);
    SDL_DestroyTexture(iceTexture);
    SDL_DestroyTexture(snowTexture);
    SDL_DestroyTexture(bombTexture);
    SDL_DestroyTexture(explosionTexture);
    SDL_DestroyTexture(icenoTexture);
    SDL_DestroyTexture(returnTexture);
    SDL_DestroyTexture(exitConfirmTexture);
    // Giải phóng âm thanh
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(moveSound);
    Mix_FreeChunk(clearSound);
    Mix_FreeChunk(explosionSound);
    Mix_FreeChunk(iceSound);
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    saveHighScore(); // Lưu điểm cao nhất khi thoát game
}
