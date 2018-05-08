
#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <omp.h>

// Board struct
typedef struct board_struct {
    int X;
    int Y;
    int** cells;
} board_t;

// Cell status enum
typedef enum cell_status {
    EMPTY,
    DEAD,
    ALIVE
} cs_t;

board_t* create_board(int sizex, int sizey);
void destroy_board(board_t* board);

board_t* board_from_file(char* filename);

void simulation(board_t* last, board_t* current);

void print_board(board_t* board);

int manage_overflows(int max, int val);
int get_value(board_t* board, int x, int y);



#endif