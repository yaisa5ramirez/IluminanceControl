#include "readSerial.h"
#include <arduino.h>
extern float xref;
readSerial::readSerial() {}


bool readSerial::recvData(bool newData) {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char endMarker = ',';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedCommand[ndx] = rc;
      ndx++;
      if (ndx >= 32) {  // reset the index
        ndx = 32 - 1;
      }
    } else {
      receivedCommand[ndx] = '\0';  // terminate the string
      recvInProgress = false;
      ndx = 0;
      newData = true;
    }
  }
  return newData;
}

void readSerial::parseData() {
  strcpy(tempChars, receivedCommand);
  char command1 = tempChars[1];
  char command2 = tempChars[2];
  switch (command1) {
    case 'd':       // Set directly the duty cycle of luminaire i to val. d <i> <val>’           
      luminaire = command2;
      command3 = String(tempChars[3]) + String(tempChars[4]) + String(tempChars[5]) + String(tempChars[6]);
      newDutyCycle = command3.toInt();
      if ((newDutyCycle > 0) && (newDutyCycle < 4095)) resp = "ack";
      else resp = "err";
      break;

    case 'g':  
      switch (command2) {
        case 'd':  // Get current duty cycle of luminaire i
          luminaire = tempChars[3];  
          bla = 8.2;
          resp = String('d') + String(luminaire) + String(bla, 0);
          break;

        case 'r':  // Get current illuminance reference of luminaire i
          luminaire = tempChars[3];  
          bla = 7.2;
          resp = String('r') + String(luminaire) + String(bla, 0);
          break;

        case 'l': // Measure the illuminance of luminaire i 'g l <i>’
          luminaire = tempChars[3];  
          bla = 6.2;
          resp = String('l') + String(luminaire) + String(bla, 0);
          break;
      }
      break;

    case 'r':  // Set the illuminance reference of luminaire i ‘r <i> <val>’
      luminaire = command2;
      command3 = String(tempChars[3]) + String(tempChars[4]);
      xref = command3.toFloat();
      if ((xref > 0) && (xref < 40)) resp = "ack";
      else resp = "err";
      break;

    default:
      // Handle unexpected command
      break;
  }

  tempChars[32];
  Serial.println(resp);
}
