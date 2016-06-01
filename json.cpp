#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "json.h"

 
char *criarJson(int intervaloEntreAmostras, int quantidadeSensores) {
  char *buf = (char *) malloc(57); // teve que ficar assim

  sprintf_P(buf, leituraJson, intervaloEntreAmostras, quantidadeSensores);
  return buf;
}

