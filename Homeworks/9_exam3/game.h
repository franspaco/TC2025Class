
#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>

//typedef int** board_t;

typedef struct board_struct {
    int X;
    int Y;
    int** cells;
} board_t;

board_t* create_board(int sizex, int sizey);
void destroy_board(board_t* board);

board_t* board_from_file(char* filename);

void simulation(board_t* last, board_t* current);

void print_board(board_t* board);


#endif