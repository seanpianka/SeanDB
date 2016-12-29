#ifndef SEAN_DB_PROMPT_H
#define SEAN_DB_PROMPT_H

#include "database.h"


void i_get(Connection *conn, const char *db_filename, uint32_t id);
void i_set(Connection *conn, const char *db_filename, uint32_t id, const char *name, const char *email);
void i_list(Connection *conn, const char *db_filename);
void i_delete(Connection *conn, const char *db_filename, uint32_t id);
void i_create(Connection *conn, const char *db_filename);

#endif
