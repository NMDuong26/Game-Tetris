#include "Tetris.h"

void Tetris::handleMenuEvents(SDL_Event& event) {
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

void Tetris::handleGameEvents(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT: movePiece(-1); break;
            case SDLK_RIGHT: movePiece(1); break;
            case SDLK_DOWN: dropPiece(); break;
            case SDLK_UP: rotatePiece(); break;
            case SDLK_b: explodeAirBomb(); break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
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

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_b) {
        if (!bombs.empty()) {
            bombs[0].explode();
        }
    }
}

bool Tetris::checkButtonClick(int mouseX, int mouseY, SDL_Rect button) {
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}
