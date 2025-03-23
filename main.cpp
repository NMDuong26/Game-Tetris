#include "Tetris.h"

int main(int argc, char* argv[]){
    srand(time(0)); // Khởi tạo seed cho hàm rand()
    Tetris game;
    game.run();
    return 0;
}
