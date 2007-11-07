
extern int sqlite_encode_binary(const unsigned char *in, int n, unsigned char *out);
extern int sqlite_decode_binary(const unsigned char *in, unsigned char *out);
extern void sqliteRandomness(int, void*);

void display_usage(char *);
char *resolve_rewrite_url(char *);
void exit_rewrite_app(int);
void catch_signal(int);
char *retrieve_url(char *);
char *retrieve_default_rewrite_url();
char *resolve_rewrite_url(char *);

#define SUCCESS 0
#define FAILURE -1
