#include <iostream>
#include <fstream>
#include <set>
#include <deque>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>

/*
part 1: the rope has only 2 links - head and tail
part 2: the rope has 10 links - the tail is the 10th link
*/

using namespace std;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;

ostream& operator<<(ostream& os, const direction_t& direction)
{
    switch (direction)
    {
    case UP:
        os << "UP";
        break;
    case DOWN:
        os << "DOWN";
        break;
    case LEFT:
        os << "LEFT";
        break;
    default: // case RIGHT:
        os << "RIGHT";
        break;
    }
    return os;
}

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
    vector<position_t> knots_positions;
    set<position_t> tail_positions;
    void move_head(direction_t direction);
    void move_tail();
public:
    rope_game(uint32_t num_knots);
    ~rope_game() = default;
    void play(const play_t& play);
    int32_t get_num_tail_positions();


};

class rope_error : public std::exception
{
private:
    string msg;
public:
    rope_error(const char* msg) : msg(msg) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class invalid_direction : public rope_error
{
public:
    invalid_direction(char direction) : rope_error(("Invalid direction: " + std::string(1, direction)).c_str()) {}
};

class invalid_num_knots : public rope_error
{
    public:
    invalid_num_knots() : rope_error("Invalid number of knots!") {}
};

class invalid_game_state : public rope_error
{
    public:
    invalid_game_state() : rope_error("Invalid game state - manhattan distance is greater than 3! (internal error)") {}
};


rope_game::rope_game(uint32_t num_knots) : tail_positions()
{
    if (num_knots < 2)
        throw invalid_num_knots();
    position_t knot_position;
    for(uint32_t i = 0; i < num_knots; i++)
    {
        knot_position.x = 0;
        knot_position.y = 0;
        knots_positions.push_back(knot_position);
    }
    tail_positions.insert(knot_position); // add the initial tail position to the set
}

int32_t rope_game::get_num_tail_positions()
{
    return tail_positions.size();
}

void rope_game::move_head(direction_t direction)
{
    position_t& head_position = knots_positions[0];
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
    position_t* lead_knot = NULL;
    position_t* curr_knot = NULL;

    // need to move knots from the second knot to the last one
    for(uint32_t i = 1; i < knots_positions.size(); i++)
    {
        lead_knot = &knots_positions[i-1];
        curr_knot = &knots_positions[i];

        int32_t x_distance = abs(lead_knot->x - curr_knot->x);
        int32_t y_distance = abs(lead_knot->y - curr_knot->y);

        int32_t x_direction = (lead_knot->x > curr_knot->x) ? 1 : -1;
        int32_t y_direction = (lead_knot->y > curr_knot->y) ? 1 : -1;

        if((x_distance + y_distance <= 1) || (x_distance == 1 && y_distance == 1))
        {
            // the two knots are adjacent or diagonal, no need to move
            continue;
        }

        bool moved = false;
        if ((x_distance == 2) || (x_distance == 1 && y_distance == 2))
        {
            curr_knot->x += x_direction;
            moved = true;
        }
        if ((y_distance == 2) || (y_distance == 1 && x_distance == 2))
        {
            curr_knot->y += y_direction;
            moved = true;
        }
        if (!moved)
        {
            throw invalid_game_state(); // should never happen
        }
    }
    // the last curr_knot is the tail
    tail_positions.insert(*curr_knot);
}

void rope_game::play(const play_t& play)
{
    //cout << "playing " << play.steps << " in direction " << play.direction << endl;
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
            throw invalid_direction(line[0]);
    }
    steps = stoi(line.substr(2)); // skip the direction and the space
    play_t result;
    result.direction = direction;
    result.steps = steps;
    return result;
}

void part_a()
{
    rope_game game(2);
    ifstream input_file("input.txt");
    string line;
    while(getline(input_file, line))
    {
        play_t play = parse_line(line);
        game.play(play);
    }
    cout << "part a: number of tail positions: " << game.get_num_tail_positions() << endl;
}

void part_b()
{
    rope_game game(10);
    ifstream input_file("input.txt");
    string line;
    while(getline(input_file, line))
    {
        play_t play = parse_line(line);
        game.play(play);
    }
    cout << "part b: number of tail positions: " << game.get_num_tail_positions() << endl;
}


int main()
{
    try
    {
        part_a();
        part_b();
    }
    catch(const exception& e)
    {
        cerr << e.what() << endl;
    }
    return 0;
}
