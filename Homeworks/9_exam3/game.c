
#include "game.h"


board_t* create_board(int sizex, int sizey){
    board_t* board = malloc(sizeof(board));
    board->X = sizex;
    board->Y = sizey;
    board->cells = (int**)malloc(sizex * sizeof(int*));
    board->cells[0] = (int*)calloc(sizex * sizey, sizeof(int));
    for(int i = 0; i < (board->X); i++){
        board->cells[i] = board->cells[0] + board->X * i;
    }
    return board;
}

void destroy_board(board_t* board){
    free(board->cells[0]);
    free(board->cells);
    free(board);
}

board_t* board_from_file(char* filename){
    int sizex, sizey;
    FILE* file = fopen(filename, "w");
    if (file == NULL){
        printf("Cannot open image file!\n");
        exit(-1);
    }
    fscanf(file, "%i %i\n", &sizex, &sizey);

    board_t* board = create_board(sizex, sizey);

    for (int i = 0; i < sizey; i++){
        for (int j = 0; j < sizex; j++){
            fscanf(file, "%i", &(board->cells[j][i]));
        }
    }
    fclose(file);
}

void print_board(board_t* board){
    for (int i = 0; i < board->Y; i++){
        for (int j = 0; j < board->X; j++){
            printf("%i ", board->cells[j][i]);
        }
        printf("\n");
    }
}


void simulation(board_t* last, board_t* current){

}