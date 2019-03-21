#include <iostream>
#include "row.h"
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

uint32_t row_size() {
    return ROW_SIZE;
}

void row_serialize(Row* source, void* destination) {
    memcpy(((char*) destination) + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(((char*) destination) + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(((char*) destination) + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void row_deserialize(void* source, Row* destination) {
    memcpy(&(destination->id), ((char*) source) + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), ((char*) source) + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), ((char*) source) + EMAIL_OFFSET, EMAIL_SIZE);
}

void row_print(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}