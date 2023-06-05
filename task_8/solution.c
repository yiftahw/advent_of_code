#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

/*
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

for each iteration, hold a max tree height already seen, and keep a boolean histogram for visible trees.
*/

#define BOARD_SIZE 99

void print_board(int32_t board[BOARD_SIZE][BOARD_SIZE])
{
    for(int32_t row = 0; row < BOARD_SIZE; row++)
    {
        for(int32_t col = 0; col < BOARD_SIZE; col++)
        {
            printf("%u", board[row][col]);
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

int main()
{
    int32_t trees_heights[BOARD_SIZE][BOARD_SIZE] = {0}; // 0 by default
    bool is_tree_visible[BOARD_SIZE][BOARD_SIZE] = {0}; // false by default
    char buffer[BOARD_SIZE + 1]; // +1 for the null terminator
    char* file_name = "input.txt";
    FILE* input = fopen(file_name, "r");
    if(!input)
    {
        fprintf(stderr, "%s not found!\n", file_name);
        return -1;
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
            return -1;
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
    fclose(input);
    assert(row == BOARD_SIZE);
    print_board(trees_heights);
    update_hist_horizontal(trees_heights, is_tree_visible, true);
    update_hist_horizontal(trees_heights, is_tree_visible, false);
    update_hist_vertical(trees_heights, is_tree_visible, true);
    update_hist_vertical(trees_heights, is_tree_visible, false);
    printf("\n");
    print_board_bool(is_tree_visible);
    int32_t visible_trees = count_visible_trees(is_tree_visible);
    printf("Visible trees: %u\n", visible_trees);

    return 0;
}
