
#include <stdio.h>

#include "ppm.h"

/**
 * Main function
 * REQUIRES 1 ARGUMENT:
 *  - ppm file name
 * 
 */
int main(int argc, char** argv){
    ppm_t* img = ppm_readFile(argv[1]);

    // Originnal
    printf("Original:\n");
    ppm_print(img);

    // Rotate images
    ppm_t* img2 = ppm_rotate(img, d90);
    ppm_t* img3 = ppm_rotate(img, d180);
    ppm_t* img4 = ppm_rotate(img, d270);

    printf("\nRoated: 90°\n");
    ppm_print(img2);

    printf("\nRoated: 180°\n");
    ppm_print(img3);

    printf("\nRoated: 270°\n");
    ppm_print(img4);

    printf("\nInverted colors:\n");
    ppm_invertColors(img);
    ppm_print(img);

    // Write inverted image to file
    char saveFile[] = "inverted.ppm";
    printf("Saving file to %s\n", saveFile);
    ppm_writeFile(saveFile, img, BINARY);

    // Free all the copies of the image
    ppm_free(img);
    ppm_free(img2);
    ppm_free(img3);
    ppm_free(img4);
    return 0;
}