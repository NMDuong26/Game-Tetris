#include "Tetris.h"

void Tetris::spawnPiece() {
    shapeIndex = rand() % 7;
    currentPiece.clear();
    for (int i = 0; i < 4; i++)
        currentPiece.push_back({SHAPES[shapeIndex][i][0] + COLS / 2 - 1, SHAPES[shapeIndex][i][1]});
    if (collides()) gameOver = true; // Nếu va chạm, đặt gameOver = true
}

bool Tetris::collides(int dx, int dy, vector<Block> testPiece) {
    if (testPiece.empty()) testPiece = currentPiece;
    for (auto& block : testPiece) {
        int x = block.x + dx, y = block.y + dy;
        if (x < 0 || x >= COLS || y >= ROWS || (y >= 0 && grid[y][x])) return true;
    }
    return false;
}

void Tetris::mergePiece() { for (auto& block : currentPiece) grid[block.y][block.x] = shapeIndex + 1; }

void Tetris::clearLines() {
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

void Tetris::movePiece(int dx) {
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

void Tetris::dropPiece() {
    while (!collides()) {
        for (auto& block : currentPiece) block.y++;
    }
    mergePiece();
    clearLines();
    spawnPiece();

    if (isSoundOn) Mix_PlayChannel(-1, moveSound, 0); // Phát âm thanh khi khối rơi

    currentScore += 10; // Điểm thưởng
}

void Tetris::rotatePiece() {
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

void Tetris::explodeAirBomb() {
    for (auto& bomb : bombs) {
        if (bomb.isActive()) { // Nếu bom vẫn hoạt động trên không
            explodeBomb(bomb.getX(), bomb.getY()); // Phát nổ
            bomb.explode();
        }
    }

    // Xóa bom đã nổ khỏi danh sách
    bombs.erase(
        std::remove_if(bombs.begin(), bombs.end(),
            [](const Bomb& b) { return !b.isActive(); }),
        bombs.end()
    );
}

void Tetris::spawnRandomBomb() {
    if (rand() % 100 == 0) {  // Tỉ lệ 1/500 mỗi frame
        bombs.emplace_back(rand() % COLS, 0, bombTexture);
    }
}

void Tetris::updateBombs() {
    for (auto& bomb : bombs) {
        if (bomb.isActive()) {
            bomb.update();
        }
            // Kiểm tra va chạm với khối hoặc đáy
            if (bomb.getY() >= ROWS) {
                    bomb.deactivate(); // Làm cho bom biến mất khi rơi hết màn hình
            } else if (bomb.getY() >= 0 && grid[bomb.getY()][bomb.getX()] != 0) {
                        explodeBomb(bomb.getX(), bomb.getY()); // Chỉ nổ nếu va chạm với khối
                            bomb.explode();
            }
    }

    bombs.erase(
    std::remove_if(bombs.begin(), bombs.end(),
        [](const Bomb& b) { return !b.isActive(); }),
    bombs.end()
    );

}

void Tetris::renderBombs() {
    for (const auto& bomb : bombs) {
        if (bomb.isActive()) {  // Chỉ vẽ bom đang active
            SDL_Rect bombRect = {
                bomb.getX() * BLOCK_SIZE,
                bomb.getY() * BLOCK_SIZE,
                BLOCK_SIZE,
                BLOCK_SIZE
            };
            SDL_RenderCopy(renderer, bombTexture, nullptr, &bombRect);
        }
    }
}

void Tetris::explodeBomb(int x, int y) {

    // Hiệu ứng nổ
    SDL_Rect explosionRect = {
        x * BLOCK_SIZE - BLOCK_SIZE,  // Center the explosion
        y * BLOCK_SIZE - BLOCK_SIZE,
        BLOCK_SIZE * 3,
        BLOCK_SIZE * 3
    };

    SDL_RenderCopy(renderer, explosionTexture, nullptr, &explosionRect);
    SDL_RenderPresent(renderer);

    SDL_Delay(200);  // 0.1 giây

    // Phá hủy khối (9 ô vuông 3x3)
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS) {
                grid[ny][nx] = 0;  // Xóa khối
            }
        }
    }

    if (isSoundOn) Mix_PlayChannel(-1, explosionSound, 0);
}

void Tetris::update() {
    spawnRandomBomb();  // Tạo bom ngẫu nhiên
    updateBombs();      // Cập nhật bom

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

// Chạy trò chơi
void Tetris::run() {
    renderBombs();  // Vẽ bom trước khi vẽ khối Tetris
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


