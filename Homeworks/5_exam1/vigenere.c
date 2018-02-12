
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vigenere.h"

char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/** 
 * Find result in virtual table
 */
char vigenere_encode_char(char src, char key){
    // The -65 is to normalize letters to numbers 0-25
    return alphabet[ (src-65 + key-65) % ALPHABET_LEN];
    // Same as:
    // return matrix[src - 65][key - 65];
}

/**
 * Backwards search in virtual table
 */
char vigenere_decode_char(char src, char key){
    // Undo the addition
    // The -65 is to normalize letters to numbers 0-25
    int out = (src-65) - (key-65);

    // Pretend there's an overflow at ALPHABET_LEN, so roll back negative values
    if(out < 0)
        out = ALPHABET_LEN + out;

    return alphabet[out];
}


char* vigenere_encode(char* key, char* src){
    int key_len = strlen(key);
    int src_len = strlen(src);

    // Key to upper case
    for(int i = 0; i < key_len; i++){
        if((key[i] >= 'a') && (key[i] <= 'z')){
            key[i] = key[i] - 32;
        }
    }

    char* out = malloc((src_len + 1) * sizeof(char));

    for(int i = 0; i < src_len; i++){
        // printf("Encoding: '%c' (%i)\n", src[i], src[i]);
        // To upper case
        if((src[i] >= 'a') && (src[i] <= 'z'))
            src[i] -= 32;
        
        // Skip non-letters
        if(src[i] >= 'A' && src[i] <= 'Z'){
            out[i] = vigenere_encode_char(src[i], key[ i % key_len]);
        }
        else{
            out[i] = src[i];
        }
    }
    return out;
}

char* vigenere_decode(char* key, char* src){
    int key_len = strlen(key);
    int src_len = strlen(src);

    // Key to upper case
    for(int i = 0; i < key_len; i++){
        if((key[i] >= 'a') && (key[i] <= 'z')){
            key[i] = key[i] - 32;
        }
    }

    char* out = malloc((src_len + 1) * sizeof(char));

    for(int i = 0; i < src_len; i++){
        // To upper case
        if((src[i] >= 'a') && (src[i] <= 'z'))
            src[i] -= 32;
        
        // Skip non-letters
        if(src[i] >= 'A' && src[i] <= 'Z'){
            out[i] = vigenere_decode_char(src[i], key[ i % key_len]);
        }
        else{
            out[i] = src[i];
        }
    }
    return out;
}