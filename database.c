#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BUF_LEN 1024
#define MAX_ROWS 128
#define MAX_PARAMS 32

typedef struct {
    uint32_t id;
    uint32_t set;
    char name[BUF_LEN];
    char email[BUF_LEN];
} Tuple;

typedef struct {
    Tuple rows[MAX_ROWS];
} Database;

typedef struct {
    FILE *file;
    Database *db;
} Connection;

void i_get(Connection *conn, const char *db_filename, uint32_t id);
void i_set(Connection *conn, const char *db_filename, uint32_t id, const char *name, const char *email);
void i_list(Connection *conn, const char *db_filename);
void i_delete(Connection *conn, const char *db_filename, uint32_t id);
void i_create(Connection *conn, const char *db_filename);

void die(const char *message);
void Tuple_print(Tuple *tuple);
Connection* Database_open(const char *filename, char mode);
void Database_close(Connection *conn);
void Database_create(Connection *conn);
void Database_load(Connection *conn);
void Database_write(Connection *conn);
void Database_set(Connection *conn, uint32_t id, const char *name, const char *email);
Tuple* Database_get(Connection *conn, uint32_t id);
void Database_delete(Connection *conn, uint32_t id);
void Database_list(Connection *conn, uint8_t show_empty);

int main(int argc, char** argv)
{
    if (argc < 2) { die("Usage: db.x <db_file>. Exiting."); }

    char *db_filename = argv[1];
    Connection *conn = NULL;

    uint8_t num_tokens = 0;
    char **split_input = calloc(MAX_PARAMS, sizeof(char*));
    char *tmp_input = calloc(BUF_LEN, sizeof(char));
    char *input = calloc(BUF_LEN, sizeof(char));
    char *buffer = NULL;
    if (!input) { die("Failed to allocate input buffer."); }

    /*
    if (argc > 3) { id = atoi(argv[3]); }
    if (id >= MAX_ROWS) { die("Provided row ID exceeds maximum rows allowed."); }
    die("Invalid operation.\n c=create, g=get, s=set, d=delete, l=list.");
    */

    for(;;)
    {
        // print prompt and get input
        printf("[SeanDB] > ");
        fgets(input, BUF_LEN, stdin);

        // make input safe, remove trailing newline
        input[BUF_LEN-1] = '\0';
        input[strcspn(input, "\n")] = 0;

        // create temp copy of input to tokenize
        strncpy(tmp_input, input, BUF_LEN);

        // array of strings containing tokenized input
        buffer = strtok(tmp_input, " ");
        split_input[0] = buffer;
        // convert first token (command) to lowercase
        for (uint32_t i = 0; split_input[0][i]; ++i)
        {
            split_input[0][i] = tolower(split_input[0][i]);
        }
        // tokenize remaining string
        for(num_tokens = 1; buffer != NULL; ++num_tokens)
        {
            if ((buffer = strtok(NULL, " ")) == NULL) { break; }
            split_input[num_tokens] = buffer;
        }

        // perform necessary command
        if (strcmp(split_input[0], "create") && num_tokens == 1)
        {
            i_create(
                conn,
                db_filename
            );
        }
        else if (strcmp(split_input[0], "get") && num_tokens == 2)
        {
            i_get(
                conn,
                db_filename,
                atoi(split_input[1])
            );
        }
        else if (strcmp(split_input[0], "set") && num_tokens == 4)
        {
            i_set(
                conn,
                db_filename,
                atoi(split_input[1]),
                split_input[2],
                split_input[3]
            );
        }
        else if (strcmp(split_input[0], "list") && num_tokens == 1)
        {
            i_list(
                conn,
                db_filename
            );
        }
        else if (strcmp(split_input[0], "delete") && num_tokens == 2)
        {
            i_delete(
                conn,
                db_filename,
                atoi(split_input[1])
            );
        }
        else
        {
            printf("Invalid command.\n");
        }


        // if EOF is sent, exit
        if (feof(stdin)) { break; }

        // set all input buffer to 0s
        memset(input, 0, BUF_LEN);
    }

    return 0;
}

void i_create(Connection *conn, const char *db_filename)
{
    conn = Database_open(db_filename, 'c');

    Database_create(conn);
    Database_write(conn);

    Database_close(conn);
}

void i_get(Connection *conn, const char *db_filename, uint32_t id)
{
    conn = Database_open(db_filename, 'g');

    Database_get(conn, id);

    Database_close(conn);
}

void i_set(Connection *conn, const char *db_filename, uint32_t id, const char *name, const char *email)
{
    conn = Database_open(db_filename, 's');

    Database_set(conn, id, name, email);
    Database_write(conn);

    Database_close(conn);
}

void i_list(Connection *conn, const char *db_filename)
{
    conn = Database_open(db_filename, 'l');

    Database_list(conn, 0);

    Database_close(conn);
}

void i_delete(Connection *conn, const char *db_filename, uint32_t id)
{
    conn = Database_open(db_filename, 'd');

    Database_delete(conn, id);
    Database_write(conn);

    Database_close(conn);
}

void die(const char *message)
{
    switch(errno)
    {
        case 0:
            printf("ERROR: %s\n", message);
            break;
        default:
            perror(message);
    }

    exit(1);
}

void Tuple_print(Tuple *tuple)
{
    printf("[%d]: { \"%s\", \"%s\" }\n", tuple->id, tuple->name, tuple->email);
}

void Database_load(Connection *conn)
{
    uint8_t rc = fread(conn->db, sizeof(Database), 1, conn->file);
    if (rc != 1) { die("Failed to load database."); }
}


Connection* Database_open(const char *filename, char mode)
{
    Connection *conn = calloc(1, sizeof(Connection));
    if (!conn) { die("Connection allocation error."); }

    conn->db = calloc(1, sizeof(Database));
    if (!conn->db) { die("Database allocation error."); }

    switch(mode)
    {
        case 'c':
            conn->file = fopen(filename, "w"); // write mode
            break;
        default:
            conn->file = fopen(filename, "r+"); // read/write mode, must exist
    }

    if (!conn->file) { die("Failed to open database file."); }
    Database_load(conn);

    return conn;
}

void Database_close(Connection *conn)
{
    if (conn)
    {
        if(conn->file) { fclose(conn->file); }
        if(conn->db)   { free(conn->db);     }
        free(conn);
        conn = NULL;
    }
}

void Database_write(Connection *conn)
{
    // instead of rewind because fseek returns an integer
    uint8_t rc = fseek(conn->file, 0, SEEK_END);
    if (rc != 1) { die("Failed to open database file."); }

    rc = fseek(conn->file, 0, SEEK_SET);
    if (rc != 1) { die("Failed to open database file."); }

    rc = fwrite(conn->db, sizeof(Database), 1, conn->file);
    if (rc != 1) { die("Failed to write to database file."); }

    rc = fflush(conn->file);
    if (rc != 1) { die("Failed to write to database file."); }
}

void Database_create(Connection *conn)
{
    uint32_t current_row = 0;

    for (current_row = 0; current_row < MAX_ROWS; ++current_row)
    {
        Tuple tuple = {.id = current_row, .set = 0};
        conn->db->rows[current_row] = tuple;
    }
}

void Database_set(Connection *conn, uint32_t id, const char *name, const char *email)
{
    Tuple *tuple = &conn->db->rows[id];
    if (tuple->set) { die("Row is already set, exiting."); }

    tuple->set = 1;

    char* rc = strncpy(tuple->name, name, BUF_LEN);
    tuple->name[BUF_LEN - 1] = '\0';
    if (!rc) { die("Failed to write name to tuple."); }

    rc = strncpy(tuple->email, email, BUF_LEN);
    tuple->email[BUF_LEN - 1] = '\0';
    if (!rc) { die("Failed to write email to tuple."); }
}

Tuple* Database_get(Connection *conn, uint32_t id)
{
    Tuple *tuple = &conn->db->rows[id];

    if (tuple->set) { return tuple; }

    die("Requested row has not been set.");
}

void Database_delete(Connection *conn, uint32_t id)
{
    Tuple tuple = { .id = id, .set = 0 };
    conn->db->rows[id] = tuple;
}

void Database_list(Connection *conn, uint8_t show_empty)
{
    uint32_t current_row = 0;

    for (; current_row < MAX_ROWS; ++current_row)
    {
        Tuple *current = &conn->db->rows[current_row];

        if (show_empty) { Tuple_print(current); }
        else if (current->set) { Tuple_print(current); }
    }
}
