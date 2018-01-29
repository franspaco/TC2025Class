/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * PPM.C
 * 
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"

/**
 * Allocate all memory required by an image's pixels.
 */
void ppm_alloc(ppm_t* image){
    image->pixels = (pixel_t**)malloc((image->height) * sizeof(pixel_t*));
    image->pixels[0] = (pixel_t*)calloc((image->width) * (image->height), sizeof(pixel_t));
    for(int i = 0; i < (image->height); i++){
        image->pixels[i] = image->pixels[0] + image->width * i;
    }
}

/**
 * Free all data associated with a ppm_t
 */
void ppm_free(ppm_t* image){
    free(image->pixels[0]);
    free(image->pixels);
    free(image);
}

/**
 * Read a file containing a ppm image and return a pointer to a new ppm_t
 */
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
    fscanf(file, "%i\n", &(img->maxval));
    ppm_alloc(img);

    //printf("MN: %s\n", img->magic_number);

    if(strcmp("P3", img->magic_number) == 0){
        printf("Type: P3\n");
        ppm_getAsciiPixels(img, file);
    }
    else if(strcmp("P6", img->magic_number) == 0){
        printf("Type: P6\n");
        ppm_getBinaryPixels(img, file);
    }

    fclose(file);
    return img;
}

/**
 * read pixels in ascii format from file
 */
void ppm_getAsciiPixels(ppm_t* img, FILE* file){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            fscanf(file, "%hu", &(img->pixels[i][j].R));
            fscanf(file, "%hu", &(img->pixels[i][j].G));
            fscanf(file, "%hu", &(img->pixels[i][j].B));
        }
    }
}

void ppm_getBinaryPixels(ppm_t* img, FILE* file){
    char bytesToRead = (img->maxval < 256)? 1 : 2;
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            unsigned char hi_b = 0;
            unsigned char lo_b = 0;

            unsigned short temp[3];

            for(int i = 0; i < 3; i++){
                if(bytesToRead == 2){
                    fread(&hi_b, 1, 1, file);
                }
                fread(&lo_b, 1, 1, file);
                temp[i] = (hi_b << 8) + lo_b;
            }

            img->pixels[i][j].R = temp[0];
            img->pixels[i][j].G = temp[1];
            img->pixels[i][j].B = temp[2];
        }
    }
}

/**
 * Print hex codes of an image.
 * Most useful for debugging
 */
void ppm_print(ppm_t* img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            printf("#%02X%02X%02X ", img->pixels[i][j].R, img->pixels[i][j].G, img->pixels[i][j].B);
        }
        printf("\n");
    }
}

/**
 * Inverts the colors of the provided image
 */
void ppm_invertColors(ppm_t* img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            img->pixels[i][j].R = img->maxval - img->pixels[i][j].R;
            img->pixels[i][j].G = img->maxval - img->pixels[i][j].G;
            img->pixels[i][j].B = img->maxval - img->pixels[i][j].B;
        }
    }
}

/**
 * Rotate image by given degrees and return a new, rotated copy
 */
ppm_t* ppm_rotate(ppm_t* img, rot_ang angle){
    ppm_t * n_img = malloc(sizeof(ppm_t));
    strncpy(n_img->magic_number, img->magic_number, 3);
    n_img->maxval = img->maxval;
    switch(angle){
        case d90:
            // Dimensions change therefore width and height are swapped
            n_img->height = img->width;
            n_img->width  = img->height;
            ppm_alloc(n_img);
            for(int i = 0; i < n_img->height; i++){ // i-> n_img height | img width
                for(int j = 0; j < n_img->width; j++){ // j-> n_img width | img height
                    // Copy pixels inverting the old image height
                    ppm_pixelcpy(&(n_img->pixels[i][j]), &(img->pixels[img->height - j - 1][i]));
                }
            }
            break;
        case d180:
            // Dimensions are kept the same
            n_img->height = img->height;
            n_img->width  = img->width;
            ppm_alloc(n_img);
            for(int i = 0; i < n_img->height; i++){ // i-> n_img height | img height
                for(int j = 0; j < n_img->width; j++){ // j-> n_img width | img width
                    // Copy pixels inverting the old image height and width
                    ppm_pixelcpy(&(n_img->pixels[i][j]), &(img->pixels[img->height - i - 1][img->width - j - 1]));
                }
            }
            break;
        case d270:
            // Dimensions change therefore width and height are swapped
            n_img->height = img->width;
            n_img->width  = img->height;
            ppm_alloc(n_img);
            for(int i = 0; i < n_img->height; i++){ // i-> n_img height | img width
                for(int j = 0; j < n_img->width; j++){ // j-> n_img width | img height
                    // Copy pixels inverting the old image width
                    ppm_pixelcpy(&(n_img->pixels[i][j]), &(img->pixels[j][img->width - i - 1]));
                }
            }
            break;
        default:
            // Unsupported options
            free(img);
            n_img = NULL;
    }
    return n_img;
}

/**
 * Copy pixel data from src to dest
 */
void ppm_pixelcpy(pixel_t* dest, pixel_t* src){
    dest->R = src->R;
    dest->G = src->G;
    dest->B = src->B;
}

void ppm_writeFile(char* filename, const ppm_t* img, ppm_type type){
    FILE* file = fopen(filename, "w");

    switch(type){
        case ASCII:
            fprintf(file, "P3\n");
            break;
        case BINARY:
            fprintf(file, "P6\n");
            break;
        default:
            //Unsupported
            fclose(file);
            return;
    }

    fprintf(file, "%i %i\n", img->height, img->width);
    fprintf(file, "%i\n", img->maxval);

    char bytesToWrite = (img->maxval < 256)? 1 : 2;

    unsigned char hi_b = 0;
    unsigned char lo_b = 0;

    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img->width; j++){
            switch(type){
                case ASCII:
                    // just write the values to the file as ascii
                    fprintf(file, "%i %i %i ", img->pixels[i][j].R, img->pixels[i][j].G, img->pixels[i][j].B);
                    break;
                case BINARY:
                    // Get the high and low byte for RED
                    // This assumes the system is little andian
                    lo_b = img->pixels[i][j].R;
                    hi_b = img->pixels[i][j].R >> 8;

                    // Write the high byte if required
                    if(bytesToWrite == 2){
                        fwrite(&hi_b, 1, 1, file);
                    }
                    // Write the low byte
                    fwrite(&lo_b, 1, 1, file);

                    // Get the high and low byte for GREEN
                    lo_b = img->pixels[i][j].G;
                    hi_b = img->pixels[i][j].G >> 8;

                    // Write the high byte if required
                    if(bytesToWrite == 2){
                        fwrite(&hi_b, 1, 1, file);
                    }
                    // Write the low byte
                    fwrite(&lo_b, 1, 1, file);

                    // Get the high and low byte for BLUE
                    lo_b = img->pixels[i][j].B;
                    hi_b = img->pixels[i][j].B >> 8;

                    // Write the high byte if required
                    if(bytesToWrite == 2){
                        fwrite(&hi_b, 1, 1, file);
                    }
                    // Write the low byte
                    fwrite(&lo_b, 1, 1, file);

            }
        }
        if(type == ASCII){
            fprintf(file, "\n");
        }
    }
    fclose(file);
}

