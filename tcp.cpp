//#include <avr/pgmspace.h>
#include <Arduino.h>
#include <WString.h>
#include "insiste.h"
#include "seriais.h"
#include "constantes.h"
#include "gprs.h"

void tcpFechar(void) {
  String esp;
  char c;
  Serial.println(F("fechando tcp..."));
  //mostraSerial();
  /*
    do {
    lerGprs();
    } while (strlen(bufGprs) == 0);
  */
  GPRS.println(F("AT+CIPCLOSE=1"));//quick close

  //mostraSerial();
  //lerGprs();
  /*
    if (String(bufGprs).indexOf(F("AT+CIPCLOSE")) == -1) {
      lerEspera(F("AT+CIPCLOSE"));
    }
  */
  /*
    delay(5000);

    //mostraSerial();

  */
  /*
    lerGprs();
    if (String(bufGprs).indexOf(F("CLOSE OK")) == -1) {
    lerEspera(F("CLOSE OK"));
    }
  */
  /*
  do {
    //delay(1);
    if (GPRS.available()) {
      c = GPRS.read();
      if (c >= 0x20) {
        Serial.print(c);
        esp += c;
      }
      if (c == '\n') {
        Serial.print(c);
      }
    }
  } while (esp.indexOf(F("CLOSE OK")) == -1);
  Serial.println();
  */
}

void tcpStart() {
  String state;
  Serial.println(F("Start:"));
/*
  do {
    GPRS.println(F("AT+CIPSTATUS"));
    lerEspera(F("STATE:"));
    state = String(bufGprs);
    while ((state.indexOf(F("PDP DEACT")) != -1) ||
           (state.indexOf(F("CONNECT FAIL"))) != -1) {
      tcp_conectar();
    }

    GPRS.println(F("AT+CGREG=1"));//enable network registration unsolicited result code
    lerEspera(OK);
    */
    GPRS.print(F("AT+CIPSTART=\"TCP\",\""));
    GPRS.print(ip);
    GPRS.println(F("\",\"8080\""));
    //lerEspera(F("OK"));
    //delay(500);
    /*
    do {
      lerGprs();
    } while (strlen(bufGprs) == 0);
    Serial.print(bufGprs);
    */
    /*if (String(bufGprs).indexOf(F("CONNECT OK")) == -1) {
      lerEspera(F("CONNECT OK"));
      }*/
 // } while (String(bufGprs).indexOf(F("CONNECT FAIL")) != -1);
}

void tcpPost(String entidade, char root[]) {
  int tam = strlen(root);

    Serial.println(F("tcp post:"));
    //mostraSerial();
    /*
    GPRS.println(F("AT+CGREG?"));
    lerEspera(F("AT+CGREG?"));
    
    if (String(bufGprs).indexOf(F("+CGREG: 1,1")) != -1) {
      tcpStart();
    } else {
      do {
        tcp_conectar();
        GPRS.println(F("AT+CGREG?"));
        while ( GPRS.read() != '\n');
        lerGprs();
      } while (String(bufGprs).indexOf(F("+CGREG: 1,1")) != -1);
      tcpStart();
    }
*/
    //GPRS.println(F("AT+CIPSEND"));

    /*
    while ( GPRS.read() != '\n');
    lerGprs();
    if (String(bufGprs).indexOf(F("ERROR")) != -1) {
      tcp_conectar();
      tcpStart();
    }
    

    lerEspera(F(">"));
    */

    //Começo do POST
    GPRS.print(F("POST /ServicoEnvioMysql/webresources/entidades."));
    
    GPRS.print(entidade);
    GPRS.println(F(" HTTP/1.1"));
    
    //lerEspera(F("HTTP"));

    GPRS.print(F("Host: "));
    GPRS.println(ip);
    
    //lerEspera(F("Host"));

    GPRS.println(F("Content-Type: application/json"));
    
    //lerEspera(F("json"));

    GPRS.print(F("Content-Length: "));
    
    GPRS.println(tam);
    
    //lerEspera(F("Content"));

    GPRS.println();
    
    GPRS.println(root);
    
    GPRS.println((char)0x1a);

    //Fim do POST
  }

