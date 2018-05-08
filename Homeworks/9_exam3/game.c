
#include "game.h"


board_t* create_board(int sizex, int sizey){
    board_t* board = malloc(sizeof(board_t));
    board->X = sizex;
    board->Y = sizey;
    board->cells = (int**)malloc(sizey * sizeof(int*));
    board->cells[0] = (int*)calloc(sizex * sizey, sizeof(int));
    for(int i = 0; i < (board->Y); i++){
        board->cells[i] = board->cells[0] + board->X * i;
    }
    // Initialize to zeros
    for(int i = 0; i < (sizex * sizey); i++){
        board->cells[0][i] = 0;
    }
    return board;
}

void destroy_board(board_t* board){
    free(board->cells[0]);
    free(board->cells);
    free(board);
}

board_t* board_from_file(char* filename){
    int sizex = 0, sizey = 0;
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        printf("Cannot open image file!\n");
        exit(-1);
    }
    // Scan size
    fscanf(file, "%i %i\n", &sizey, &sizex);
    // Create board of that size
    board_t* board = create_board(sizex, sizey);
    // Read file into board
    int buffer = 0;
    for (int i = 0; i < sizey; i++){
        for (int j = 0; j < sizex; j++){
            fscanf(file, "%i ", &buffer);
            // Translate 1's into Cell State enum
            board->cells[i][j] = (buffer==1)? ALIVE : EMPTY;
        }
    }
    fclose(file);
    return board;
}

char encode(int val){
    switch(val){
        case EMPTY:
            return ' ';
        case DEAD:
            return '.';
        case ALIVE:
            return 'O';
    }
}

void print_board(board_t* board){
    for (int i = 0; i < board->Y; i++){
        for (int j = 0; j < board->X; j++){
            printf("%c|", encode(board->cells[i][j]));
        }
        printf("\n");
    }
}

int manage_overflows(int max, int val){
    // val is within range
    if( val >= 0 && val < max){
        return val;
    }
    // val is negative overflowed
    else if ( val < 0 ){
        while(val < 0){
            val += max;
        }
        return val;
    }
    // val is positive overflowed
    else {
        return val % max;
    }
}

int get_value(board_t* board, int x, int y){
    x = manage_overflows(board->X, x);
    y = manage_overflows(board->Y, y);
    return board->cells[y][x];
}

int count_live_neughbours(board_t* board, int x, int y){
    int count = 0;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if (i == 0 && j == 0)
                continue;
            count += (get_value(board, x + i, y + j) == ALIVE) ? 1 : 0;
        }
    }
    return count;
}

int get_next_status(board_t* board, int x, int y){
    int status = board->cells[y][x];
    int neighbours = count_live_neughbours(board, x, y);
    // Cell was dead in last frame
    if (status == EMPTY || status == DEAD){
        if (neighbours == 3){
            return ALIVE;
        }
        else {
            return status;
        }
    }
    // Cell was alive in last frame
    else{
        // If (less than 2) or (more than 3) neighbours alive: kill cell
        if (neighbours < 2 || neighbours > 3){
            return DEAD;
        }
        // If 2 or 3 neighbours: let it live
        else {
            return ALIVE;
        }
    }
}

typedef struct thread_data{
    board_t* last;
    board_t* current;
    int row_init;
    int row_end;
}data_t;


//#define SIM_MODE 1

#ifdef SIM_MODE
#if SIM_MODE==1

#define MAX_THREADS 2

void* thread_calculate_rows(void* args){
    data_t* data = (data_t*)args;
    printf("Doing: %i -> %i %ix%i\n", data->row_init, data->row_end, data->row_end-data->row_init, data->current->X);
    clock_t t = clock();
    int count = 0;
    for (int y = data->row_init; y < data->row_end; y++){
        for (int x = 0; x < (data->current->X); x++){
            data->current->cells[y][x] = get_next_status(data->last, x, y);
            count++;
        }
    }
    printf("Assert: %i == %i\n", (data->row_end-data->row_init)*(data->current->X), count);
    printf("Took: %fs\n", (double)(clock()-t)/CLOCKS_PER_SEC);
    free(data);
    pthread_exit(0);
}

void make_thread(board_t* last, board_t* current, pthread_t* tid, int y_min, int y_max){
    data_t* data = malloc(sizeof(data_t));
    data->last = last;
    data->current = current;
    data->row_init = y_min;
    data->row_end = y_max;
    pthread_create(tid, NULL, thread_calculate_rows, (void*)data);
}
void simulation(board_t* last, board_t* current){
    pthread_t* tids = malloc(MAX_THREADS * sizeof(pthread_t));
    pthread_t extra_thread;
    int rowspth = (current->Y) / MAX_THREADS;

    if(rowspth != 0){
        for(int t = 0; t < MAX_THREADS-1; t++){
            printf("Launch %i -> %i\n", t*rowspth, (t+1)*rowspth);
            make_thread(last, current, &tids[t], t*rowspth, (t+1)*rowspth);
        }
    }
    printf("Launch %i -> %i\n", (MAX_THREADS-1)*rowspth, current->Y);
    make_thread(last, current, &tids[MAX_THREADS-1], (MAX_THREADS-1)*rowspth, current->Y);
    for(int y = 0; y < MAX_THREADS; y++){
        pthread_join(tids[y], NULL);
        printf("Join\n");
    }
    free(tids);
}
#elif SIM_MODE==2
void simulation(board_t* last, board_t* current){
    #pragma omp parallel for default(none) shared(last, current)
    for (int y = 0; y < current->Y; y++){
        for (int x = 0; x < current->X; x++){
            current->cells[y][x] = get_next_status(last, x, y);
        }
    }
}
#endif
#else
void simulation(board_t* last, board_t* current){
    for (int y = 0; y < current->Y; y++){
        for (int x = 0; x < current->X; x++){
            current->cells[y][x] = get_next_status(last, x, y);
        }
    }
}
#endif