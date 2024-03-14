#ifndef READSERIAL_H
#define READSERIAL_H
#include <Arduino.h>

class readSerial {
  public:
    readSerial();
    void loop();
    bool recvData(bool newData);
    void parseData();
    boolean newData = false;

  private:
    char receivedCommand[32];
    char tempChars[32];
    
  
    int newDutyCycle;
    char luminaire;
    int currentDutyCycle;
    String resp;
    float bla = 1.2;
    String command3;
  
  
};


#endif