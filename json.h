#ifndef _JSON_H
#define _JSON_H

char *criarJson(int intervaloEntreAmostras, int quantidadeSensores);

const char leituraJson[] PROGMEM = "{\"intervaloEntreAmostras\": %d, \"quantidadeSensores\":%d}";
#endif

