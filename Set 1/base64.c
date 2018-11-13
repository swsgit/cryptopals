#include "base64.h"
#include <stdio.h>
#include <stdlib.h>

/*
        Base64 encoding reference table
 _______________________________________________
|       M       |       a       |       n       |
|0 1 0 0 1 1 0 1|0 1 1 0 0 0 0 1|0 1 1 0 1 1 1 0|
|_____T_____|_____W_____|_____F_____|_____u_____|

Concept:
    Base64 encoding converts three octets into four encoded characters.

Abstract:
    Combine the bits of three 8-bit words into one 24-bit word.
    Seperate the 24-bit word into four, 6-bit words.
    Length of encoded string is (4 * (len / 3)) + padding.
    Padding is determined by amount of bits left over after mod 3 divison.

Constants:
    0x03 = 00000011
    0x0f = 00001111
    0x3f = 00111111

// Shifting "M" right by 2 produces its last 6 bits.
// This binary value as a decimal is 19.
enc[i]   = 01001101 >> 2 = |00|010011 = b64_table[19] = T

// Trim off M's 2 last bits using AND 0x03.
// Then Shift left by 4 to create the 2 last bits of a 6 bit word.
// Shifting "a" right by 4 produces the last 4 bits.
// x OR y produces 22, the 6 bit word W in the encoding table.
enc[i+1] = |01|001101
           |00|000011 AND
       x = 00000001 << 4
         = 00010000
       y = 01100001 >> 4
         = 00000110
   x | y = |00|010110 = b64_table[22] = W

// Trim off a's last 4 bits using AND 0x0f.
// Then shift left by 2 to create last 4 bits of a 6 bit word.
// Shifting n right by 6 produces its last 2 bits.
// x OR y produces 5, the 6 bit word F in the encoding table.
enc[i+2] = |01|100001
           |00|001111 AND
       x = 00000001 << 2
         = 00000100
       y = 01101110 >> 6
           00000001
   x | y = |00|000101 = b64_table[5] = F

// Trim off n's first 6 bits using AND 0x3f
enc[i+3] = |01|101110
           |00|111111 AND
           |00|101110 = b64_table[46] = u
*/

static const unsigned char pr2six[256] = {
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64};

static const unsigned char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const char *src, size_t len, size_t *dst_len) {
  size_t i, j, rem, padding;
  register const char *a = src;
  register char *dst;
  rem = len % 3;
  if (rem != 0) {
    padding = 4;
  } else {
    padding = 0;
  }

  *dst_len = 4 * (len / 3) + padding;
  dst = malloc(*dst_len * sizeof(char) + 1);
  for (i = 0, j = 0; i < *dst_len && j < len; i += 4, j += 3) {
    dst[i] = b64_table[a[j] >> 2];
    dst[i + 1] = b64_table[((a[j] & 0x03) << 4) | (a[j + 1] >> 4)];
    dst[i + 2] = b64_table[((a[j + 1] & 0x0f) << 2) | (a[j + 2] >> 6)];
    dst[i + 3] = b64_table[a[j + 2] & 0x3f];
  }
  if (rem == 1) {
    dst[*dst_len - 2] = '=';
    dst[*dst_len - 1] = '=';
  } else if (rem == 2) {
    dst[*dst_len - 1] = '=';
  }

  dst[*dst_len] = '\0';

  return dst;
}

char *base64_decode(const char *src, size_t len, size_t *dst_len) {
  size_t i, j, padding;
  register char *dst;
  register const char *a = src;

  if (src[len - 2] == '=' && src[len - 1] == '=') {
    padding = 2;
  } else if (src[len - 1] == '=') {
    padding = 1;
  } else {
    padding = 0;
  }
  *dst_len = ((len / 4) * 3) - padding;
  dst = malloc(*dst_len * sizeof(*dst));

  for (i = 0, j = 0; i < *dst_len && j < len; i += 3, j += 4) {
    dst[i] = pr2six[(size_t)a[j]] << 2 | pr2six[(size_t)a[j + 1]] >> 4;
    dst[i + 1] = pr2six[(size_t)a[j + 1]] << 4 | pr2six[(size_t)a[j + 2]] >> 2;
    dst[i + 2] = pr2six[(size_t)a[j + 2]] << 6 | pr2six[(size_t)a[j + 3]];
  }
  if (padding == 1) {
    dst[i + 1] = pr2six[(size_t)a[j + 1]] << 4 | pr2six[(size_t)a[j + 2]] >> 2;
  } else if (padding == 2) {
    dst[i + 2] = pr2six[(size_t)a[j + 2]] << 6 | pr2six[(size_t)a[j + 3]];
  }
  dst[*dst_len] = '\0';

  return dst;
}