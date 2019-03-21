#include <iostream>
#include "table.h"

uint32_t table_rows_per_page(Table* table) {
    return PAGE_SIZE / table->row_size;
}

uint32_t table_max_rows(Table* table) {
    return table_rows_per_page(table) * TABLE_MAX_PAGES;
}

void* table_row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / table_rows_per_page(table);
    void* page = table->pages[page_num];
    if (!page) {
        // Allocate memory only when we try to access a page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    // Offset of the row within the current page
    uint32_t row_offset = row_num % table_rows_per_page(table);
    // Offset in bytes of the row
    uint32_t byte_offset = row_offset * table->row_size;
    return ((char *) page) + byte_offset;
}

Table* new_table(uint32_t row_size) {
    auto table = (Table*) malloc(sizeof(Table));
    table->num_rows = 0;
    table->row_size = row_size;
    return table;
}