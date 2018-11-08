#include "hex2str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *hex_to_string(const unsigned char *src, size_t len, size_t *dst_len) {
  size_t i = 0, j = 0;
  char hex[3];
  char c;
  char *dst = malloc((len / 2) * sizeof(char) + 1);
  *dst_len = len / 2;
  for (; i < len && j < *dst_len; i += 2, j++) {
    sprintf(hex, "%c%c", src[i], src[i + 1]);
    hex[2] = '\0';
    c = strtol(hex, NULL, 16);
    dst[j] = c;
  }
  dst[j] = '\0';

  return dst;
}

void print_hex(const char *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    printf("%c", src[i]);
  }
  printf("\n");
}