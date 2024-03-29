#ifndef SQLITE_CLONE_ROW_H
#define SQLITE_CLONE_ROW_H
#include <iostream>

const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row_t {
    uint32_t id;
    // C strings are supposed to end with a null character
    // so allocate an additional byte for that
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};
typedef struct Row_t Row;

void row_serialize(Row* source, void* destination);
void row_deserialize(void* source, Row* destination);
void row_print(Row* row);
uint32_t row_size();

#endif //SQLITE_CLONE_ROW_H
