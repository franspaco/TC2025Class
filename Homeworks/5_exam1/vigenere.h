

#ifndef VIGENERE_H
#define VIGENERE_H

#define ALPHABET_LEN 26
#define A_OFFSET 97 // states the ascii value of the first letter of the alphabet, a is 97

char to_lower_case(char in);

char* vigenere_encode(char* key, char* src);
char* vigenere_encode_partial(char* key, char* src, int count, int chunk_size);
char* vigenere_encode_offset(char* key, char* src, int offset);
char vigenere_encode_char(char src, char key);
char vigenere_encode_letter(char src, char key);

char* vigenere_decode(char* key, char* src);
char* vigenere_decode_partial(char* key, char* src, int count, int chunk_size);
char* vigenere_decode_offset(char* key, char* src, int offset);
char vigenere_decode_char(char src, char key);
char vigenere_decode_letter(char src, char key);

#endif