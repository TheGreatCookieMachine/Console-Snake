#include <iostream>
#include <vector>
#include <array>

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
};

class Game {
public:
    Snake snake;
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

    Game(std::array<int, 2> snakePosition, Direction direction): snake(snakePosition, direction) {
        clearBoard();
    }

    void clearBoard() {
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                screen[i][j] = ' ';
            }
        }
    }

    void printGame() {
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
    return 0;
}
