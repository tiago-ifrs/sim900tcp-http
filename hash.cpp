#include <MD5.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "hash.h"

char *calculaHash(int sensor, float valor) {
  char testeMd5[26];
  unsigned char *hash;
  char *md5str;
  int conv = valor * 10;

  sprintf_P(testeMd5, formatoHash, sensor, conv / 10, conv % 10);

  hash = MD5::make_hash(testeMd5);
  md5str = MD5::make_digest(hash, 16);

  free(hash);
  return md5str;
}
