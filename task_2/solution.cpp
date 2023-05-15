
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

const char OPPONENT_ROCK     = 'A';
const char OPPONENT_PAPER    = 'B';
const char OPPONENT_SCISSORS = 'C';

const char PLAYER_ROCK      = 'X';
const char PLAYER_PAPER     = 'Y';
const char PLAYER_SCISSORS  = 'Z';

const char SHOULD_LOSE = 'X';
const char SHOULD_DRAW = 'Y';
const char SHOULD_WIN  = 'Z';

typedef enum
{
    ROCK = 0,
    PAPER,
    SCISSORS,
    SHAPE_LAST_INDEX = SCISSORS
} shape_e;

#define SHAPES_SIZE (SHAPE_LAST_INDEX + 1)

typedef enum
{
    ROCK_BONUS = 1,
    PAPER_BONUS,
    SCISSORS_BONUS
} shape_bonus_e;

typedef enum
{
    LOSS = 0,
    DRAW = 3,
    WIN = 6
} result_score_e;

const uint32_t result_map[SHAPES_SIZE][SHAPES_SIZE] =
{
    {
        {DRAW}, //OPPONENT ROCK
        {LOSS}, //OPPONENT PAPER
        {WIN}, //OPPONENT SCISSORS
    }, //PLAYER ROCK
    {
        {WIN}, //OPPONENT ROCK
        {DRAW}, //OPPONENT PAPER
        {LOSS}, //OPPONENT SCISSORS
    }, //PLAYER PAPER
    {
        {LOSS}, //OPPONENT ROCK
        {WIN}, //OPPONENT PAPER
        {DRAW}, //OPPONENT SCISSORS
    }, //PLAYER SCISSORS
};

const uint32_t bonus_map[SHAPES_SIZE] = 
{
    ROCK_BONUS, PAPER_BONUS, SCISSORS_BONUS
};

int32_t calculate_round_score(shape_e player_move, shape_e opponent_move)
{
    uint32_t score = 0;
    score += bonus_map[player_move];
    score += result_map[player_move][opponent_move];
    //cout << "player move " << player_move << " opponent move " << opponent_move << " score " << score << endl;
    return score;
}

typedef struct
{
    shape_e player_move;
    shape_e opponent_move;
} round_shapes_s;

round_shapes_s get_round_shapes_from_line(string line, bool is_part_two = false)
{
    round_shapes_s result;
    char opponent_move = line[0], second_col = line[2];

    if(opponent_move == OPPONENT_ROCK)
        result.opponent_move = ROCK;
    else if(opponent_move == OPPONENT_PAPER)
        result.opponent_move = PAPER;
    else if(opponent_move == OPPONENT_SCISSORS)
        result.opponent_move = SCISSORS;

    if(!is_part_two)
    {
        if(second_col == PLAYER_ROCK)
            result.player_move = ROCK;
        else if(second_col == PLAYER_PAPER)
            result.player_move = PAPER;
        else if(second_col == PLAYER_SCISSORS)
            result.player_move = SCISSORS;
    }
    else
    {
        /* X = SHOULD WIN, Y = SHOULD DRAW, Z SHOULD LOSE */
        result_score_e round_result;
        if(second_col == SHOULD_LOSE)
            round_result = LOSS;
        else if(second_col == SHOULD_DRAW)
            round_result = DRAW;
        else // should win
            round_result = WIN;

        bool found = false;
        for(uint32_t i = 0; i < SHAPES_SIZE; i++)
        {
            if(result_map[i][result.opponent_move] == round_result)
            {
                result.player_move = static_cast<shape_e>(i);
                found = true;
                break;
            }
        }
        if(!found)
            throw found;
    }

    return result;
}

vector<round_shapes_s> load_data(string path, bool is_part_two = false)
{
    vector<round_shapes_s> result;
    fstream data_file;
    string line;
    data_file.open(path, ios::in);
    round_shapes_s temp = {ROCK, ROCK};
    while(getline(data_file, line))
    {
        //cout << "line: " << line << endl;
        temp = get_round_shapes_from_line(line, is_part_two);
        result.push_back(temp);
        //cout << "player move " << temp.player_move << " opponent move " << temp.opponent_move << endl;
    }
    return result;
}

int main()
{
    uint32_t score = 0;
    bool is_part_two = true;
    vector<round_shapes_s> rounds = load_data("input.txt", is_part_two);
    uint32_t length = rounds.size();
    for(uint32_t i = 0; i < length; i++)
    {
        score += calculate_round_score(rounds.at(i).player_move, rounds.at(i).opponent_move);
    }
    cout << "score: " << score << endl;
}
