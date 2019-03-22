#include <iostream>
#include "row.h"
#include "table.h"

struct InputBuffer_t {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};
typedef struct InputBuffer_t InputBuffer;

enum ExecuteResult_t {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};
typedef enum ExecuteResult_t ExecuteResult;

enum MetaCommandResult_t {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetaCommandResult;

enum PrepareResult_t {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID
};
typedef enum PrepareResult_t PrepareResult;

enum StatementType_t {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};
typedef StatementType_t StatementType;

struct Statement_t {
    StatementType type;
    Row row_to_insert; // only used by the insert statement
};
typedef struct Statement_t Statement;

// REPL
InputBuffer* new_input_buffer() {
    auto * input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db > ");
}

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

// Meta Commands
MetaCommandResult do_meta_command(InputBuffer* inputBuffer) {
    if (strcmp(inputBuffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

// Prepare Statements
PrepareResult prepare_insert(InputBuffer* inputBuffer, Statement* statement) {
    statement->type = STATEMENT_INSERT;

    char* keyword = strtok(inputBuffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer* inputBuffer, Statement* statement) {
    if (strncmp(inputBuffer->buffer, "insert", 6) == 0) {
        return prepare_insert(inputBuffer, statement);
    }
    if (strcmp(inputBuffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= table_max_rows(table)) {
        return EXECUTE_TABLE_FULL;
    }
    Row *row_to_insert = &(statement->row_to_insert);
    row_serialize(row_to_insert, table_row_slot(table, table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table-> num_rows; i++) {
        row_deserialize(table_row_slot(table, i), &row);
        row_print(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}

int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  Table* table = new_table(row_size());

  while (true) {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {
        switch (do_meta_command(input_buffer)) {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
        }
    }

    Statement statement;
    switch (prepare_statement(input_buffer, &statement)) {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_NEGATIVE_ID):
            printf("ID must be positive.\n");
            continue;
        case (PREPARE_STRING_TOO_LONG):
            printf("String is too long.\n");
            continue;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s'\n", input_buffer->buffer);
            continue;
    }

    switch(execute_statement(&statement, table)) {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table full.\n");
            break;
    };
  }
}