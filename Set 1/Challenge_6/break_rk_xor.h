typedef struct {
  size_t keysize;
  double distance;
} key;

unsigned char keymap[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz .?!,'&";

key *get_keys(char *src, size_t len);
size_t hamming_distance(char *s1, char *s2, size_t len);
void sort_keys(key *r, size_t n);
void swap(key *a, key *b);
char *rk_xor(size_t keysize, size_t ctxt_len, char *key, char *ctxt);
size_t strscore(char *str, unsigned char *keymap, size_t keymap_len,
                size_t str_len);
char *sk_xor(char key, char *ctxt, size_t ctxt_len);