#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

/*
part a:
for a tree to be visible it needs to meet a criteria
- either an edge tree
- tallest [and only one] between it and some edge
- for example, 1355 - from left to right only the last 5 is not visible - the previous 5 hides it

strategy:
scan the field 4 times - each time from each direction:
- left to right row by row
- right to left row by row
- top down column by column
- bottom up column by column

- time complexity for a square matrix of size N is O(4*N) = O(N) (where row and column sizes are sqrt(N))

for each iteration, hold a max tree height already seen, and keep a boolean histogram for visible trees.

part b:
need to find the scenic score of each tree
- the scenic score of a tree is the multiplication of the number of visible trees from it in each direction
- stop counting visible trees in some direction when a tree is taller than or the same height as the current one

strategy:
- for each tree, scan the viewing distance in each direction and count the number of visible trees
- for each tree, we scan at most it's row and column

- time complexity for a square matrix of size N is O(N * 2 * sqrt(N)) = O(N^1.5) (where row and column sizes are sqrt(N)
*/

#define BOARD_SIZE 99
#define TREE_HEIGHT_OPTIONS 10

bool load_data(char* file_name, int32_t trees_heights[BOARD_SIZE][BOARD_SIZE])
{
    char buffer[BOARD_SIZE + 1]; // +1 for the null terminator
    FILE* input = fopen(file_name, "r");
    if(!input)
    {
        fprintf(stderr, "%s not found!\n", file_name);
        return false;
    }
    int32_t row = 0;
    while(true)
    {
        int32_t bytes_read = fread(buffer, 1, BOARD_SIZE + 1, input);
        if(feof(input))
        {
            break;
        }
        if(bytes_read != BOARD_SIZE + 1)
        {
            fprintf(stderr, "Error reading %s!\n", file_name);
            return false;
        }
        buffer[strcspn(buffer, "\r\n")] = 0; // remove the newline
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            //char c = ;
            int32_t tree_height = buffer[col] - '0';
            assert(tree_height >= 0 && tree_height <= 9);
            trees_heights[row][col] = tree_height;
        }
        row++;
    }
    assert(row == BOARD_SIZE);
    fclose(input);
    return true;
}

void print_board(int32_t board[BOARD_SIZE][BOARD_SIZE])
{
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            printf("%u ", board[row][col]);
        }
        printf("\n");
    }
}

void print_board_bool(bool board[BOARD_SIZE][BOARD_SIZE])
{
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            printf("%d", board[row][col]);
        }
        printf("\n");
    }
}

void update_hist_horizontal(int32_t board[BOARD_SIZE][BOARD_SIZE], bool hist[BOARD_SIZE][BOARD_SIZE], bool is_left_to_right)
{
    int32_t max_height, col;
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        max_height = -1;
        for(int32_t col_iter = 0; col_iter < BOARD_SIZE; col_iter++)
        {
            col = is_left_to_right ? col_iter : BOARD_SIZE - 1 - col_iter;
            if(board[row][col] > max_height)
            {
                max_height = board[row][col];
                hist[row][col] = true;
            }
        }
    }
}

void update_hist_vertical(int32_t board[BOARD_SIZE][BOARD_SIZE], bool hist[BOARD_SIZE][BOARD_SIZE], bool is_top_down)
{
    /*
    to benefit from locality of reference, we will scan the board row by row, but update the histogram column by column
    */
    int32_t row, max_heights[BOARD_SIZE];
    for(int32_t col = 0; col < BOARD_SIZE; col++)
    {
        max_heights[col] = -1;
    }
    for (int32_t row_iter = 0; row_iter < BOARD_SIZE; row_iter++)
    {
        row = is_top_down ? row_iter : BOARD_SIZE - 1 - row_iter;
        for (int32_t col = 0; col < BOARD_SIZE; col++)
        {
            if (board[row][col] > max_heights[col])
            {
                max_heights[col] = board[row][col];
                hist[row][col] = true;
            }
        }
    }
}

int32_t count_visible_trees(bool hist[BOARD_SIZE][BOARD_SIZE])
{
    int32_t count = 0;
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            if(hist[row][col])
            {
                count++;
            }
        }
    }
    return count;
}

void part_a(int32_t trees_heights[BOARD_SIZE][BOARD_SIZE], bool is_tree_visible[BOARD_SIZE][BOARD_SIZE])
{
    printf("part a:\n");
    update_hist_horizontal(trees_heights, is_tree_visible, true);
    update_hist_horizontal(trees_heights, is_tree_visible, false);
    update_hist_vertical(trees_heights, is_tree_visible, true);
    update_hist_vertical(trees_heights, is_tree_visible, false);
    int32_t visible_trees = count_visible_trees(is_tree_visible);
    printf("Visible trees: %u\n", visible_trees);
}

void update_tree_visible_trees_horizontal(int32_t trees_heights[BOARD_SIZE][BOARD_SIZE], int32_t trees_scores[BOARD_SIZE][BOARD_SIZE], bool is_left_to_right)
{
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col_iter = 0; col_iter < BOARD_SIZE; col_iter++)
        {
            int32_t col = (is_left_to_right) ? col_iter : (BOARD_SIZE - 1 - col_iter);
            int32_t next_col = (is_left_to_right) ? (col + 1) : (col - 1); 
            int32_t col_end = (is_left_to_right) ? (BOARD_SIZE) : -1;
            int32_t num_trees = 0;
            while(next_col != col_end)
            {
                num_trees++;
                if(trees_heights[row][next_col] >= trees_heights[row][col])
                {
                    break;
                }
                next_col += (is_left_to_right) ? 1 : -1; 
            }
            trees_scores[row][col] *= num_trees;
        }
    }
}

void update_tree_visible_trees_vertical(int32_t trees_heights[BOARD_SIZE][BOARD_SIZE], int32_t trees_scores[BOARD_SIZE][BOARD_SIZE], bool is_top_down)
{
    for(int32_t col = 0; col < BOARD_SIZE; col++)
    {
        for(int32_t row_iter = 0; row_iter < BOARD_SIZE; row_iter++)
        {
            int32_t row = (is_top_down) ? row_iter : (BOARD_SIZE - 1 - row_iter);
            int32_t next_row = (is_top_down) ? (row + 1) : (row - 1); 
            int32_t row_end = (is_top_down) ? (BOARD_SIZE) : -1;
            int32_t num_trees = 0;
            int32_t max_tree = -1;
            while(next_row != row_end)
            {
                num_trees++;
                if(trees_heights[next_row][col] >= trees_heights[row][col])
                {
                    break;
                }
                next_row += (is_top_down) ? 1 : -1;
            }
            trees_scores[row][col] *= num_trees;
        }
    }
}

int32_t find_max_score_tree(int32_t trees_scores[BOARD_SIZE][BOARD_SIZE])
{
    int32_t max_score = -1;
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            if(trees_scores[row][col] > max_score)
            {
                max_score = trees_scores[row][col];
            }
        }
    }
    return max_score;
}


void part_b(int32_t trees_heights[BOARD_SIZE][BOARD_SIZE], int32_t trees_scores[BOARD_SIZE][BOARD_SIZE])
{
    printf("part b:\n");
    update_tree_visible_trees_horizontal(trees_heights, trees_scores, true);
    update_tree_visible_trees_horizontal(trees_heights, trees_scores, false);
    update_tree_visible_trees_vertical(trees_heights, trees_scores, true);
    update_tree_visible_trees_vertical(trees_heights, trees_scores, false);
    int32_t max_score = find_max_score_tree(trees_scores);
    printf("Max score: %u\n", max_score);
}

int main()
{
    bool is_tree_visible[BOARD_SIZE][BOARD_SIZE] = {0};
    int32_t trees_heights[BOARD_SIZE][BOARD_SIZE] = {0};
    int32_t trees_scores[BOARD_SIZE][BOARD_SIZE];

    for(int32_t i = 0; i<BOARD_SIZE;i++)
    {
        for(int32_t j = 0; j<BOARD_SIZE;j++)
        {
            trees_scores[i][j] = 1; // multiplicative identity
        }
    }

    char* file_name = "input.txt";
    load_data(file_name, trees_heights);
    part_a(trees_heights, is_tree_visible);
    part_b(trees_heights, trees_scores);
    return 0;
}
