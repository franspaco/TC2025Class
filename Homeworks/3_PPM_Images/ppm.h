/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * PPM.H
 * 
 */

#ifndef PPM_H
#define PPM_H

typedef enum color_enum {R, G, B} colot_t;

// Single Pixel
// Includes a short for each color channel
typedef struct pixel_struct {
    unsigned short R;
    unsigned short G;
    unsigned short B;
} pixel_t;

// PPM file
// Includes the magic number
typedef struct ppm_struct{
    char magic_number[3];
    int width;
    int height;
    int maxval;
    pixel_t ** pixels;
} ppm_t;

void ppm_alloc(ppm_t* image);
void ppm_free(ppm_t* image);
ppm_t* ppm_readFile(char* filename);
void ppm_writeFile(char* filename, const ppm_t* image);
void ppm_getAsciiPixels(ppm_t* img, FILE* file);

void ppm_print(ppm_t* img);

void ppm_invertColors(ppm_t* img);

#endif