#include "database.h"
#include "prompt.h"

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

    for(;;)
    {
        // print prompt and get input
        printf("[SeanDB] > ");
        fgets(input, BUF_LEN, stdin);

        // make input safe, remove trailing newline
        input[BUF_LEN-1] = '\0';
        input[strcspn(input, "\n")] = 0;

        // if EOF is sent, exit
        if (feof(stdin)) { break; }

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
        if (!strcmp(split_input[0], "create") && num_tokens == 1)
        {
            i_create(
                conn,
                db_filename
            );
        }
        else if (!strcmp(split_input[0], "get") && num_tokens == 2)
        {
            i_get(
                conn,
                db_filename,
                atoi(split_input[1])
            );
        }
        else if (!strcmp(split_input[0], "set") && num_tokens == 4)
        {
            i_set(
                conn,
                db_filename,
                atoi(split_input[1]),
                split_input[2],
                split_input[3]
            );
        }
        else if (!strcmp(split_input[0], "list") && num_tokens == 1)
        {
            i_list(
                conn,
                db_filename
            );
        }
        else if (!strcmp(split_input[0], "delete") && num_tokens == 2)
        {
            i_delete(
                conn,
                db_filename,
                atoi(split_input[1])
            );
        }
        else
        {
            printf("Invalid operation. Use create, get, get, delete, list.\n");
        }


        // set all input buffer to 0s
        memset(input, 0, BUF_LEN);
        memset(split_input, 0, BUF_LEN);
    }

    free(split_input);
    free(tmp_input);
    free(input);

    return 0;
}
