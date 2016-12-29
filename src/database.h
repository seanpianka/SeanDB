#ifndef SEAN_DB_SRC_H
#define SEAN_DB_SRC_H

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

#endif
