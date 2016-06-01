#include <stdint.h>
#include <Regexp.h>
#include <Arduino.h>

// é rapidão:
bool compara(const String cte, char buf[], uint8_t i) {
  bool ret = false;
  if (i > 0) {
    if (String(buf).indexOf(cte) != -1) {
      ret = true;
    } // if(String(bufSer).indexOf(F("HTTP/1.1 200 OK")) != -1)
    else {
      ret = false;
    } // else
  } // if (i > 0)
  else {
    ret = false;
  } // else
  return ret;
}
  /*
    bool compara(const String cte, char buf[], uint8_t i) {
    bool ret = false;
    char *convStr;
    uint8_t len;
    MatchState ms;
    uint8_t result;

    if (i > 0) {
      len = cte.length() + 1;
      convStr = (char *)malloc(len);
      if (convStr == NULL) {
        Serial.print(F("ERRO: malloc "));
        Serial.print(len);
        //Serial.println(F("bytes em compara()"));
        //Serial.println(F("Reinicie :("));
        while (1);
      } // if (convStr == NULL)
      else {
        cte.toCharArray(convStr, len);

        ms.Target(buf);
        result = ms.MatchCount(convStr);
        if (result == 1) {
          ret = true;

        } // if (result == 1)
        else {
          ret = false;
        } // else
        free(convStr);
      } // else
    } // if (i > 0)
    else {
      ret = false;
    } // else
    return ret;
    }
  */
