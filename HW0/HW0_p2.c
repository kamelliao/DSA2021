#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_SIZE 5
#define BLACK 'o'
#define WHITE '_'
#define EMPTY ' '

typedef struct board
{
    char body[MAX_SIZE][MAX_SIZE];
    int num_row;
    int num_col;
} Board;

typedef struct clue
{
    int row[MAX_SIZE][MAX_SIZE];
    int col[MAX_SIZE][MAX_SIZE];
} Clue;

Board* new_board(int num_row, int num_col);
Clue* set_clue(int num_row, int num_col);

bool is_fiiled(Board* board);
bool is_valid(Board* board, Clue* clue);

int* pattern_row(Board* board, int row);
int* pattern_col(Board* board, int col);

char* next_empty_cell(Board* board);
void print_board(Board* board);

bool Recur(Board* board, Clue* clue);

int main(void)
{
    // inputs
    int num_row, num_col;
    scanf("%d%d", &num_row, &num_col);

    // initialize board
    Board* board = new_board(num_row, num_col);

    // clues
    Clue* clue = set_clue(num_row, num_col);

    // start algorithm
    Recur(board, clue);

    // release memory
    free(board);
    free(clue);

    return 0;
}

Board* new_board(int num_row, int num_col)
{
    Board* board = (Board*)calloc(1, sizeof(Board));
    board->num_row = num_row;
    board->num_col = num_col;

    for (int i = 0; i < num_row; i++)
        for (int j = 0; j < num_col; j++)
            board->body[i][j] = EMPTY;

    return board;
}

Clue* set_clue(int num_row, int num_col)
{
    Clue* clue = (Clue*)calloc(1, sizeof(Clue));

    // input row clues
    for (int i = 0; i < num_row; i++)
    {
        int num_clue;
        scanf("%d", &num_clue);
        for (int j = 0; j < num_clue; j++)
            scanf("%d", &clue->row[i][j]);
    }

    // input col clues
    for (int i = 0; i < num_col; i++)
    {
        int num_clue;
        scanf("%d", &num_clue);
        for (int j = 0; j < num_clue; j++)
            scanf("%d", &clue->col[i][j]);
    }

    return clue;
}

bool is_fiiled(Board* board)
{
    for (int i = 0; i < board->num_row; i++)
        for (int j = 0; j < board->num_col; j++)
            if (board->body[i][j] == EMPTY)
                return false;

    return true;
}

int* pattern_row(Board* board, int row)
{
    // scan through specified row
    int* pattern = (int*)calloc(MAX_SIZE, sizeof(int));
    int num_black = 0, k = 0;
    for (int j = 0; j < board->num_col; j++)
    {
        if (board->body[row][j] == BLACK)
        {
            num_black += 1;
            if (j == board->num_col - 1)
            {
                pattern[k] = num_black;
            }
        }
        else if (board->body[row][j] != BLACK && num_black != 0)
        {
            pattern[k] = num_black;
            num_black = 0;
            k++;
        }
    }

    return pattern;
}

int* pattern_col(Board* board, int col)
{
    int* pattern = (int*)calloc(MAX_SIZE, sizeof(int));
    int num_black = 0, k = 0;
    for (int i = 0; i < board->num_row; i++)
    {
        if (board->body[i][col] == BLACK)
        {
            num_black += 1;
            if (i == board->num_row - 1)
            {
                pattern[k] = num_black;
            }
        }
        else if (board->body[i][col] != BLACK && num_black != 0)
        {
            pattern[k] = num_black;
            num_black = 0;
            k++;
        }
    }

    return pattern;
}


bool is_valid(Board* board, Clue* clue)
{
    // check row
    for (int i = 0; i < board->num_row; i++)
    {
        int* pattern = pattern_row(board, i);
        bool match = true;
        for (int k = 0; k < MAX_SIZE; k++)
        {
            if (pattern[k] != clue->row[i][k])
                match = false;
        }
        
        free(pattern);
        if (!match)
            return false;
    }

    // check col
    for (int j = 0; j < board->num_col; j++)
    {
        int* pattern = pattern_col(board, j);
        bool match = true;
        for (int k = 0; k < MAX_SIZE; k++)
        {
            if (pattern[k] != clue->col[j][k])
                match = false;
        }

        free(pattern);
        if (!match)
            return false;
    }

    return true;
}

char* next_empty_cell(Board* board)
{
    for (int i = 0; i < board->num_row; i++)
    {
        for (int j = 0; j < board->num_col; j++)
        {
            if (board->body[i][j] == EMPTY)
                return &(board->body[i][j]);
        }
    }
}

void print_board(Board* board)
{
    for (int i = 0; i < board->num_row; i++)
    {
        for (int j = 0; j < board->num_col; j++)
            printf("%c", board->body[i][j]);
        printf("\n");
    }
}

bool Recur(Board* board, Clue* clue)
{
    if (is_fiiled(board))
    {
        if (is_valid(board, clue))
        {
            print_board(board);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        bool succeed;
        char* cell = next_empty_cell(board);

        *cell = BLACK;
        succeed = Recur(board, clue);
        if (succeed)
            return true;

        *cell = WHITE;
        succeed = Recur(board, clue);
        if (succeed)
            return true;

        *cell = EMPTY;
        return false;
    }
}