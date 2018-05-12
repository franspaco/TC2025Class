/**
 * Francisco Huelsz Prince
 * A01019512
 */

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

// Create and allocate a board
board_t* create_board(int sizex, int sizey);
// Destroy and free all data in a board
void destroy_board(board_t* board);

// Create a board from a filename
board_t* board_from_file(char* filename);

// Main simulation function, has different implementations (See readme)
void simulation(board_t* last, board_t* current);

// Helper function to print a board
void print_board(board_t* board);

// Makes sure a value is within a specific range handling overflows.
int manage_overflows(int max, int val);

// Gets the status of a cell in a board, handles overflows internally.
int get_value(board_t* board, int x, int y);



#endif