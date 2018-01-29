
#ifndef PPM_H
#define PPM_H

// Single Pixel
typedef struct pixel_struct {
    unsigned short R;
    unsigned short G;
    unsigned short B;
} pixel_t;

// PPM file
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

#endif