#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <conio.h>
#include <windows.h>

#define now_milliseconds() std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
#define since_epoch_count(time) time.time_since_epoch().count()

// Doesn't count walls, actual screen size is 2 higher than inputted value
const unsigned SCREEN_HEIGHT = 10;
const unsigned SCREEN_WIDTH = 20;

// Used for printing the game to the screen
const char SNAKE_HEAD_CHAR = 'S';
const char SNAKE_CHAR = 's';
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

// Console attributes
HANDLE CONSOLE_OUTPUT;
COORD STARTING_CURSOR_COORD;

void showCursor(bool status) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(CONSOLE_OUTPUT, &cursorInfo);
    cursorInfo.bVisible = status;
    SetConsoleCursorInfo(CONSOLE_OUTPUT, &cursorInfo);
}

enum Direction {
    up, down, left, right
};

class Coordinate {
public:
    unsigned x;
    unsigned y;

    Coordinate() {
        this->x = 0;
        this->y = 0;
    }

    Coordinate(unsigned x, unsigned y) {
        this->x = x;
        this->y = y;
    }

    bool operator ==(Coordinate obj) {
        return (this->x == obj.x) && (this->y == obj.y);
    }
};

class Apple {
public:
    Coordinate position;

    Apple() {}

    Apple(Coordinate position) {
        this->position = position;
    }
};

class Snake {
public:
    std::vector<Coordinate> body;
    Direction direction;
    Direction directionBuffer;
    bool ateApple;

    Snake(Coordinate position, Direction direction) {
        this->body.push_back(position);
        this->direction = direction;
        this->directionBuffer = direction;
        this->ateApple = false;
    }

    Coordinate nextHead() {
        Coordinate destination = this->body[0];
        switch (this->directionBuffer) {
        case up:
            destination.x--;
            break;
        case down:
            destination.x++;
            break;
        case left:
            destination.y--;
            break;
        case right:
            destination.y++;
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
    Coordinate startPosition;
    Direction startDirection;
    Snake snake;
    Apple apple;
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    unsigned score;
    bool gameOver;

    Game(Coordinate snakePosition, Direction direction): snake(snakePosition, direction) {
        this->startPosition = snakePosition;
        this->startDirection = direction;
        srand(since_epoch_count(now_milliseconds()));
        this->moveApple();
        this->clearBoard();
        this->score = 0;
        this->gameOver = false;
    }

    void moveApple() {
        std::vector<Coordinate> places;
        for (unsigned i = 0; i < SCREEN_HEIGHT; i++) {
            for (unsigned j = 0; j < SCREEN_WIDTH; j++) {
                Coordinate position(i, j);
                if (std::find(this->snake.body.begin(), this->snake.body.end(), position) == this->snake.body.end()) {
                    places.push_back(position);
                }
            }
        }
        this->apple.position = places[rand() % places.size()];
    }

    void process() {
        Coordinate nextHead = this->snake.nextHead();
        if (nextHead == this->apple.position) {
            this->snake.ateApple = true;
            this->moveApple();
            this->score++;
        }

        auto snakeCollision = std::find(this->snake.body.begin(), this->snake.body.end(), nextHead);
        if (nextHead.x >= SCREEN_HEIGHT || nextHead.y >= SCREEN_WIDTH ||
            (snakeCollision != this->snake.body.end() && snakeCollision != this->snake.body.end() - 1)) {
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
        SetConsoleCursorPosition(CONSOLE_OUTPUT, STARTING_CURSOR_COORD);

        this->screen[this->apple.position.x][this->apple.position.y] = APPLE_CHAR;
        for (Coordinate position: this->snake.body) {
            this->screen[position.x][position.y] = SNAKE_CHAR;
        }
        this->screen[this->snake.body[0].x][this->snake.body[0].y] = SNAKE_HEAD_CHAR;

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
        srand(since_epoch_count(now_milliseconds()));
        while (true) {
            auto startTime = now_milliseconds();
            this->process();
            if (this->gameOver) {
                break;
            }
            this->printGame();
            auto currentTime = now_milliseconds();

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
                currentTime = now_milliseconds();
            } while (since_epoch_count(currentTime) - since_epoch_count(startTime) < FRAME_TIME);
        }
        printf("\nGame over! Press enter to continue...");
        while (true) {
            if (kbhit()) {
                switch(getch()) {
                case QUIT_KEY:
                    return;
                case ENTER_KEY:
                    COORD cursorPosition = {0, STARTING_CURSOR_COORD.Y + SCREEN_HEIGHT + 3};
                    SetConsoleCursorPosition(CONSOLE_OUTPUT, cursorPosition);
                    printf("                                     ");
                    this->resetGame();
                    goto restart;
                }
            }
        }
    }
};

int main() {
    // Console attributes
    CONSOLE_OUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
    if (GetConsoleScreenBufferInfo(CONSOLE_OUTPUT, &consoleBufferInfo)) {
        STARTING_CURSOR_COORD = consoleBufferInfo.dwCursorPosition;
    } else {
        printf("Could not get screen buffer info.");
        return 1;
    }

    showCursor(false);

    Game game = Game({10, 10}, up);
    game.mainloop();

    showCursor(true);

    return 0;
}
