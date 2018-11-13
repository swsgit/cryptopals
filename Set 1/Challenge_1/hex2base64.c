#include "base64.h"
#include "hex2str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned char hex_str[] =
    "49276d206b696c6c696e6720796f757220627261696e206c696b6"
    "5206120706f69736f6e6f7573206d757368726f6f6d";

int main(void) {
  size_t hex_len;
  size_t enc_len;
  char *str = hex_to_string(hex_str, strlen((char *)hex_str), &hex_len);
  char *enc = base64_encode(str, strlen(str), &enc_len);

  printf("%s %zu\n%s %zu\n%s %zu", hex_str, strlen((char *)hex_str), str,
         hex_len, enc, enc_len);
  free(str);
  free(enc);

  return 0;
}
