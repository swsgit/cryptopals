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

static const unsigned char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const char *src, size_t len, size_t *dst_len) {
  size_t i, j, rem, padding;
  const char *a = src;
  rem = len % 3;
  if (rem != 0) {
    padding = 4;
  } else {
    padding = 0;
  }

  *dst_len = 4 * (len / 3) + padding;
  char *dst = malloc(*dst_len * sizeof(char) + 1);
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