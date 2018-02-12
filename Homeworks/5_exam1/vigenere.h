

#ifndef VIGENERE_H
#define VIGENERE_H

#define ALPHABET_LEN 26

char* vigenere_encode(char* key, char* src);
char vigenere_encode_char(char src, char key);

char* vigenere_decode(char* key, char* src);
char vigenere_decode_char(char src, char key);

#endif