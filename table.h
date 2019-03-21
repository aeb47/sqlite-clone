#ifndef SQLITE_CLONE_TABLE_H
#define SQLITE_CLONE_TABLE_H

#include <iostream>

// Page size is 4KB b/c it's the same size as a page used in the virtual memory
// systems of most computer architectures. This means one page in the db corresponds
// to one page used by the operating system.
const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;

struct Table_t {
    void* pages[TABLE_MAX_PAGES];
    uint32_t num_rows;
    uint32_t row_size;
};
typedef struct Table_t Table;

uint32_t table_rows_per_page(Table* table);
uint32_t table_max_rows(Table* table);
void* table_row_slot(Table* table, uint32_t row_num);
Table* new_table(uint32_t row_size);

#endif //SQLITE_CLONE_TABLE_H
