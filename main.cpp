#include <iostream>
#include <vector>
#include <array>

enum Direction {
    up, down, left, right
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

int main()
{
    Snake snake = Snake({0, 0}, up);
    return 0;
}
