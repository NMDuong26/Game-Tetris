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
            renderExitConfirm(); //Hiển thị tùy chọn
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
            case SDLK_b: explodeAirBomb(false); break;  // Chỉ bom thường
            case SDLK_SPACE: explodeAirBomb(true); break;  // Chỉ bom xăng
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

       // Xử lý sự kiện click nút pause
        if (checkButtonClick(mouseX, mouseY, pauseButton)) {
            isPaused = !isPaused;

            // Xử lý âm thanh
            if (isSoundOn) {
                isPaused ? Mix_PauseMusic() : Mix_ResumeMusic();
            }
            render(); // Cập nhật giao diện
            // Hiển thị hộp thoại khi pause
            if (isPaused) {
                renderPauseDialog();
            }
        }

        if (checkButtonClick(mouseX, mouseY, soundButton)) {
            isSoundOn = !isSoundOn;
            if (isSoundOn && !isPaused) {
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

bool Tetris::checkButtonHover(int mouseX, int mouseY, SDL_Rect button) {
    return (mouseX >= button.x && mouseX <= button.x + button.w &&
            mouseY >= button.y && mouseY <= button.y + button.h);
}

bool Tetris::checkButtonClick(int mouseX, int mouseY, SDL_Rect button) {
    if (checkButtonHover(mouseX, mouseY, button)) {
        if (isSoundOn) {
            Mix_PlayChannel(-1, moveSound, 0); // Phát âm thanh khi click
        }
        return true;
    }
    return false;
}

