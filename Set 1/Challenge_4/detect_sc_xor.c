#define _CRT_SECURE_NO_WARNINGS
#include "hex2str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char keymap[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz .?!';:,";

char *single_xor(const unsigned char *str, unsigned char key, size_t len);
size_t strscore(const unsigned char *str, const unsigned char *keymap,
                size_t keymap_len, size_t str_len);
void remove_newline(char *str, size_t str_len);

int main(void) {
  size_t score;
  size_t certain;
  size_t line_number = 1;
  char *dec;
  char *str;
  size_t dst_len;
  size_t hex_len;
  size_t keymap_len = strlen((char *)keymap);
  unsigned char key = 0x00;
  unsigned char buf[62];

  FILE *fp = fopen("4.txt", "r");
  if (!fp) {
    perror("File opening failed");
    return EXIT_FAILURE;
  }

  while (fgets((char *)buf, sizeof(buf), fp) != NULL) {
    key = 0x00;
    hex_len = strlen((char *)buf);
    remove_newline((char *)buf, hex_len);
    hex_len = strlen((char *)buf);
    str = hex_to_string(buf, hex_len, &dst_len);
    for (size_t i = 0; i < 255; i++, key++) {
      dec = single_xor((unsigned char *)str, key, dst_len);
      score = strscore((unsigned char *)dec, keymap, keymap_len, dst_len);
      certain = (score + 1 >= dst_len) ? 1 : 0;
      if (certain) {
        printf("Line %zu: %s ^ %c (0x%x)\n", line_number, buf, key, key);
        printf("%s\n", dec);
      }
      free(dec);
    }
    free(str);
    line_number++;
  }
  fclose(fp);

  return 0;
}

char *single_xor(const unsigned char *str, unsigned char key, size_t len) {
  size_t i;
  char *dst = malloc(len * sizeof(char) + 1);
  for (i = 0; i < len; i++) {
    dst[i] = str[i] ^ key;
  }
  dst[i] = '\0';

  return dst;
}

size_t strscore(const unsigned char *str, const unsigned char *keymap,
                size_t keymap_len, size_t str_len) {
  size_t score = 0;
  for (size_t i = 0; i < keymap_len; i++) {
    for (size_t j = 0; j < str_len; j++) {
      if (str[j] == keymap[i]) {
        score++;
      }
    }
  }

  return score;
}

void remove_newline(char *str, size_t str_len) {
  for (size_t i = 0; i < str_len; i++)
    if (str[i] == '\n') {
      str[i] = '\0';
    }
}