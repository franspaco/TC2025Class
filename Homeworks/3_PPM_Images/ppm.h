/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * PPM.H
 * 
 * This program supports reading and writting P6 and P3 files.
 * Also supported: P6 files using either 1 or 2 bytes.
 * (Determined automatically through the max value)
 */

#ifndef PPM_H
#define PPM_H

// Colors
typedef enum color_enum {R, G, B} colot_t;

// Supported rotation angles
typedef enum rot_angles {d90, d180, d270} rot_ang;

// PPM file type
typedef enum ppm_img_type {ASCII, BINARY} ppm_type;

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

// Memory
void ppm_alloc(ppm_t* image);
void ppm_free(ppm_t* image);

// I/O
ppm_t* ppm_readFile(char* filename);
void ppm_writeFile(char* filename, const ppm_t* image, ppm_type type);
void ppm_getAsciiPixels(ppm_t* img, FILE* file);
void ppm_getBinaryPixels(ppm_t* img, FILE* file);

// Debug
void ppm_print(ppm_t* img);

// Editing
void ppm_invertColors(ppm_t* img);
ppm_t* ppm_rotate(ppm_t* img, rot_ang angle);

// Helpers / Misc
void ppm_pixelcpy(pixel_t* dest, pixel_t* src);

#endif