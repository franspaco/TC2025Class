/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * PPM.C
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"

void ppm_alloc(ppm_t* image){
    image->pixels = (pixel_t**)malloc((image->height) * sizeof(pixel_t*));
    image->pixels[0] = (pixel_t*)calloc((image->width) * (image->height), sizeof(pixel_t));
    for(int i = 0; i < (image->height); i++){
        image->pixels[i] = image->pixels[0] + image->width * i;
    }
}

void ppm_free(ppm_t* image){
    free(image->pixels[0]);
    free(image->pixels);
    free(image);
}

ppm_t* ppm_readFile(char* filename){
    // File to read
    FILE* file = fopen(filename, "r");
    // Pointer to image struct
    ppm_t * img = malloc(sizeof(ppm_t));
    // Read the magic number
    fgets(img->magic_number, 3, file);
    // Make sure there's a null terminator
    img->magic_number[2] = '\0';
    fscanf(file, "%i %i", &(img->height), &(img->width));
    fscanf(file, "%i", &(img->maxval));
    ppm_alloc(img);

    printf("MN: %s\n", img->magic_number);

    if(strcmp("P3", img->magic_number) == 0){
        printf("Type: P3\n");
        ppm_getAsciiPixels(img, file);
    }
    else if(strcmp("P6", img->magic_number) == 0){
        printf("Type: P6\n");
    }

    fclose(file);
    return img;
}

void ppm_getAsciiPixels(ppm_t* img, FILE* file){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            fscanf(file, "%hu", &(img->pixels[i][j].R));
            fscanf(file, "%hu", &(img->pixels[i][j].G));
            fscanf(file, "%hu", &(img->pixels[i][j].B));
        }
    }
}

void ppm_print(ppm_t* img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            printf("#%02X%02X%02X ", img->pixels[i][j].R, img->pixels[i][j].G, img->pixels[i][j].B);
        }
        printf("\n");
    }
}

void ppm_invertColors(ppm_t* img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            img->pixels[i][j].R = img->maxval - img->pixels[i][j].R;
            img->pixels[i][j].G = img->maxval - img->pixels[i][j].G;
            img->pixels[i][j].B = img->maxval - img->pixels[i][j].B;
        }
    }
}

void ppm_writeFile(char* filename, const ppm_t* image){

}

