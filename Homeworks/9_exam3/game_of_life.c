/*
    WRITE YOUR NAME HERE:
    Francisco Huelsz Prince
    A01019512

    Simulation of Conway's Game of Life using OpenMP
    Based on the explanations at:
        https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
    Online versions:
        http://pmav.eu/stuff/javascript-game-of-life-v3.1.1/
        https://bitstorm.org/gameoflife/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pgm_image.h"
// ADD YOUR EXTRA LIBRARIES HERE
#include "game.h"
#include "tiny_png/TinyPngOut.h"

#define STRING_SIZE 50

///// Function declarations /////
void usage(const char * program);
void lifeSimulation(int iterations, char * start_file);
void saveAsPNG(const board_t * board, int iteration);
// ADD YOUR FUNCTION DECLARATIONS HERE

int main(int argc, char * argv[])
{
    char * start_file = "../Boards/pulsar.txt";
    int iterations = 5;

    printf("\n=== GAME OF LIFE ===\n");
    printf("{By: Francisco Huelsz Prince}\n");

    // Parsing the arguments
    if (argc == 2)
    {
        iterations = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        iterations = atoi(argv[1]);
        start_file = argv[2];
    }
    else if (argc != 1)
    {
        usage(argv[0]);
        return 1;
    }

    // Run the simulation with the iterations specified
    printf("Running the simulation with file '%s' using %d iterations\n", start_file, iterations);
    lifeSimulation(iterations, start_file);

    return 0;
}

// Print usage information for the program
void usage(const char * program)
{
    printf("Usage:\n");
    printf("%s [iterations] [board file]\n", program);
}

clock_t t;

// Main loop for the simulation
void lifeSimulation(int iterations, char * start_file) {
    board_t* last = board_from_file(start_file);
    printf("Board ready! (%i)\n", iterations);
    double avg_png_time = 0;
    double avg_sim_time = 0;

    for (int i = 0; i < iterations; i++) {
        //printf("It: %03i\n", i);

        // Save last to file
        t = clock();
        saveAsPNG(last, i);
        //print_board(last);
        avg_png_time += (double)(clock()-t)/CLOCKS_PER_SEC;

        // Create new board
        board_t* current = create_board(last->X, last->Y);

        // Simulate
        t = clock();
        simulation(last, current);
        avg_sim_time += (double)(clock()-t)/CLOCKS_PER_SEC;

        // Destroy last
        destroy_board(last);
        // Make current board the last
        last = current;
    }
    printf("Avg sim  time: %fs\n", avg_sim_time/(double)iterations);
    printf("Avg save time: %fs\n", avg_png_time/(double)iterations);
    printf("\n");
}

// Save the contents of the board as a PGM image
void saveAsPNG(const board_t * board, int iteration) {
    char buffer[50];
    sprintf(buffer, "frames/frame_%04i.png", iteration);
    // Initialize file and Tiny Png Output
    FILE* fout = fopen(buffer, "wb");
    struct TinyPngOut pngout;
    if(fout == NULL || TinyPngOut_init(&pngout, fout, board->X, board->Y) != TINYPNGOUT_OK){
        // Error
        exit(2);
    }

    // Once file is ok format the data:
    int full_size = (board->X) * (board->Y);
    int pixels_total = full_size * 3;
    uint8_t* pixels = malloc(pixels_total*sizeof(uint8_t));
    uint8_t R = 255, G = 255, B = 255;
    for(int i = 0; i < full_size; i++){
        switch (board->cells[0][i]){
            case EMPTY:
                R = 255;
                G = 255;
                B = 255;
                break;
            case DEAD:
                R = 200;
                G = 200;
                B = 255;
                break;
            case ALIVE:
                R = 0;
                G = 0;
                B = 0;
                break;
        }
        pixels[i*3 + 0] = R;
        pixels[i*3 + 1] = G;
        pixels[i*3 + 2] = B;
    }

    // Write image data
	if (TinyPngOut_write(&pngout, pixels, full_size) != TINYPNGOUT_OK){
        // Error
        exit(3);
    }
    // Check for proper completion
	if (TinyPngOut_write(&pngout, NULL, 0) != TINYPNGOUT_DONE){
        // Error
        exit(3);
    }
	fclose(fout);
    free(pixels);
}
