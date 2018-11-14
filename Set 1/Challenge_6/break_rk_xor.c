#include "break_rk_xor.h"
#include "base64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t keysize;
  double avg_distance;
} result;

result *find_keysize(const char *str, size_t len);
size_t hamming_distance(const char *s1, const char *s2, size_t len);
void sort_keysizes(result *r, size_t n);
void swap(result *a, result *b);

int main(void) {
  char *b64d;
  size_t dst_len, enc_len;
  size_t keysizes[3];
  enc_len = strlen((char *)enc_str);
  b64d = base64_decode((char *)enc_str, enc_len, &dst_len);
  result *r = find_keysize(b64d, dst_len);
  sort_keysizes(r, 38);
  for (size_t i = 0; i < 3; i++) {
    keysizes[i] = r[i].keysize;
    printf("keysizes[i] = %zu\n", keysizes[i]);
  }
  free(r);

  free(b64d);

  return 0;
}

result *find_keysize(const char *str, size_t len) {
  result *r = malloc(39 * sizeof(*r));
  size_t idx, hd, KEYSIZE = 2, block_num = 0;
  const char *a = str;
  while (KEYSIZE != 41) {
    size_t ops = 0, hds = 0, pos = 0;
    do {
      char *block1 = malloc(KEYSIZE * sizeof(char));
      char *block2 = malloc(KEYSIZE * sizeof(char));
      for (idx = 0; idx < KEYSIZE; idx++) {
        if (pos + KEYSIZE > len)
          break;
        block1[idx] = a[pos];
        pos++;
      }

      for (idx = 0; idx < KEYSIZE; idx++) {
        if (pos + KEYSIZE > len)
          break;
        block2[idx] = a[pos];
        pos++;
      }
      hd = hamming_distance(block1, block2, KEYSIZE);
      free(block1);
      free(block2);
      hd /= KEYSIZE;
      hds += hd;
      ops++;
    } while (pos + KEYSIZE <= len);
    r[block_num].keysize = KEYSIZE;
    r[block_num].avg_distance = (double)hds / (double)ops;
    block_num++;

    KEYSIZE++;
  }

  return r;
}

/* Bubble sort O(n^2) */
void sort_keysizes(result *r, size_t n) {
  int i, j = n, s = 1;
  while (s) {
    s = 0;
    for (i = 1; i < j; i++) {
      if (r[i].avg_distance < r[i - 1].avg_distance) {
        swap(&r[i], &r[i - 1]);
        s = 1;
      }
    }
    j--;
  }
}

void swap(result *a, result *b) {
  result temp = *b;
  *b = *a;
  *a = temp;
}

size_t hamming_distance(const char *s1, const char *s2, size_t len) {
  unsigned char val;
  size_t dist = 0;
  for (size_t i = 0; i < len; i++) {
    val = s1[i] ^ s2[i];
    while (val) {
      ++dist;
      val &= val - 1;
    }
  }
  return dist;
}
