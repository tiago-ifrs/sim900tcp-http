#include <SoftwareSerial.h>
#include <Regexp.h>
#include "hash.h"
#include "json.h"
#include "tcp.h"
#include "seriais.h"
#include "gprs.h"
#include "constantes.h"
#include "clearBuffer.h"
#include "compara.h"

typedef enum {OK_AT, CIPSHUT, SHUT_OK, PIC_I, LER_PIC_I, PIC_Q, LER_PIC_Q, JSON, LER_SENSORES, TCP_START, TCP_START_OK, TCP_START_CONNECT_OK, CIFSR, REGEX_IP, CIICR,
              START_OK, CSTT, INITIAL_OK, CIPSEND, PROMPT, POST, FIM, SEND_OK, LER_JSON, LER_HTTP, LER_GLASSFISH, TCP_CLOSED
             } EstadosGprs;
EstadosGprs estG;

typedef enum {SIM, MODULO_PIC} EstadosTerminal;
EstadosTerminal estT;

//prototype
void mostraSerial(void);
void tcpFechar(void);

void lerJson(void);
void tcpPost(String entidade, char root[]);

const char formatoFloat[] PROGMEM = "%d.%d";
const char formatoDado[] PROGMEM = "{\"idLeitura\":%s,\"sensor\":%d,\"valor\":%d.%d, \"hash\":\"%s\"}";

#define BPS 9600

void setup()
{
  estG = OK_AT;
  estT = SIM;

  char c;

  /* Definições de tempo do código PIC
      #define     TEMPO_1_SEGUNDO     18
      #define     TEMPO_10_SEGUNDOS   180
      #define     TEMPO_30_SEGUNDOS   540
      #define     TEMPO_1_MINUTO      1080
      #define     TEMPO_10_MINUTOS    10800
      #define     TEMPO_30_MINUTOS    32400
      #define     TEMPO_1_HORA        64800
  */

  GPRS.begin(BPS);
  Serial.begin(BPS);
  PIC.begin(2400);

  while (!Serial);
  Serial.println(F("Teste"));

  Serial.println(F("ligando..."));

  GPRS.println(F("AT"));
}

void loop()  {
  char *hash;
  static int tam = 0;
  static int qSens = 0;
  static int intAmostra = 0;

  //static char jsonLeitura[57];
  static char *jsonLeitura;
  static char jsonIdLeitura[136]; //era global
  //static uint8_t sensor[5];
  static float valor[5];
  //static uint8_t sensor;
  static uint8_t sensorAtual = 1;
  //static float valor;

  char c;

  char cBufSer;
  char cBufPic = '\0';
  static uint8_t iBufSer = 0;
  static uint8_t iBufPic = 0;
  static uint8_t iBufJson = 0;

  uint8_t igual;
  //#define BUFF_SER _SS_MAX_RX_BUFF
  //static char bufSer[BUFF_SER ];
  char bufSer[_SS_MAX_RX_BUFF];
  static char bufPic[_SS_MAX_RX_BUFF];
  static char bufDados[220];

  static bool comeco = false;
  bool leituraValida = false;
  String stPic;

  switch (estT) {
    case SIM:
      /*
        SoftwareSerial: listen()

        Description

        Enables the selected software serial port to listen.
        Only one software serial port can listen at a time; data that arrives for other
        ports will be discarded. Any data already received is discarded during the call
        to listen() (unless the given instance is already listening).
      */
      GPRS.listen(); // a função já faz o teste se já está ouvindo
      /*
        SoftwareSerial: available()

        Description
        Get the number of bytes (characters) available for reading from a software serial
        port. This is data that's already arrived and stored in the serial receive buffer.
      */
      //while (GPRS.available()) {
      if (GPRS.available()) {
        cBufSer = GPRS.read();
        if (cBufSer != -1) {
          bufSer[iBufSer++] = cBufSer;
          leituraValida = true;
          Serial.print(cBufSer);
          delayMicroseconds(8); // tem que ter pra ler direito (json)
          //bufSer[iBufSer] = cBufSer;
          //iBufSer++;
        }
      } // if (GPRS.available())
      break;
    case MODULO_PIC:
      PIC.listen();
      while (PIC.available()) {
        //if (PIC.available()) {
        delayMicroseconds(2500); // tem que ter pra ler direito
        cBufPic = PIC.read();
        if (cBufPic != -1) {
          if (cBufPic != '\0') {
            if (cBufPic >= '\n') { // ascii 0
              bufPic[iBufPic++] = cBufPic;
              Serial.print(cBufPic);

              //iBufPic++;
            } // if (cBufPic >= '\n')
          } // if(cBufPic != '\0') {
        } // if (cBufPic != -1)
        leituraValida = true;
      } // if??? (PIC.available())
      break;
  }
  switch (estG) {
    case OK_AT:
      if (compara(OK, bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = CIPSHUT;
      } // if (compara(OK, bufSer, iBufSer))
      break; //case OK_AT
    case CIPSHUT:
      //  Deactivate GPRS PDP Context
      GPRS.println(F("AT+CIPSHUT"));
      estG = SHUT_OK;
      break; //case CIPSHUT
    case SHUT_OK:
      if (compara(F("SHUT OK"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = CSTT;
      } // if (compara(F("SHUT OK"), bufSer, iBufSer))
      break; //case SHUT_OK
    case CSTT:
      // Start Task And Set APN, USER, NAME, PASSWORD
      GPRS.println(F("AT+CSTT=\"claro.com.br\""));
      estG = INITIAL_OK;
      break;
    case INITIAL_OK:
      if (compara(OK, bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = CIICR;
      } // if (compara(OK, bufSer, iBufSer))
      else {
        if (compara(F("ERROR"), bufSer, iBufSer)) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = CIPSHUT;
        } // if (compara(F("ERROR"), bufSer, iBufSer))
      } // else
      break; // case INITIAL_OK
    case CIICR:
      // Bring Up Wireless Connection With GPRS Or CSD
      GPRS.println(F("AT+CIICR"));
      estG = START_OK;
      break;
    case START_OK:
      if (compara(OK, bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = CIFSR;
      }  // if (compara(OK, bufSer, iBufSer))
      else {
        if (compara(F("ERROR"), bufSer, iBufSer)) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = CIPSHUT;
        }  // if (compara(F("ERROR"), bufSer, iBufSer))
      }//else
      break; // case START_OK:
    case CIFSR:
      // Get Local IP Address
      GPRS.println(F("AT+CIFSR"));
      estG = REGEX_IP;
      break;
    case REGEX_IP:
      if (iBufSer > 0) {
        MatchState ms;
        ms.Target(bufSer);
        char result = ms.MatchCount ("[.]");
        if (result == 3) { // condição para executar próximo comando
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = PIC_I;
          estT = MODULO_PIC;
        } // if (result == 3)
      } // if(iBufSer > 0)
      break; // case REGEX_IP:
    case PIC_I:
      PIC.write('I');
      estG = LER_PIC_I;
      break; // case PIC_I
    case LER_PIC_I:
      if (cBufPic == '\n') {
        intAmostra = atoi(bufPic);
        Serial.print(F("intervalo amostra:"));
        Serial.println(intAmostra);
        clearBuffer(bufPic, iBufPic);
        iBufPic = 0;
        estG = PIC_Q;
      } // if(cBufPic == '\n')
      break; // case LER_PIC_I
    case PIC_Q:
      PIC.write('Q');
      estG = LER_PIC_Q;
      break; // case PIC_Q
    case LER_PIC_Q:
      if (cBufPic == '\n') {
        qSens = atoi(bufPic);
        Serial.print(F("quantidade sensores:"));
        Serial.println(qSens);
        clearBuffer(bufPic, iBufPic);
        iBufPic = 0;
        estG = JSON;
      } // if(cBufPic == '\n')
      break; // case LER_PIC_Q
    case JSON:
      Serial.println(F("enviar leitura"));
      jsonLeitura = criarJson(intAmostra, qSens); // jsonLeitura é static
      Serial.println(F("jsonLeitura"));
      Serial.println(jsonLeitura);
      estT = SIM;
      estG = TCP_START;
      break; // case JSON
    case LER_SENSORES:// depois do 1º envio, entra neste estado
      char *sls;
      uint8_t sen;
      if (leituraValida == true) {
        for (uint8_t i = 1; i <= qSens; i++) {
          sls = &bufPic[7 * (i - 1)];
          stPic = String(sls);
          Serial.print(F("sls:"));
          Serial.println(sls);
          //stPic = String(bufPic);
          igual = stPic.indexOf("=") + 1;
          sen = stPic.substring(0, igual).toInt();
          valor[sen] = stPic.substring(igual).toFloat();
          //        bufPic = bufPic[i*5];
        } // for (uint8_t i = 1; i < qSens; i++)

        estT = SIM;
        estG = TCP_START;
      } // if(leituraValida == true)
      break; // case LER_SENSORES
    case TCP_START:
      tcpStart();
      estG = TCP_START_OK;
      break; // case TCP_START
    case TCP_START_OK:
      if (compara(OK, bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = TCP_START_CONNECT_OK;
      }
      break; // case TCP_START_OK
    case TCP_START_CONNECT_OK:
      if (compara(F("CONNECT OK"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = CIPSEND;
      } // if (compara(F("CONNECT OK"), bufSer, iBufSer))
      else {
        if (compara(F("+PDP: DEACT"), bufSer, iBufSer)) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = CIICR;
        } // if (compara(F("+PDP: DEACT"), bufSer, iBufSer))
        else {
          if (compara(F("CONNECT FAIL"), bufSer, iBufSer)) {
            clearBuffer(bufSer, iBufSer);
            iBufSer = 0;
            estG = CIICR;
          } // if (compara(F("CONNECT FAIL"), bufSer, iBufSer))
        } // else
      } // else
      break; // case TCP_START_CONNECT_OK
    //} // else if(tam == 0)
    case CIPSEND:
      GPRS.println(F("AT+CIPSEND"));
      estG = PROMPT;
      break; //case CIPSEND:
    case PROMPT:
      if (compara(F(">"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = POST;
      }
      break; // case PROMPT
    case POST:
      if (tam == 0) // posta e espera resposta do servidor com o array de leitura
      {
        tcpPost(F("leitura"), jsonLeitura);
        estG = FIM;
      } // if (tam == 0)
      else { // array de dados
        //for (int i = 0; i < qSens; i++) {

        //Serial.println(F("hash?"));

        //enviarDados(jsonIdLeitura, hash, sensor, valor);

        int conv = valor[sensorAtual] * 10;
        hash = calculaHash(sensorAtual, valor[sensorAtual]);

        sprintf_P(bufDados, formatoDado, jsonIdLeitura, sensorAtual, conv / 10, conv % 10, hash);

        Serial.println(F("buffer em enviar dados"));
        Serial.println(bufDados);

        tcpPost(F("dado"), bufDados);
        free(hash);
        //} // for (int i = 0; i < qSens; i++)
        estG = FIM;
      } // else
      break; //case POST
    case FIM:
      if (cBufSer != -1) {
        if (iBufSer != 0) {
          //Serial.print(cBufSer);
          if (cBufSer == (char)0x1a) {
            Serial.println(F("fim"));

            clearBuffer(bufSer, iBufSer);
            iBufSer = 0;
            estG = SEND_OK;
          } //if (cBufSer == (char)0x1a)
          else {
            if (iBufSer == _SS_MAX_RX_BUFF) { // limpar o buffer para continuar a leitura
              clearBuffer(bufSer, iBufSer);
              iBufSer = 0;
            } // if (iBufSer == _SS_MAX_RX_BUFF)
          } // else
        } // if(iBufSer == 0)
      } // if (cBufSer != -1) => leitura inválida
      break; // case FIM
    case SEND_OK:
      if (compara(F("SEND OK"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        if (tam == 0) {
          estG = LER_JSON;
        } // if (tam == 0)
        else {
          estG = LER_HTTP;

        } // else

      } // if (compara(F("SEND OK"), bufSer, iBufSer))
      else {
        if (compara(F("CLOSED"), bufSer, iBufSer)) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = CSTT;
        } // if (compara(F("CLOSED"), bufSer, iBufSer))
      } // else
      break; // case SEND_OK
    case LER_HTTP:
      if (compara(F("HTTP/1.1 200 OK"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        estG = LER_GLASSFISH;
      } // if (compara(F("HTTP/1.1 200 OK"), bufSer, iBufSer))
      else {
        if (compara(F("CLOSED"), bufSer, iBufSer)) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          estG = CSTT;
        } // if (compara(F("CLOSED"), bufSer, iBufSer))
      } // else
      //estG = LER_GLASSFISH;
      break; // case LER_HTTP
    case LER_GLASSFISH:
      if (leituraValida) { // teste para não ocupar muito a leitura
        if (iBufSer == _SS_MAX_RX_BUFF) {
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
        } // if (iBufSer == _SS_MAX_RX_BUFF)
      } // if (leituraValida)
      else {
        if (compara(OK, bufSer, iBufSer)) { // porque essa função é demorada
          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          tcpFechar();
          estG = TCP_CLOSED;
        } // if (compara(OK, bufSer, iBufSer))
      } // else
      break; // case LER_GLASSFISH
    case LER_JSON:
      //if (cBufSer != -1) {
      if (iBufSer != 0) {
        //Serial.print(cBufSer);
        if (cBufSer == '{') {
          comeco = true;
        }
        if (comeco == true) {
          //jsonIdLeitura[iBufJson] = cBufSer;
          //iBufJson++;
          jsonIdLeitura[iBufJson++] = cBufSer;
        }
        if (cBufSer == '}') {
          Serial.println(F("fimLerJson"));
          //clearBuffer(bufSer, iBufSer);
          //iBufSer = 0;

          //lerJson();
          //não tem mais malloc(): free(jsonLeitura); // libera array criado para enviar a hash
          //
          Serial.println(F("json Recebido:"));
          Serial.println(jsonIdLeitura);

          tam = strlen(jsonIdLeitura);
          Serial.println(F("enviou leitura"));

          clearBuffer(bufSer, iBufSer);
          iBufSer = 0;
          tcpFechar();
          estG = TCP_CLOSED;
        } //if (cBufSer == '}')
        else {
          if (iBufSer == _SS_MAX_RX_BUFF) { // limpar o buffer para continuar a leitura
            //Serial.write(bufSer, iBufSer);
            clearBuffer(bufSer, iBufSer);
            iBufSer = 0;
          } // if (iBufSer == _SS_MAX_RX_BUFF)
        } // else
      } // if(iBufSer == 0)
      //} // if (cBufSer != -1) => leitura inválida
      break; // case LER_JSON
    case TCP_CLOSED:
      if (compara(F("CLOSE OK"), bufSer, iBufSer)) {
        clearBuffer(bufSer, iBufSer);
        iBufSer = 0;
        if (iBufPic == 0) {
          estG = LER_SENSORES;
          estT = MODULO_PIC;
        } // if(iBufPic == 0)
        else {
          if (sensorAtual < qSens) {
            estG = TCP_START;
            sensorAtual++;
          } // if(sensorAtual < qSens)
          else {
            if (sensorAtual == qSens) {
              clearBuffer(bufPic, iBufPic);
              iBufPic = 0;
              sensorAtual = 1;
              estG = LER_SENSORES;
              estT = MODULO_PIC;
            } // if (sensorAtual == qSens)
          } // else
        } //else
      } //if (compara(F("CLOSE OK"), bufSer, iBufSer))
      break; //case TCP_CLOSED:
  } //switch est
}
