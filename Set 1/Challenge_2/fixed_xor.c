#include "hex2str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char hex_str[] = "1c0111001f010100061a024b53535009181c";
static const unsigned char hex_key[] = "686974207468652062756c6c277320657965";
static const unsigned char res_hex[] = "746865206b696420646f6e277420706c6179";

char *xor_shuffle(const char *str, const char *key, size_t len) {
  size_t i;
  char *dst = malloc(len * sizeof(char) + 1);
  for (i = 0; i < len; i++) {
    dst[i] = str[i] ^ key[i];
  }
  dst[i] = '\0';

  return dst;
}

int main(void) {
  size_t str_len;
  size_t key_len;
  size_t res_len;
  char *str = hex_to_string(hex_str, strlen((char *)hex_str), &str_len);
  char *key = hex_to_string(hex_key, strlen((char *)hex_key), &key_len);
  char *result = hex_to_string(res_hex, strlen((char *)res_hex), &res_len);
  char *shuffled = xor_shuffle(str, key, key_len);

  if (strcmp(shuffled, result) == 0) {
    printf("Results match\n");
  }

  free(str);
  free(key);
  free(result);
  free(shuffled);

  return 0;
}
