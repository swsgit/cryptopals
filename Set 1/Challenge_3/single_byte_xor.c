#include "hex2str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char hex_str[] =
    "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

static const unsigned char keymap[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz .?!';:,";

char *single_xor(const unsigned char *str, unsigned char key, size_t len);
size_t strscore(const unsigned char *str, const unsigned char *keymap,
                size_t keymap_len, size_t str_len);

int main(void) {
  size_t score;
  size_t certain;
  char *dec;
  size_t dst_len;
  size_t hex_len = strlen((char *)hex_str);
  size_t keymap_len = strlen((char *)keymap);
  unsigned char key = 0x00;
  char *str = hex_to_string(hex_str, hex_len, &dst_len);

  /* uint8_t max = 2^8 - 1 = 255 */
  for (size_t i = 0; i < 255; i++, key++) {
    dec = single_xor((unsigned char *)str, key, dst_len);
    score = strscore((unsigned char *)dec, keymap, keymap_len, dst_len);
    certain = (score + 1 >= dst_len) ? 1 : 0;
    if (certain) {
      print_hex(dec, dst_len);
    }
    free(dec);
  }

  free(str);

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
