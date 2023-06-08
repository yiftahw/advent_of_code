#include <iostream>
#include <fstream>
#include <set>
#include <deque>
#include <string>
#include <cstdint>
#include <cmath>

using namespace std;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;

typedef struct
{
    direction_t direction;
    int32_t steps; // 1-255
} play_t;

typedef struct position_t
{
    int32_t x;
    int32_t y;
    // override operator < for std::set
    bool operator<(const struct position_t& other) const
    {
        if(x < other.x)
            return true;
        else if(x > other.x)
            return false;
        else // x == other.x
            return y < other.y;
    }
} position_t;

class rope_game
{
private:
    position_t head_position;
    position_t tail_position;
    std::set<position_t> tail_positions;
    void move_head(direction_t direction);
    void move_tail();
public:
    rope_game();
    ~rope_game() = default;
    void play(const play_t& play);
    int32_t get_num_tail_positions();
};

rope_game::rope_game() : tail_positions()
{
    head_position.x = 0;
    head_position.y = 0;
    tail_position.x = 0;
    tail_position.y = 0;
    tail_positions.insert(tail_position);
}

int32_t rope_game::get_num_tail_positions()
{
    return tail_positions.size();
}

void rope_game::move_head(direction_t direction)
{
    switch (direction)
    {
    case UP:
        head_position.y++;
        break;
    case DOWN:
        head_position.y--;
        break;
    case LEFT:
        head_position.x--;
        break;
    default: // case RIGHT:
        head_position.x++;
        break;
    }
}

void rope_game::move_tail()
{
    int32_t x_distance = std::abs(head_position.x - tail_position.x);
    int32_t y_distance = std::abs(head_position.y - tail_position.y);

    int32_t x_direction = (head_position.x > tail_position.x) ? 1 : -1;
    int32_t y_direction = (head_position.y > tail_position.y) ? 1 : -1;

    if((x_distance + y_distance <= 1) || (x_distance == 1 && y_distance == 1))
    {
        // head is close enough to tail, no need to move it
        return;
    }
    // size is 2 or greater
    if(x_distance == 2 && y_distance < 2)
    {
        tail_position.x += x_direction;
        if(y_distance > 0) // move diagonally
            tail_position.y += y_direction;
    }
    else if(y_distance == 2 && x_distance < 2)
    {
        tail_position.y += y_direction;
        if(x_distance > 0) // move diagonally
            tail_position.x += x_direction;
    }
    else
    {
        throw "Invalid state of the game!"; // should never happen
    }
    tail_positions.insert(tail_position); // add the new tail position to the set
}

void rope_game::play(const play_t& play)
{
    for(int32_t i = 0; i < play.steps; i++)
    {
        move_head(play.direction);
        move_tail();
    }
}

/*
@ parse a line from the input file and return a play_t
@ format should be: <direction> <steps>
@ throws an exception if the line is invalid
*/
play_t parse_line(const string& line)
{
    /*
    line should be in the format:
    <direction> <steps>
    where <direction> is one of U, D, L, R
    */
    direction_t direction;
    int32_t steps;
    switch (line[0])
    {
        case 'U':
            direction = UP;
            break;
        case 'D':
            direction = DOWN;
            break;
        case 'L':
            direction = LEFT;
            break;
        case 'R':
            direction = RIGHT;
            break;
        default:
            throw "Invalid direction";
    }
    steps = stoi(line.substr(2)); // skip the direction and the space
    play_t result;
    result.direction = direction;
    result.steps = steps;
    return result;
}


int main()
{
    rope_game game;
    ifstream input_file("input.txt");
    string line;
    while(getline(input_file, line))
    {
        play_t play = parse_line(line);
        game.play(play);
    }
    cout << "part a: number of tail positions: " << game.get_num_tail_positions() << endl;
}
