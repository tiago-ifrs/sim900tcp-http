#include <WString.h>
#include <Arduino.h>
#include "seriais.h"
#include "gprs.h"
#include "constantes.h"

void insiste(String s, String esp, String forca) {
  //char *l;
  //char *r;

  do {
    GPRS.println(s);
    //lerEspera(s);
    delay(500);
    //while (!GPRS.available());
    //mostraSerial();
    //delay(1000);

    do {
      lerGprs();
    } while (strlen(bufGprs) == 0);

    Serial.print(F("insistindo: "));
    Serial.print(s);
    Serial.print(F("lido: "));
    Serial.write(bufGprs);
    Serial.print(F("-----"));

    //if (strstr(esp.c_str(), l) == NULL) {
    if (String(bufGprs).indexOf(esp) == -1) {
      Serial.print(F("forcando: "));
      Serial.println(forca);
      Serial.print(F("-----"));

      GPRS.println(forca);
      //delay(1000);
      //mostraSerial();

      lerGprs();

      //if (strstr(esp.c_str(), r) == NULL) {
      if (String(bufGprs).indexOf(esp) == -1) {
        Serial.println(F("1 minuto..."));
        delay(15000);
        Serial.println(F("passou"));
      }
    }
  }  while (String(bufGprs).indexOf(esp) == -1);

  Serial.println(EXIT);

  return;
}
