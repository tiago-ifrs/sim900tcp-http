#include <SoftwareSerial.h>
void clearBuffer(char buffer[], uint8_t count)              // function to clear buffer array
{
  for (int i = 0; i < count; i++)
  //for (int i = 0; i < _SS_MAX_RX_BUFF; i++)
  {
    buffer[i] = '\0'; // clear all index of array with command NULL
  }
}
