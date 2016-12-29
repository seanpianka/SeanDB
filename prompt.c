#include "prompt.h"
#include "database.h"

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

