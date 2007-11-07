#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h> 
#include <signal.h>

#include "sqlite.h"
#include "rewriteDB.h"
#include "rewriteFuncs.h"

/* Implementation of a function to return a random row:

    SELECT RAND(foo) FROM bar;

   Currently only the first 4096 rows will be looked at.    
*/

#define MAX_RAND_ROWS 4096

typedef struct _rand_function_context
{
    char *row_data[MAX_RAND_ROWS];
    int total_rows;
} rand_function_context;

void rewrite_rand_function_step(sqlite_func *func, int argc, const char **argv)
{
    rand_function_context *ctx;
    
    ctx = sqlite_aggregate_context(func, sizeof(*ctx));
    
    if(ctx && argv[0] && (ctx->total_rows < MAX_RAND_ROWS))
    {
        ctx->row_data[ctx->total_rows] = strdup(argv[0]);    
        ctx->total_rows++;
    }
    
}

void rewrite_rand_function_finalize(sqlite_func *func)
{
    rand_function_context *ctx;
    int rand_row, i;

    ctx = sqlite_aggregate_context(func, sizeof(*ctx));
    
    sqliteRandomness(sizeof(rand_row), &rand_row);

    rand_row = abs(rand_row);
    
    sqlite_set_result_string(func, ctx->row_data[rand_row % ctx->total_rows], -1);
    
    for(i = 0; i < ctx->total_rows; i++)
    {
        if(ctx->row_data[i])
        {
            free(ctx->row_data[i]);
        }
    }
    
}

int rewrite_register_custom_sqlite_functions(sqlite *db)
{
    if(sqlite_create_aggregate(db, "rand", 1,
                               rewrite_rand_function_step,
                               rewrite_rand_function_finalize,
                               NULL) != 0)
    {
        fprintf(stderr, "Could not register custom functions\n");
        return FAILURE;
    }
    
    return SUCCESS;
}
