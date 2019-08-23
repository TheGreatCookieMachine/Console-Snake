#include <iostream>
#include <vector>
#include <array>
#include <chrono>

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

    Snake(std::array<int, 2> position, Direction direction) {
        this->body.push_back(position);
        this->direction = direction;
    }

    void process() {
        this->body.push_back(this->body.back());
        for (int i = this->body.size() - 1; i > 0; i--) {
            this->body[i] = this->body[i-1];
        }

        std::array<int, 2> destination = this->body[1];
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
        this->body[0] = destination;
    }
};

class Game {
public:
    Snake snake;
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

    Game(std::array<int, 2> snakePosition, Direction direction): snake(snakePosition, direction) {
        clearBoard();
    }

    void process() {
        this->snake.process();
    }

    void clearBoard() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                screen[i][j] = ' ';
            }
        }
    }

    void printGame() {
        std::system("cls");

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

        clearBoard();
    }
};

int main()
{
    Game game = Game({5, 5}, up);
    game.printGame();
    _sleep(1000);
    game.process();
    game.printGame();
    return 0;
}
