#include "Tetris.h"

void Tetris::renderMenu() {
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

void Tetris::renderText(const char* text, int x, int y, SDL_Color color) {
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

void Tetris::renderInstructions() {
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

void Tetris::renderGameOver() {
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

    // Dừng trước khi thoát
    SDL_Delay(1000);

    // Thoát khỏi game
    return;
}

void Tetris::render() {
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

void Tetris::renderBackground() {
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr); // Vẽ texture nền
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu nền đen nếu không có texture
        SDL_RenderClear(renderer);
    }
}

void Tetris::drawBlock(int x, int y, SDL_Color color) {
    SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect); // Vẽ khối

    // Vẽ viền
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
