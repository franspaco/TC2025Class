/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vigenere.h"

/**
 * The definition of the alphabet
 */
char alphabet[] = "abcdefghijklmnopqrstuvwxyz";

/**
 * Makes letters lower case, returns the same char 
 * if it was not an upper case letter.
 */
char to_lower_case(char in){
    if((in >= 'A') && (in <= 'Z'))
        return in + 32;
    return in;
}

/**
 * Returns the encoded char given the original character 
 * and the character to use as key.
 * Takes in any char.
 */
char vigenere_encode_char(char src, char key){
    key = to_lower_case(key);
    src = to_lower_case(src);
    // printf("E: %c - %c\n", src, key);
    if(src >= 'a' && src <= 'z'){
        return vigenere_encode_letter(src, key);
    }
    else{
        return src;
    }
}

/**
 * Returns the decoded char given the original character 
 * and the character to use as key.
 * Takes in any char.
 */
char vigenere_decode_char(char src, char key){
    key = to_lower_case(key);
    src = to_lower_case(src);
    if(src >= 'a' && src <= 'z'){
        return vigenere_decode_letter(src, key);
    }
    else{
        return src;
    }
}

/** 
 * Find result in virtual table.
 * Takes in ONLY lower case letters!
 */
char vigenere_encode_letter(char src, char key){
    return alphabet[ (src-A_OFFSET + key-A_OFFSET) % ALPHABET_LEN];
}

/**
 * Backwards search in virtual table
 * Takes in ONLY lower case letters!
 */
char vigenere_decode_letter(char src, char key){
    // Undo the addition
    int out = src - key;

    // Pretend there's an overflow at ALPHABET_LEN, so roll back negative values
    if(out < 0)
        out = ALPHABET_LEN + out;

    return alphabet[out];
}

/**
 * Main function for encoding.
 * Requires:
 *   key - the key to use as a null terminated string
 *   src - the text to encode as a null terminated string
 *   offset - a number to offset the first letter of the key to be used.
 *            Useful when the src string is part of a bigger text.
 */
char* vigenere_encode_offset(char* key, char* src, int offset){
    // Get lengths
    int key_len = strlen(key);
    int src_len = strlen(src);

    // Allocate new memory
    char* out = malloc((src_len + 1) * sizeof(char));

    // Encode every character
    for(int i = 0; i < src_len; i++){
        out[i] = vigenere_encode_char(src[i], key[ (i + offset) % key_len]);
    }
    // Add a null terminator, for safety
    out[src_len] = '\0';
    return out;
}


/**
 * Main function for decoding.
 * Requires:
 *   key - the key to use as a null terminated string
 *   src - the text to decode as a null terminated string
 *   offset - a number to offset the first letter of the key to be used.
 *            Useful when the src string is part of a bigger text.
 */
char* vigenere_decode_offset(char* key, char* src, int offset){
    int key_len = strlen(key);
    int src_len = strlen(src);

    char* out = malloc((src_len + 1) * sizeof(char));

    for(int i = 0; i < src_len; i++){
        out[i] = vigenere_decode_char(src[i], key[ (i + offset) % key_len]);
    }
    out[src_len] = '\0';
    return out;
}

/**
 * The default, recomended function.
 * To be used when no offset is required, i.e. when
 * the text being encoded is completley contained in the 
 * string being passed. This won't keep track of where you
 * left off (what part of the key is next)
 */
char* vigenere_encode(char* key, char* src){
    return vigenere_encode_offset(key, src, 0);
}

/**
 * The default, recomended function.
 * To be used when no offset is required, i.e. when
 * the text being decoded is completley contained in the 
 * string being passed. This won't keep track of where you
 * left off (what part of the key is next)
 */
char* vigenere_decode(char* key, char* src){
    return vigenere_decode_offset(key, src, 0);
}

/**
 * To be used when encoding in chunks, this allows to keep track
 * of the next letter of the key to be used to encode all chunks
 * as if they were a single one.
 * The extra parameters required:
 *   count - the number of the chunk being processed, starts in 0
 *   chunk_size - the size of the chunks
 */
char* vigenere_encode_partial(char* key, char* src, int count, int chunk_size){
    return vigenere_encode_offset(key, src, chunk_size * count);
}

/**
 * To be used when encoding in chunks, this allows to keep track
 * of the next letter of the key to be used to encode all chunks
 * as if they were a single one.
 * The extra parameters required:
 *   count - the number of the chunk being processed, starts in 0
 *   chunk_size - the size of the chunks
 */
char* vigenere_decode_partial(char* key, char* src, int count, int chunk_size){
    return vigenere_decode_offset(key, src, chunk_size * count);
}