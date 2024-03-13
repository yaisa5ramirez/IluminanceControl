#include "readSerial.h"
/*
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char endMarker = ',';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      //Serial.println("State of the command");
      //Serial.println(recievedCommand);
      recievedCommand[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {  // reset the index
        ndx = numChars - 1;
      }
    } else {
      recievedCommand[ndx] = '\0';  // terminate the string
      recvInProgress = false;
      ndx = 0;
      newData = true;
    }
  }
}


void parseData() {
  char comando1 = tempChars[0];
  char comando2 = tempChars[1];
  char comando3 = tempChars[2];

  switch (comando1) {
    case 'd':                       // Set directly the duty cycle of luminaire i to val. d <i> <val>’
      iluminaire = comando2 - '0';  // Convert char to int
      newdutycycle = comando3 - '0';
      // return ack or err
      break;

    case 'g':
      switch (comando2) {  

        case 'd': // Get current duty cycle of luminaire i
        iluminaire = comando3 - '0'; // Convert char to int
        // return current duty cycle
          break;
        
        case 'r': // Get current illuminance reference of luminaire i
        iluminaire = comando3 - '0'; // Convert char to int
        // return current iluminance reference in lux
          break;

        case 'l': // // Measure the illuminance of luminaire i 'g l <i>’
        iluminaire = comando3 - '0'; // Convert char to int
        // return current iluminance in lux
          break;

              }

      break;

    case 'r': // Set the illuminance reference of luminaire i ‘r <i> <val>’
      iluminaire = comando2 - '0';  // Convert char to int
      newreference = comando3 - '0';
      break;

    default:
      // Handle unexpected command
      break;
  }
}

*/
