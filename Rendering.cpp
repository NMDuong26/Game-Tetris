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

        // Vẽ tiêu đề với hiệu ứng
        SDL_Color textColor = {0, 0, 0, 255}; // Màu chữ đen
        SDL_Color glowColor = {100, 150, 255, 80}; // Màu viền phát sáng xanh
        const char* message = "Do you want to quit the game?";
        int textWidth, textHeight;
        TTF_SizeText(font, message, &textWidth, &textHeight);

        // Vẽ bóng đổ trước (tạo độ sâu)
        renderTextWithEffect(message,
                        SCREEN_WIDTH/2 - textWidth/2 + 1,
                        SCREEN_HEIGHT/2 - textHeight/2 - 50 + 1,
                        {50, 50, 50, 150}, // Màu bóng xám
                        false); // Không glow

        // Vẽ text chính với hiệu ứng phát sáng
        renderTextWithEffect(message,
                        SCREEN_WIDTH/2 - textWidth/2,
                        SCREEN_HEIGHT/2 - textHeight/2 - 50,
                        textColor,
                        true); // Có glow

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

       // Vẽ tiêu đề "GAME PAUSED" với hiệu ứng nổi bật
        SDL_Color textColor = {0, 0, 0, 255}; // Màu chữ trắng
        SDL_Color glowColor = {100, 200, 255, 120}; // Màu viền phát sáng xanh nhạt
        const char* message = "GAME PAUSED";
        int textWidth, textHeight;
        TTF_SizeText(font, message, &textWidth, &textHeight);

            // Vị trí trung tâm (đã điều chỉnh xuống 180px từ giữa màn hình)
        int textX = SCREEN_WIDTH/2 - textWidth/2;
        int textY = SCREEN_HEIGHT/2 - textHeight/2 - 172;

        // Vẽ bóng đổ (lệch 3px để tạo chiều sâu)
        renderTextWithEffect(message,
                        textX + 1,
                        textY + 1,
                        {0, 0, 0, 180}, // Màu bóng đen
                        false); // Không glow

        // Vẽ text chính với hiệu ứng phát sáng mạnh
        renderTextWithEffect(message,
                        textX,
                        textY,
                        textColor,
                        true); // Có glow

        // Tùy chọn: Thêm hiệu ứng highlight trắng nhẹ
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 60);
        SDL_Rect highlightRect = {
            textX - 5,
            textY - 5,
            textWidth + 10,
            textHeight + 10
        };
        SDL_RenderDrawRect(renderer, &highlightRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

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
    renderBackground();

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

    // Lấy vị trí chuột (đã sửa lỗi chính tả GetWouseState -> GetMouseState)
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Vẽ các nút
    if (!inMenu) {
        renderButton(pauseButton, isPaused ? pauseButtonTexture : continueTexture, mouseX, mouseY);
    }
    renderButton(soundButton, isSoundOn ? soundOnTexture : soundOffTexture, mouseX, mouseY);

    // Hiển thị điểm
    SDL_Color textColor = {225, 225, 225, 255}; // Trắng
    SDL_Color shadowColor = {0, 0, 0, 255};   // Màu bóng đổ

    char scoreText[40];
    sprintf(scoreText, "SCORE: %d", currentScore);
    renderTextWithEffect(scoreText, 12, 52, shadowColor, false);

    renderTextWithEffect(scoreText, 10, 50, textColor, true);

    sprintf(scoreText, "HIGH SCORE: %d", highScore);
    renderTextWithEffect(scoreText, 12, 93, shadowColor, false);
    renderTextWithEffect(scoreText, 10, 91, textColor, true);

    renderSnow();
    SDL_RenderPresent(renderer);
}

void Tetris::renderTextWithEffect(const char* text, int x, int y, SDL_Color color, bool withGlow) {
    // Tạo surface và texture từ text
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};

    // Hiệu ứng đổ bóng (vẽ trước text chính)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(textTexture, 80); // Độ trong suốt của bóng
    SDL_Rect shadowRect = {
        textRect.x + 2, // Độ lệch bóng
        textRect.y + 2,
        textRect.w,
        textRect.h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &shadowRect);
    SDL_SetTextureAlphaMod(textTexture, 255); // Reset alpha

    // Hiệu ứng phát sáng viền khi withGlow = true
    if (withGlow) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // Màu xanh phát sáng

        // Vẽ 3 lớp glow với kích thước tăng dần
        for (int i = 0; i < 3; i++) {
            SDL_Rect glowRect = {
                textRect.x - i,
                textRect.y - i,
                textRect.w + i * 2,
                textRect.h + i * 2
            };
            SDL_RenderDrawRect(renderer, &glowRect);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }

    // Vẽ text chính
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    // Giải phóng bộ nhớ
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
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

    // 1. Vẽ khối chính với màu đặc
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // 2. Hiệu ứng viền 3D sắc nét
    // Viền sáng (top và left)
    SDL_SetRenderDrawColor(renderer,
        min(color.r + 80, 255),
        min(color.g + 80, 255),
        min(color.b + 80, 255),
        255);
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y); // Top
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h - 1); // Left

    // Viền tối (bottom và right)
    SDL_SetRenderDrawColor(renderer,
        max(color.r - 80, 0),
        max(color.g - 80, 0),
        max(color.b - 80, 0),
        255);
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y, rect.x + rect.w - 1, rect.y + rect.h); // Right
    SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h - 1, rect.x + rect.w, rect.y + rect.h - 1); // Bottom

    // 3. Hiệu ứng phát sáng viền ôm trọn khối
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // Màu xanh phát sáng

    // Vẽ 3 lớp glow với kích thước tăng dần
    for (int i = 1; i <= 3; i++) {
        SDL_Rect glowRect = {
            rect.x - i,
            rect.y - i,
            rect.w + i * 2,
            rect.h + i * 2
        };
        // Chỉ vẽ phần glow không bị che bởi khối khác
        if (x > 0 && grid[y][x-1] == 0) // Kiểm tra ô bên trái
            SDL_RenderDrawLine(renderer, glowRect.x, glowRect.y, glowRect.x, glowRect.y + glowRect.h);
        if (x < COLS-1 && grid[y][x+1] == 0) // Kiểm tra ô bên phải
            SDL_RenderDrawLine(renderer, glowRect.x + glowRect.w, glowRect.y, glowRect.x + glowRect.w, glowRect.y + glowRect.h);
        if (y > 0 && grid[y-1][x] == 0) // Kiểm tra ô phía trên
            SDL_RenderDrawLine(renderer, glowRect.x, glowRect.y, glowRect.x + glowRect.w, glowRect.y);
        if (y < ROWS-1 && grid[y+1][x] == 0) // Kiểm tra ô phía dưới
            SDL_RenderDrawLine(renderer, glowRect.x, glowRect.y + glowRect.h, glowRect.x + glowRect.w, glowRect.y + glowRect.h);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 4. Viền đen sắc nét (vẽ sau cùng)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}
