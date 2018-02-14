

#ifndef VIGENERE_WORKER_H
#define VIGENERE_WORKER_H

#define BUFFER_SIZE 512

char* create_outfile_name(char* filename, char action);

void worker_start(char action, char* key, char* fileName, FILE* src_file, int* fd);

#endif