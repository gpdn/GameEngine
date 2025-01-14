
#include "file.h"

const char* read_file(const char* source) {
    FILE* file = fopen(source, "rb");

    if(file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    
    size_t last_byte_read = fread(buffer, sizeof(char), file_size, file);

    if(last_byte_read < file_size) {
        free(buffer);
        return NULL;
    }

    buffer[last_byte_read] = '\0';

    fclose(file);
    return buffer;
}

const char* read_file_no_terminator(const char* source, size_t* size) {
    FILE* file = fopen(source, "rb");

    if(file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    
    size_t last_byte_read = fread(buffer, sizeof(char), file_size, file);

    if(last_byte_read < file_size) {
        free(buffer);
        return NULL;
    }

    *size = file_size;

    fclose(file);
    return buffer;
}
