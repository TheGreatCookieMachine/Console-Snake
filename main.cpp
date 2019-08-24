#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>

// Doesn't count walls, actual screen size is 2 higher than inputted value
const int SCREEN_HEIGHT = 32;
const int SCREEN_WIDTH = 64;

// Used for printing the game to the screen
const char SNAKE_CHAR = 'S';
const char APPLE_CHAR = '@';

enum Direction {
    up, down, left, right
};

struct Apple {
public:
    std::array<int, 2> position;

    Apple(std::array<int, 2> position) {
        this->position = position;
    }
};

class Snake {
public:
    std::vector<std::array<int, 2>> body;
    Direction direction;
    bool ateApple;

    Snake(std::array<int, 2> position, Direction direction) {
        this->body.push_back(position);
        this->direction = direction;
        ateApple = false;
    }

    std::array<int, 2> nextHead() {
        std::array<int, 2> destination = this->body[0];
        switch (this->direction) {
        case up:
            destination[0]--;
            break;
        case down:
            destination[0]++;
            break;
        case left:
            destination[1]--;
            break;
        case right:
            destination[1]++;
            break;
        }
        return destination;
    }

    void process() {
        if (ateApple) {
            this->body.push_back(this->body.back());
        }

        for (int i = this->body.size() - 1; i > 0; i--) {
            this->body[i] = this->body[i-1];
        }

        this->body[0] = this->nextHead();
    }
};

class Game {
public:
    Snake snake;
    Apple apple;
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

    Game(std::array<int, 2> snakePosition, Direction direction): snake(snakePosition, direction), apple({5, 10}) {
        this->clearBoard();
    }

    void process() {
        if (this->snake.nextHead() == this->apple.position) {
            this->snake.ateApple = true;
        }

        this->snake.process();

        this->snake.ateApple = false;
    }

    void clearBoard() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                this->screen[i][j] = ' ';
            }
        }
    }

    void printGame() {
        std::system("cls");

        this->screen[this->apple.position[0]][this->apple.position[1]] = APPLE_CHAR;
        for (std::array<int, 2> position: this->snake.body) {
            this->screen[position[0]][position[1]] = SNAKE_CHAR;
        }

        printf("+");
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            printf("-");
        }
        printf("+\n");

        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            printf("|");
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                printf("%c", this->screen[i][j]);
            }
            printf("|\n");
        }

        printf("+");
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            printf("-");
        }
        printf("+");

        this->clearBoard();
    }
};

int main()
{
    Game game = Game({10, 10}, up);
    game.printGame();
    while (true) {
        _sleep(1000);
        game.process();
        game.printGame();
    }
    return 0;
}
