#ifndef H_FILE
#define H_FILE

#include <stdio.h>
#include <stdlib.h>

const char* read_file(const char* source);
const char* read_file_no_terminator(const char* source, size_t* file_size);

#endif
