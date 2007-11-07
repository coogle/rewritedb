#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h> 
#include <signal.h>

#include "sqlite.h"
#include "rewriteDB.h"
#include "rewriteFuncs.h"

sqlite *db;

void exit_rewrite_app(int exit_code)
{
    if(db != NULL)
    {
        sqlite_close(db);
    }
    exit(exit_code);
}

void catch_signal(int signal_number)
{
    /* handle different signals differently if we want.. */
    switch(signal_number)
    {
        case SIGINT:
        case SIGTERM:
        default:
            break;
    }
    exit_rewrite_app(0);
}


int main(int argc, char *argv[])
{
    char url[1024];
    char *database_fn,
         *err_msg = NULL,
         *rewrite_url = NULL;
    
    if(argc < 2)
    {
        display_usage(argv[0]);
        exit(0);
    }
    
    database_fn = argv[1];
    
    db = sqlite_open(database_fn, 0666, &err_msg);
    
    if(db == NULL)
    {
        fprintf(stderr, "Could not open rewrite database: %s\n", err_msg);
        sqlite_freemem(err_msg);
        exit_rewrite_app(-1);
    }
    
    sqlite_busy_timeout(db, 2000);
    
    sqlite_exec(db, "PRAGMA default_synchronous = OFF", NULL, NULL, NULL);
    sqlite_exec(db, "PRAGMA count_changes = OFF", NULL, NULL, NULL);
 
    if(rewrite_register_custom_sqlite_functions(db) == FAILURE)
    {
        exit_rewrite_app(-1);
    }
    
    signal(SIGINT, catch_signal);
    signal(SIGTERM, catch_signal);
    
    setbuf(stdout, NULL);
  
    while (1)
    {
        if (fgets(url, 1024, stdin))
        {
            // strip newline
            url[strlen(url)-1]='\0';
            
            rewrite_url = resolve_rewrite_url(url);
            
            if(rewrite_url)
            {
                printf("%s\n", rewrite_url);
                free(rewrite_url);
            }
            else
            {
                printf("/\n");
            }
        }
    }
}

char *retrieve_url(char *query)
{
    char *error,
         *retval = NULL;
    const char *tail, **rowdata, **colnames;
    int  result, colcount;
    
    sqlite_vm *vm;
    
    if(!query)
    {
        fprintf(stderr, "Given a bad query\n");
        exit_rewrite_app(-1);
    }
    
    if(sqlite_compile(db, query, &tail, &vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not compile query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    sqlite_freemem(query);
    
    switch((result = sqlite_step(vm, &colcount, &rowdata, &colnames)))
    {
        case SQLITE_ROW:
            if(rowdata[0] != NULL)
            {
                retval = strdup(rowdata[0]);
            }
            break;
        default:
            break;
    }
    
    if(sqlite_finalize(vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not finalize query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    return retval;
}

char *retrieve_default_rewrite_url()
{
    char *query,
         *error,
         *retval = NULL;
    const char *tail, **rowdata, **colnames;
    int  result, colcount;
    
    sqlite_vm *vm;
    
    query = sqlite_mprintf("SELECT value FROM global_settings WHERE setting='default_url'");
    
    if(query == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        exit_rewrite_app(-1);
    }

    if(sqlite_compile(db, query, &tail, &vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not compile query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    sqlite_freemem(query);
    
    switch((result = sqlite_step(vm, &colcount, &rowdata, &colnames)))
    {
        case SQLITE_ROW:
            if(rowdata[0] != NULL)
            {
                retval = strdup(rowdata[0]);
            }
            break;
        default:
            break;
    }
    
    if(sqlite_finalize(vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not finalize query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    return retval;
}

char *resolve_rewrite_url(char *url)
{
    char *query,
         *error,
         *retval = NULL;
    const char *tail, **rowdata, **colnames;
    sqlite_vm *vm;
    int  result, colcount;
    
    query = sqlite_mprintf("SELECT groupID FROM rewrites WHERE rewriteURL='%q' LIMIT 1", url);
    
    if(query == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        exit_rewrite_app(-1);
    }
    
    if(sqlite_compile(db, query, &tail, &vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not compile query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    sqlite_freemem(query);
    
    switch((result = sqlite_step(vm, &colcount, &rowdata, &colnames)))
    {
        case SQLITE_ROW:
            if(rowdata[0] != NULL)
            {
                /* This is where we pick the type of rotation we're doing
                   and then provide the appropiate query to the retrieve_url()
                   function */
                
                query = sqlite_mprintf("SELECT RAND(url) FROM group_members WHERE group_id=%s", rowdata[0]);
                retval = retrieve_url(query);
            }
            break;
        case SQLITE_DONE:
            /* Use default rewrite url if one exists */
            retval = retrieve_default_rewrite_url();
            break;
        default:
            break;
    }
    
    if(sqlite_finalize(vm, &error) != SQLITE_OK)
    {
        fprintf(stderr, "Could not finalize query (%s)\n", error);
        sqlite_freemem(error);
        sqlite_freemem(query);
        exit_rewrite_app(-1);
    }
    
    return retval;
}

void display_usage(char *prog)
{
    printf("Usage:\n\t%s <sqlite_database>\n", prog);
}
