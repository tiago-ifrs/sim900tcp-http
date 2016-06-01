#include <SoftwareSerial.h>
#include <Arduino.h>
#include "seriais.h"
#include "gprs.h"
char bufGprs[_SS_MAX_RX_BUFF];
void resetGprs()
  {
    /*
    digitalWrite(9, LOW);
    delay(1000);
    digitalWrite(9, HIGH);
    delay(2000);
    digitalWrite(9, LOW);
    delay(3000);
    */
    digitalWrite(10, LOW);
    delay(1000);
    digitalWrite(10, HIGH);
    delay(1);
    digitalWrite(10, LOW);
    delay(3000);
  }
void lerGprs() {
  //char *l;
  char aux[_SS_MAX_RX_BUFF];
  //char *aux;
  char c = '\0';
  int count = 0;   // counter for buffer array

  for (int i = 0; i < _SS_MAX_RX_BUFF; i++) {
    bufGprs[i] = '\0';
  }
  //delay(250); //intervalinho
  /*
      aux = (char *)malloc(_SS_MAX_RX_BUFF);
      if (aux == NULL) {
        Serial.println(F("ERRO: malloc de aux em GRPSread == 64"));
        while (1);
      }
  */
  //Serial.println(F("lergprs:"));
  //while (!GPRS.available()); //esperar esvaziar o buffer da leitura anterior
  //delay(256);//delay 2,4,8,16(???) 96,128,250,500,1000(pára em cgatt)
  //while (strlen(bufGprs) == 0) {
  while (GPRS.available())         // reading data into char array
    //while (c != '\n')         // reading data into char array
  {
    //delay(2); //parou no close e lixo na tela
    delay(1); //parou no close,
    /*lixo na tela:*/
    //delayMicroseconds(850); //parou no close
    //delayMicroseconds(750); //parou no send ok
    //delayMicroseconds(500);//parou no start e no send ok
    c = GPRS.read();
    bufGprs[count] = c;
    //Serial.print(c);
    count++;

    if (c == -1) {
      count = 0;
      break; //erro de leitura
    }
    if (count == _SS_MAX_RX_BUFF) {
      bufGprs[count] = '\0';
      break; //já chega
    }
  }
  //}
  for (int i = count; i < _SS_MAX_RX_BUFF; i++) { //<=, pois  "<" imprime string com lixo (buffer overflow?), desconexão da serial, reinicio do arduino e incremento no id_leitura
    bufGprs[i] = '\0';
  }
  //Serial.println(F("lergprs fim"));

  return;
}

