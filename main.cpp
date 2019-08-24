#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <conio.h>

// Doesn't count walls, actual screen size is 2 higher than inputted value
const int SCREEN_HEIGHT = 10;
const int SCREEN_WIDTH = 20;

// Used for printing the game to the screen
const char SNAKE_CHAR = 'S';
const char APPLE_CHAR = '@';

// In milliseconds
const int FRAME_TIME = 500;

enum Direction {
    up, down, left, right
};

class Apple {
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
    Direction directionBuffer;
    bool ateApple;

    Snake(std::array<int, 2> position, Direction direction) {
        this->body.push_back(position);
        this->direction = direction;
        this->directionBuffer = direction;
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
        this->direction = this->directionBuffer;
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

    void moveApple() {
        std::vector<std::array<int, 2>> places;
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                std::array<int, 2> position = {i, j};
                if (std::find(this->snake.body.begin(), this->snake.body.end(), position) == this->snake.body.end()) {
                    places.push_back(position);
                }
            }
        }
        this->apple.position = places[rand() % places.size()] ;
    }

    void process() {
        if (this->snake.nextHead() == this->apple.position) {
            this->snake.ateApple = true;
            this->moveApple();
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

    void mainloop() {
        while (true) {
            auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            this->process();
            this->printGame();
            auto currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            do {
                if (kbhit()) {
                    switch(getch()) {
                    case '\x1b':
                        return;
                    case 'w':
                        if (this->snake.direction == left || this->snake.direction == right) {
                            this->snake.directionBuffer = up;
                        }
                        break;
                    case 's':
                        if (this->snake.direction == left || this->snake.direction == right) {
                            this->snake.directionBuffer = down;
                        }
                        break;
                    case 'a':
                        if (this->snake.direction == up || this->snake.direction == down) {
                            this->snake.directionBuffer = left;
                        }
                        break;
                    case 'd':
                        if (this->snake.direction == up || this->snake.direction == down) {
                            this->snake.directionBuffer = right;
                        }
                        break;
                    }
                }
                currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            } while (currentTime.time_since_epoch().count() - startTime.time_since_epoch().count() < FRAME_TIME);
        }
    }
};

int main()
{
    Game game = Game({10, 10}, up);
    game.mainloop();
    return 0;
}
