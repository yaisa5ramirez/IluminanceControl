#include "pidcontroller.h"

void readmySerial(char solicitud) {
  switch (solicitud) {
    case 'd':
      //Serial.println("ack");  // Enviar confirmación de éxito
      break;

    case 'r':
      //Serial.println("ack");  // Enviar confirmación de éxito
      break;  

    case 'o':
      //Serial.println("ack");  // Enviar confirmación de éxito
      break;
    
    case 'a':
      //Serial.println("ack");  // Enviar confirmación de éxito
      break;

    case 'k':
      //Serial.println("ack");  // Enviar confirmación de éxito
      break;

    case 'g':
      // Obtener información actual
      /*
      switch (infoType) {
        case 'd':
          //Serial.print("d ");
          ///Serial.println(dutyCycle);
          break;

        case 'r':
          //Serial.print("r ");
          //Serial.println(illuminanceReference);
          break;
        
        case 'l':
          //Serial.print("l ");
          //Serial.println(illuminanceReference);
          break;
        
        case 'o':
          //Serial.print("o ");
          //Serial.println(illuminanceReference);
          break;

        case 'a':
          //Serial.print("a ");
          //Serial.println(illuminanceReference);
          break;

        case 'x':
          //Serial.print("x ");
          //Serial.println(illuminanceReference);
          break;

        case 'k':
          //Serial.print("k ");
          //Serial.println(illuminanceReference);
          break;

        case 'p':
          //Serial.print("r ");
          //Serial.println(illuminanceReference);
          break;

        case 't':
          //Serial.print("t ");
          //Serial.println(illuminanceReference);
          break;

        case 'b':
          //Serial.print("b ");
          //Serial.println(illuminanceReference);
          break;
          // Agregar casos para otros tipos de información (l, o, a, k, x, p, t) según sea necesario
          // ...
      }
      */
      break;

      case 's':
      //Serial.println("time");  // Enviar confirmación de éxito
      break;

      case 'S':
      //Serial.println("time");  // Enviar confirmación de éxito
      break;

      // Agregar más casos para otros comandos (l, o, a, k, x, p, t, s) según sea necesario
      // ...

    default:
      // Comando no reconocido
      //Serial.println("err");
      break;
  }
}
