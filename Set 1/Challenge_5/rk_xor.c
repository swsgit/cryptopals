#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned char str[] =
    "Burning 'em, if you ain't quick and nimble\nI go crazy when "
    "I hear a cymbal";
const unsigned char key[] = "ICE";
const unsigned char expected[] =
    "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c"
    "2a26226324272765272"
    "a282b2f20430a652e2c652a3124333a653e2b2027630c692b2028316"
    "5286326302e27282f";

unsigned char *rk_xor(const unsigned char *src, const unsigned char *key,
                      size_t src_len, size_t key_len);

int main(void) {
  size_t str_len, key_len;
  unsigned char *str_dec;
  key_len = strlen((char *)key);
  str_len = strlen((char *)str);

  str_dec = rk_xor(str, key, str_len, key_len);

  printf("GOT: ");
  for (size_t i = 0; i < str_len; i++) {
    printf("%02hhx", str_dec[i]);
  }
  printf("\nEXP: %s", expected);

  free(str_dec);

  return 0;
}

unsigned char *rk_xor(const unsigned char *src, const unsigned char *key,
                      size_t src_len, size_t key_len) {
  size_t i = 0, j = 0;
  unsigned char *dst;
  dst = malloc(src_len * sizeof(*dst) + 1);
  for (i = 0; i < src_len; i++) {
    dst[i] = src[i] ^ key[j];
    if ((j + 1) == key_len)
      j = 0;
    else
      j++;
  }
  dst[src_len + 1] = '\0';

  return dst;
}