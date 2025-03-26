#include "Tetris.h"
#include "bomb.h"

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
        int x = block.x + dx;
        int y = block.y + dy;

        // Kiểm tra va chạm với tường hoặc khối đã đặt (không bao gồm tuyết)
        if (x < 0 || x >= COLS || y >= ROWS) {
            return true;
        }

        // Chỉ kiểm tra va chạm với grid nếu y trong phạm vi hợp lệ
        // và ô đó không phải là hiệu ứng tuyết
        if (y >= 0 && grid[y][x] != 0) {
            return true;
        }
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
            if (!iceEffectActive) { // Chỉ tăng tốc khi KHÔNG có hiệu ứng băng
                speed = max(100, speed - 20);
            }
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
    if (rand() % 100 == 0) {  // Tỉ lệ 1/100 mỗi frame
        BombType type = (rand() % 10 < 8) ?  NORMAL_BOMB : ICE_BOMB;  // 80% NORMAL, 20% ICE
        SDL_Texture* tex = (type == ICE_BOMB) ? iceTexture : bombTexture;
        bombs.emplace_back(rand() % COLS, 0, tex, type);
    }
}


void Tetris::updateBombs() {

    for (auto& bomb : bombs) {

        if (bomb.isActive()) {
           bomb.update(); // Truyền tốc độ vào hàm update của bom
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
        if (bomb.isActive()) {
            SDL_Rect bombRect = {
                bomb.getX() * BLOCK_SIZE,
                bomb.getY() * BLOCK_SIZE,
                BLOCK_SIZE,
                BLOCK_SIZE
            };
            //Chọn texture đúng theo loại bom
            SDL_Texture* tex = (bomb.getType() == ICE_BOMB) ? iceTexture : bombTexture;
            SDL_RenderCopy(renderer, tex, nullptr, &bombRect);
        }
    }
}

void Tetris::explodeBomb(int x, int y) {
    BombType bombType = NORMAL_BOMB; // Giá trị mặc định

    // Tìm quả bom tại (x, y) trong danh sách bombs
    for (Bomb& bomb : bombs) {
        if (bomb.getX() == x && bomb.getY() == y) {
            bombType = bomb.getType();
            break;
        }
    }

     if (bombType == ICE_BOMB) {
        activateIceEffect(3000);
        if (isSoundOn) Mix_PlayChannel(-1, iceSound, 0);

        // Thêm kiểm tra và xử lý khối đang rơi
        bool needDrop = false;
        for (auto& block : currentPiece) {
            if (block.y + 1 < ROWS && grid[block.y + 1][block.x] != 0) {
                needDrop = true;
                break;
            }
        }

        if (needDrop) {
            while (!collides(0, 1)) {
                for (auto& block : currentPiece) block.y++;
            }
        }
     }
    else{
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
}

void Tetris::initSnow() {
    snowflakes.clear();
    const int FLAKE_COUNT = 100; // 100 bông tuyết
    const float MIN_SPEED = 20.0f; // Tăng tốc độ tối thiểu
    const float MAX_SPEED = 30.0f; // Tăng tốc độ tối đa

    for (int i = 0; i < FLAKE_COUNT; i++) {
        snowflakes.push_back({
            static_cast<float>(rand() % (COLS * BLOCK_SIZE)), // Vị trí x ngẫu nhiên
            static_cast<float>(-(rand() % 50 + 20)), // Bắt đầu cao hơn (từ -20 đến -70)
            MIN_SPEED + static_cast<float>(rand() % 100) * (MAX_SPEED - MIN_SPEED) / 100.0f
        });
    }
}

void Tetris::activateIceEffect(Uint32 duration) {
    if (!iceEffectActive) {
        originalSpeed = speed;
    }

    iceEffectActive = true;
    iceEffectEndTime = SDL_GetTicks() + duration;
    snowEndTime = iceEffectEndTime + 3000;
    speed = originalSpeed * 2; // Giảm tốc game 50%

    isSnowing = true;
    initSnow();
}
void Tetris::updateSnow() {
    if (!isSnowing) return;

    bool allFlakesGone = true;
    float speedModifier = iceEffectActive ? 0.5f : 1.0f; // Giảm tốc 50% khi hiệu ứng băng còn hoạt động

    for (auto& flake : snowflakes) {
        if (flake.y <= ROWS * BLOCK_SIZE) {
            flake.y += flake.speed * speedModifier; // Áp dụng tốc độ theo hiệu ứng
            allFlakesGone = false;
        }
    }

    if (allFlakesGone && SDL_GetTicks() > snowEndTime) {
        isSnowing = false;
    }
}

void Tetris::renderSnow() {
    if (!isSnowing) return;

    SDL_Rect dest;
    for (const auto& flake : snowflakes) {
        dest = { static_cast<int>(flake.x), static_cast<int>(flake.y), 20, 20 };
        SDL_RenderCopy(renderer, snowTexture, nullptr, &dest);
    }
}

void Tetris::update() {
    spawnRandomBomb();  // Tạo bom ngẫu nhiên
    updateBombs();      // Cập nhật bom
    updateSnow();       // Cập nhật vị trí tuyết

    // Kiểm tra hiệu ứng băng
   if (iceEffectActive && SDL_GetTicks() > iceEffectEndTime) {
        bool snowFinished = true;
        for (auto& flake : snowflakes) {
            if (flake.y <= ROWS * BLOCK_SIZE) {
                snowFinished = false;
                break;
            }
        }
        if (snowFinished) {
            speed = originalSpeed; // Chỉ khôi phục khi tuyết đã rơi hết
            iceEffectActive = false;
        }
    }


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
        saveHighScore();
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

