
int rewrite_register_custom_sqlite_functions(sqlite *);

void rewrite_rand_function_step(sqlite_func *, int, const char **);
void rewrite_rand_function_finalize(sqlite_func *);
