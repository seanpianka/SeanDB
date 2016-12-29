#include "database.h"


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
