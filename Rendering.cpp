#include "Tetris.h"

void Tetris::renderButton(SDL_Rect button, SDL_Texture* texture, int mouseX, int mouseY) {
    bool isHovered = checkButtonHover(mouseX, mouseY, button);
    SDL_Rect renderRect = button;

    // Hiệu ứng hover tổng hợp
    if (isHovered) {
        // 1. Hiệu ứng lún xuống mượt mà
        renderRect.y += 3;

        // 2. Hiệu ứng bóng đổ đa lớp
        for (int i = 1; i <= 3; i++) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 20 * i);
            SDL_Rect shadow = {
                button.x + i,
                button.y + 3 + i * 2,
                button.w,
                button.h
            };
            SDL_RenderFillRect(renderer, &shadow);
        }

        // 3. Hiệu ứng phát sáng viền
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80);
        for (int i = 0; i < 3; i++) {
            SDL_Rect glowRect = {
                button.x - i,
                button.y - i,
                button.w + i * 2,
                button.h + i * 2
            };
            SDL_RenderDrawRect(renderer, &glowRect);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // 4. Hiệu ứng scale nhẹ
        renderRect.x -= button.w * 0.05f / 2;
        renderRect.y -= button.h * 0.05f / 2;
        renderRect.w += button.w * 0.05f;
        renderRect.h += button.h * 0.05f;
    }

    // Vẽ nút chính với hiệu ứng alpha nếu cần
    SDL_SetTextureAlphaMod(texture, isHovered ? 240 : 255);
    SDL_RenderCopy(renderer, texture, nullptr, &renderRect);
    SDL_SetTextureAlphaMod(texture, 255); // Reset alpha

    // 5. Hiệu ứng viền nổi khi hover
    if (isHovered) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
        SDL_RenderDrawRect(renderer, &renderRect);

        // Viền trong
        SDL_SetRenderDrawColor(renderer, 200, 230, 255, 100);
        SDL_Rect innerBorder = {
            renderRect.x + 2,
            renderRect.y + 2,
            renderRect.w - 4,
            renderRect.h - 4
        };
        SDL_RenderDrawRect(renderer, &innerBorder);
    }
}

void Tetris::renderMenu() {
    // Vẽ hình ảnh nền menu
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, nullptr); // Vẽ background
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nếu không có background, đặt màu nền là đen
        SDL_RenderClear(renderer);
    }

    // Lấy vị trí chuột
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Vẽ các nút với hiệu ứng hover
    renderButton(startButton, startButtonTexture, mouseX, mouseY);
    renderButton(instructionsButton, instructionsButtonTexture, mouseX, mouseY);
    renderButton(exitButton, exitButtonTexture, mouseX, mouseY);
    renderButton(soundButton, isSoundOn ? soundOnTexture : soundOffTexture, mouseX, mouseY);

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
    bool exitInstructions = false;

    while (!exitInstructions) {
        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Hiển thị hình ảnh hướng dẫn
        if (instructionsTexture) {
            SDL_Rect instructionsRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-38};
            SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
        } else {
            SDL_Color textColor = {255, 255, 255, 255};
            renderText("Failed to load instructions image!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, textColor);
        }

        // Lấy vị trí chuột
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Vẽ nút Exit với hiệu ứng hover
        SDL_Rect backButton = {SCREEN_WIDTH - 100, SCREEN_HEIGHT - 90, 80, 80};
        renderButton(backButton, exitConfirmTexture, mouseX, mouseY);

        SDL_RenderPresent(renderer);

        // Xử lý sự kiện
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exitInstructions = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (checkButtonClick(event.button.x, event.button.y, backButton)) {
                    exitInstructions = true;
                }
            }
        }

        // Giảm tải CPU
        SDL_Delay(10);
    }
}

// Hàm render hộp thoại xác nhận
void Tetris::renderExitConfirm() {
    bool exitLoop = false;

    while (!exitLoop) {
        // Vẽ hộp thoại xác nhận
        SDL_SetRenderDrawColor(renderer, 225, 255, 255, 255);
        SDL_Rect dialogBox = {SCREEN_WIDTH/2 - 250, SCREEN_HEIGHT/2 - 150, 500, 300};
        SDL_RenderFillRect(renderer, &dialogBox);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &dialogBox);

        // Vẽ tiêu đề
        SDL_Color textColor = {0, 0, 0, 255};
        const char* message = "Do you want to quit the game?";
        int textWidth, textHeight;
        TTF_SizeText(font, message, &textWidth, &textHeight);
        renderText(message, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - textHeight/2 - 50, textColor);

        // Lấy vị trí chuột
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Vẽ các nút với hiệu ứng hover (cập nhật liên tục)
        SDL_Rect returnButton = {SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2, 100, 100};
        SDL_Rect exitConfirmButton = {SCREEN_WIDTH/2 + 20, SCREEN_HEIGHT/2, 100, 100};

        renderButton(returnButton, returnTexture, mouseX, mouseY);
        renderButton(exitConfirmButton, exitConfirmTexture, mouseX, mouseY);

        SDL_RenderPresent(renderer);

        // Xử lý sự kiện
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exitLoop = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (checkButtonClick(event.button.x, event.button.y, returnButton)) {
                    showExitConfirm = false;
                    exitLoop = true;
                }
                else if (checkButtonClick(event.button.x, event.button.y, exitConfirmButton)) {
                    SDL_Quit();
                    exit(0);
                }
            }
        }

        // Thêm delay nhỏ để giảm tải CPU
        SDL_Delay(10);
    }
}

void Tetris::renderPauseDialog() {
    bool dialogActive = true;
    const Uint32 dialogStartTime = SDL_GetTicks();

    // Thiết lập các nút (kích thước và vị trí mới)
    SDL_Rect buttons[] = {
        {SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 -30 , 100, 100},  // Continue (giữa)
        {SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 - 140 , 100, 100},  // Home
        {SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 80, 100, 100}   // Exit
    };

    while (dialogActive) {
        // 2. Vẽ hộp thoại chính với kích thước phù hợp
        SDL_SetRenderDrawColor(renderer, 225, 255, 255, 255);
        SDL_Rect dialogBox = {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 200, 300, 400};

        SDL_RenderFillRect(renderer, &dialogBox);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &dialogBox);

        // 3. Vẽ tiêu đề với vị trí mới
        SDL_Color textColor = {0, 0, 0, 255};
        const char* message = "GAME PAUSED";
        int textWidth, textHeight;
        TTF_SizeText(font, message, &textWidth, &textHeight);
        renderText(message, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - textHeight/2 - 180, textColor); // Xuống thêm 10px

        // 4. Lấy vị trí chuột
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // 5. Vẽ các nút bằng ảnh với hiệu ứng hover (dùng renderButton)
        renderButton(buttons[0], continueTexture, mouseX, mouseY);  // Continue
        renderButton(buttons[1], homeTexture, mouseX, mouseY);      // Home
        renderButton(buttons[2], exitConfirmTexture, mouseX, mouseY);      // Exit

        SDL_RenderPresent(renderer);

        // 6. Xử lý sự kiện (tương tự exitConfirm)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                dialogActive = false;
                SDL_Quit(); // Thoát SDL
                exit(0);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (checkButtonClick(event.button.x, event.button.y, buttons[0])) {
                    // Continue
                    isPaused = false;
                    dialogActive = false;
                    if (isSoundOn) Mix_ResumeMusic();
                }
                else if (checkButtonClick(event.button.x, event.button.y, buttons[1])) {
                    // Home - Về menu chính
                    returnToMainMenu();
                    dialogActive = false;
                }
                else if (checkButtonClick(event.button.x, event.button.y, buttons[2])) {
                    // Exit game
                    SDL_Quit(); // Thoát SDL
                    exit(0);
                }
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                // Thoát hộp thoại khi ấn ESC
                dialogActive = false;
            }
        }

        SDL_Delay(10); // Giảm tải CPU
    }
}

void Tetris::resetGame() {
    // Reset trạng thái game
    currentScore = 0;
    speed = 500;
    isPaused = false;
    gameOver = false;

    // Làm trống lưới game
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            grid[y][x] = 0;
        }
    }

    // Reset khối hiện tại
    currentPiece.clear();
    spawnPiece(); // Tạo khối mới
    // Xóa tất cả bomb
    bombs.clear();

    // Reset các hiệu ứng đặc biệt
    iceEffectActive = false;
    iceEffectEndTime = 0;
    snowflakes.clear(); // Xóa tất cả bông tuyết(nếu có)

}

void Tetris::returnToMainMenu() {
    resetGame(); // Reset toàn bộ game
    inMenu = true;
    if (isSoundOn) Mix_ResumeMusic();

    renderMenu();
    SDL_RenderPresent(renderer);
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

    // Vẽ lưới và các khối
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (grid[y][x]) {
                drawBlock(x, y, COLORS[grid[y][x] - 1]);
            }
        }
    }

    renderBombs();

    // Vẽ khối hiện tại
    for (auto& block : currentPiece) {
        drawBlock(block.x, block.y, COLORS[shapeIndex]);
    }

    // Lấy vị trí chuột cho hiệu ứng hover
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Vẽ nút pause với hiệu ứng hover
    if (!inMenu) {
        renderButton(pauseButton,isPaused ? pauseButtonTexture : continueTexture,mouseX, mouseY);
    }

    // Vẽ nút âm thanh với hiệu ứng hover
    renderButton(soundButton,isSoundOn ? soundOnTexture : soundOffTexture,mouseX, mouseY);

    // Hiển thị điểm
    SDL_Color textColor = {255, 255, 255, 255};
    char scoreText[40];
    sprintf(scoreText, "Score: %d", currentScore);
    renderText(scoreText, 10, 50, textColor);
    sprintf(scoreText, "High Score: %d", highScore);
    renderText(scoreText, 10, 80, textColor);

    renderSnow();
    SDL_RenderPresent(renderer);
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
