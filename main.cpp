#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <conio.h>
#include <windows.h>

// Doesn't count walls, actual screen size is 2 higher than inputted value
const unsigned SCREEN_HEIGHT = 10;
const unsigned SCREEN_WIDTH = 20;

// Used for printing the game to the screen
const char SNAKE_CHAR = 'S';
const char APPLE_CHAR = '@';

// In milliseconds
const unsigned FRAME_TIME = 500;

// Values returned by getch()
const char QUIT_KEY = '\x1b';
const char ENTER_KEY = '\r';
const char UP_KEY = 'w';
const char DOWN_KEY = 's';
const char LEFT_KEY = 'a';
const char RIGHT_KEY = 'd';

enum Direction {
    up, down, left, right
};

class Apple {
public:
    std::array<unsigned, 2> position;

    Apple(std::array<unsigned, 2> position) {
        this->position = position;
    }
};

class Snake {
public:
    std::vector<std::array<unsigned, 2>> body;
    Direction direction;
    Direction directionBuffer;
    bool ateApple;

    Snake(std::array<unsigned, 2> position, Direction direction) {
        this->body.push_back(position);
        this->direction = direction;
        this->directionBuffer = direction;
        this->ateApple = false;
    }

    std::array<unsigned, 2> nextHead() {
        std::array<unsigned, 2> destination = this->body[0];
        switch (this->directionBuffer) {
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

        for (unsigned i = this->body.size() - 1; i > 0; i--) {
            this->body[i] = this->body[i-1];
        }

        this->body[0] = this->nextHead();
    }
};

class Game {
public:
    std::array<unsigned, 2> startPosition;
    Direction startDirection;
    Snake snake;
    Apple apple;
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    unsigned score;
    bool gameOver;

    Game(std::array<unsigned, 2> snakePosition, Direction direction): snake(snakePosition, direction), apple({0, 0}) {
        this->startPosition = snakePosition;
        this->startDirection = direction;
        srand(std::chrono::system_clock::now().time_since_epoch().count());
        this->moveApple();
        this->clearBoard();
        this->score = 0;
        this->gameOver = false;
    }

    void moveApple() {
        std::vector<std::array<unsigned, 2>> places;
        for (unsigned i = 0; i < SCREEN_HEIGHT; i++) {
            for (unsigned j = 0; j < SCREEN_WIDTH; j++) {
                std::array<unsigned, 2> position = {i, j};
                if (std::find(this->snake.body.begin(), this->snake.body.end(), position) == this->snake.body.end()) {
                    places.push_back(position);
                }
            }
        }
        this->apple.position = places[rand() % places.size()] ;
    }

    void process() {
        std::array<unsigned, 2> nextHead = this->snake.nextHead();
        if (nextHead == this->apple.position) {
            this->snake.ateApple = true;
            this->moveApple();
            this->score++;
        } else if (nextHead[0] >= SCREEN_HEIGHT || nextHead[1] >= SCREEN_WIDTH ||
                   std::find(this->snake.body.begin(), this->snake.body.end(), nextHead) != this->snake.body.end()) {
            gameOver = true;
            return;
        }

        this->snake.process();
        this->snake.ateApple = false;
    }

    void resetGame() {
        this->snake.body.clear();
        this->snake.body.push_back(this->startPosition);
        this->snake.direction = this->startDirection;
        this->snake.directionBuffer = this->snake.direction;
        this->moveApple();
        this->score = 0;
        this->gameOver = false;
    }

    void clearBoard() {
        for (unsigned i = 0; i < SCREEN_HEIGHT; i++) {
            for (unsigned j = 0; j < SCREEN_WIDTH; j++) {
                this->screen[i][j] = ' ';
            }
        }
    }

    void printGame() {
        COORD cursorPosition = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

        this->screen[this->apple.position[0]][this->apple.position[1]] = APPLE_CHAR;
        for (std::array<unsigned, 2> position: this->snake.body) {
            this->screen[position[0]][position[1]] = SNAKE_CHAR;
        }

        printf("Score: %*u\n", 10, this->score);

        printf("+");
        for (unsigned i = 0; i < SCREEN_WIDTH; i++) {
            printf("-");
        }
        printf("+\n");

        for (unsigned i = 0; i < SCREEN_HEIGHT; i++) {
            printf("|");
            for (unsigned j = 0; j < SCREEN_WIDTH; j++) {
                printf("%c", this->screen[i][j]);
            }
            printf("|\n");
        }

        printf("+");
        for (unsigned i = 0; i < SCREEN_WIDTH; i++) {
            printf("-");
        }
        printf("+");

        this->clearBoard();
    }

    void mainloop() {
        restart:
        srand(std::chrono::system_clock::now().time_since_epoch().count());
        while (true) {
            auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            this->process();
            if (this->gameOver) {
                break;
            }
            this->printGame();
            auto currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

            do {
                if (kbhit()) {
                    switch(getch()) {
                    case QUIT_KEY:
                        return;
                    case UP_KEY:
                        if (this->snake.direction == left || this->snake.direction == right) {
                            this->snake.directionBuffer = up;
                        }
                        break;
                    case DOWN_KEY:
                        if (this->snake.direction == left || this->snake.direction == right) {
                            this->snake.directionBuffer = down;
                        }
                        break;
                    case LEFT_KEY:
                        if (this->snake.direction == up || this->snake.direction == down) {
                            this->snake.directionBuffer = left;
                        }
                        break;
                    case RIGHT_KEY:
                        if (this->snake.direction == up || this->snake.direction == down) {
                            this->snake.directionBuffer = right;
                        }
                        break;
                    }
                }
                currentTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            } while (currentTime.time_since_epoch().count() - startTime.time_since_epoch().count() < FRAME_TIME);
        }
        printf("\nGame over! Press enter to continue...");
        while (true) {
            if (kbhit()) {
                switch(getch()) {
                case QUIT_KEY:
                    return;
                case ENTER_KEY:
                    COORD cursorPosition = {0, SCREEN_HEIGHT + 3};
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
                    printf("                                     ");
                    this->resetGame();
                    goto restart;
                }
            }
        }
    }
};

int main()
{
    Game game = Game({10, 10}, up);
    game.mainloop();
    return 0;
}
