#include "break_rk_xor.h"
#include "base64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYLEN 41

int main(void) {
  unsigned char test_key = 0x00;
  char b64e[3836];
  errno_t err;
  FILE *fp;
  char *dec;
  key *keys;
  size_t dst_len, sample_size, score, i, j;

  err = fopen_s(&fp, "6.txt", "rb");
  if (err) {
    perror("Failed to open file");
  }
  fread(b64e, sizeof *b64e, 3836, fp);

  char *b64d = base64_decode(b64e, 3836, &dst_len);
  keys = get_keys(b64d, dst_len);
  sort_keys(keys, MAX_KEYLEN);
  for (size_t i = 0; i < 3; i++) {
    printf("keysize: %zu distance: %lf\n", keys[i].keysize, keys[i].distance);
  }

  char *sample = malloc((dst_len / 16) * sizeof(char));
  printf("%zu\n", dst_len);
  for (i = 0, j = 0; j + 16 < dst_len; i++, j += 16) {
    sample[i] = b64d[j];
  }
  sample_size = i;

  for (i = 0; i < 255; i++, test_key++) {
    dec = sk_xor(test_key, sample, sample_size);
    score = strscore((char *)dec, keymap, strlen((char *)keymap), sample_size);
    if (score + 55 > sample_size) {
      printf("key: 0x%01x '%c' score: %zu size: %zu\n", test_key, test_key,
             score, sample_size);
    }
  }

  free(dec);
  free(sample);
  free(keys);
  free(b64d);

  return 0;
}

size_t strscore(char *str, unsigned char *keymap, size_t keymap_len,
                size_t str_len) {
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

key *get_keys(char *src, size_t src_len) {
  double avg_distance;
  size_t j = 0, i = 0, distance = 0, num_blocks = 0;
  char *block1, *block2;
  key *k = malloc(MAX_KEYLEN * sizeof(key));
  for (size_t key_len = 1; key_len < MAX_KEYLEN; key_len++) {
    block1 = malloc(key_len * sizeof(char));
    block2 = malloc(key_len * sizeof(char));
    while (j + key_len < src_len) {
      while (i < key_len) {
        block1[i] = src[j];
        block2[i] = src[j + key_len];
        j++;
        i++;
      }
      i = 0;
      distance += hamming_distance(block1, block2, key_len);
      num_blocks++;
    }
    free(block1);
    free(block2);
    distance /= key_len;
    avg_distance = (double)distance / (double)num_blocks;
    k[key_len - 1].distance = avg_distance;
    k[key_len - 1].keysize = key_len;
    distance = 0;
    num_blocks = 0;
    j = 0;
  }

  return k;
}

/* Bubble sort O(n^2) */
void sort_keys(key *r, size_t n) {
  int i, j = n, s = 1;
  while (s) {
    s = 0;
    for (i = 1; i < j; i++) {
      if (r[i].distance < r[i - 1].distance) {
        swap(&r[i], &r[i - 1]);
        s = 1;
      }
    }
    j--;
  }
}

void swap(key *a, key *b) {
  key temp = *b;
  *b = *a;
  *a = temp;
}

size_t hamming_distance(char *s1, char *s2, size_t len) {
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

char *rk_xor(size_t keysize, size_t ctxt_len, char *key, char *ctxt) {
  register size_t i = 0, j = 0;
  char *enc = malloc(ctxt_len * sizeof(*enc));
  while (i < ctxt_len) {
    if (j > keysize) {
      j = 0;
    }
    enc[i] = ctxt[i] ^ key[j];
    i++;
    j++;
  }

  return enc;
}

char *sk_xor(char key, char *ctxt, size_t ctxt_len) {
  register size_t i = 0;
  char *enc = malloc(ctxt_len * sizeof(*enc));
  while (i < ctxt_len) {
    enc[i] = ctxt[i] ^ key;
    i++;
  }

  return enc;
}