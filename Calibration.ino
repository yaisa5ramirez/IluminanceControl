#include "can.h"
#include "mcp2515.h"
#include <hardware/flash.h>  

uint8_t this_pico_flash_id[8], node_address;
struct can_frame canMsgTx, canMsgRx;
unsigned long counterTx{ 0 }, counterRx{ 0 };
MCP2515::ERROR err;
unsigned long time_to_write;
unsigned long write_delay{ 5000 };
unsigned long small_write_delay{ 1000 };
const byte interruptPin{ 20 };
volatile byte data_available{ false };
MCP2515 can0{ spi0, 17, 19, 16, 18, 10000000 };

void checkCanError() {
  if (can0.checkError()) {
    // Get error flags
    uint8_t errorFlags = can0.getErrorFlags();

    // Check if overflow occurred
    if (errorFlags & MCP2515::EFLG_RX1OVR) {
      Serial.println("RX1OVR: Receive Buffer 1 overflow");
    }
    if (errorFlags & MCP2515::EFLG_RX0OVR) {
      Serial.println("RX0OVR: Receive Buffer 0 overflow");
      // Clear the overflow flag and the receive buffer
      can0.clearRXnOVR();                     // Clear RX0OVR flag
      can0.readMessage(MCP2515::RXB0, NULL);  // Read and discard the message from RXB0
    }
    if (errorFlags & MCP2515::EFLG_TXBO) {
      Serial.println("TXBO: Bus-off state");
    }
    // Additional error flags can be checked similarly

    // Clear error flags
    can0.clearERRIF();
  }
}

void read_interrupt(uint gpio, uint32_t events) {
  data_available = true;
}

int ids = 0;
int row = 0;
int ledsprendidos = 0;
int finished = 0;
bool ackLED = false;
bool wakingup = true;
bool ack = false;
bool allfinished = false;
bool off = false;
uint8_t listids[3];
uint8_t finishedIds[] = {};
uint8_t NodesWhoAck[] = {};
int acks = 0;
int ownack = 0;

// Contains the gains of all the leds
// Row 0 is O_i
// Rows 1,2,3 contain the K of the others of the calibration
float Kmatrix[4][3] = {
  { 99, 99, 99 },
  { 99, 99, 99 },
  { 99, 99, 99 },
  { 99, 99, 99 }
};

void resetArray(uint8_t arr[], uint8_t value) {
  for (int i = 0; i < 3; i++) {
    arr[i] = value;
  }
}

void printMatrix(float matrix[4][3]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.print(matrix[i][j]);
      Serial.print('\t');  // Add tab between elements for better formatting
    }
    Serial.println();  // Move to the next line after printing each row
  }
}


void setup() {
  // setup can
  flash_get_unique_id(this_pico_flash_id);
  node_address = this_pico_flash_id[7];
  can0.reset();
  can0.setBitrate(CAN_1000KBPS);
  can0.setNormalMode();
  gpio_set_irq_enabled_with_callback(interruptPin, GPIO_IRQ_EDGE_FALL, true, &read_interrupt);
  time_to_write = millis() + write_delay;
}

void Ranking(uint8_t arr[]) {  // Make a ranking of the list of devices identified
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2 - i; j++) {
      if (arr[j] < arr[j + 1]) {
        // Swap the elements if they are in the wrong order
        uint8_t temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}


bool isNumberNotInList(uint8_t arr[], uint8_t number) {  // Check if the id has not been recieved before
  for (int i = 0; i < 3; i++) {
    if (arr[i] == number) {
      return false;
    }
  }
  // Number is not found in the list, return true
  return true;
}


int findPosition(uint8_t arr[], uint8_t target) {  // Find my index in the list of ids
  for (int i = 0; i < 3; i++) {
    if (arr[i] == target) {
      // Return the position (index) if the target is found
      return i;
    }
  }
  return -1;
}


void SendData(unsigned int node_address, float message, int who, char instruc1, char instruc2, char instruc3) {
  canMsgTx.can_id = node_address;
  canMsgTx.can_dlc = 8;

  // Convert the float to a byte array
  byte *bytePointer = (byte *)&message;  // Get a pointer to the float variable
  for (int i = 0; i < 4; i++) {
    canMsgTx.data[i] = bytePointer[i];  // Copy each byte of the float to the CAN message data
  }
  canMsgTx.data[4] = byte(who);
  canMsgTx.data[5] = instruc1;
  canMsgTx.data[6] = instruc2;
  canMsgTx.data[7] = instruc3;

  err = can0.sendMessage(&canMsgTx);  // Send message
  //Serial.print("I am sending ");
  //Serial.print(who);
  //Serial.print(instruc1);
  //Serial.print(instruc2);
  //Serial.println(instruc3);
}


void recieveData(bool *ackLED, bool *allfinished, bool *ack, bool *off, int *ids, int myId, int row, int *ledsprendidos, int *ownack) {
  data_available = true;
  unsigned long last_send_time = 0;  // Variable to track the time of the last message sent
  if (data_available) {
    unsigned long current_time = millis();  // Get the current time
    checkCanError(); //Flush the buffer in case it is full
    can0.readMessage(&canMsgRx);
    String instruction = String(char(canMsgRx.data[5])) + String(char(canMsgRx.data[6])) + String(char(canMsgRx.data[7]));
    uint8_t recievedId = canMsgRx.data[4];  // Define the indice of the martrix for this node
    float message;
    byte *bytePointer = (byte *)&message;  // Get a pointer to the float variable
    for (int i = 0; i < 4; i++) {
      bytePointer[i] = canMsgRx.data[i];  // Copy each byte from the CAN message data to the float
    }
    //Serial.print("I recieved from  ");
    //Serial.print(recievedId);
    //Serial.print(" the message ");
    //Serial.print(message);
    //Serial.print(" with the instruction ");
    //Serial.println(instruction);
    uint8_t sender = canMsgRx.can_id;  // Who sent the message

    if (instruction == "001") {  // Recieved the order of saving the data of someone else, send ack to that node, meassure my data and send it 
      float lux = 1;  // Meassure mine ADD READ MY LUX
      if (Kmatrix[row][recievedId] == 99.00) { // Check if I had saved that position before
        Kmatrix[row][recievedId] = message;  // Save in the matrix
        }
      if (current_time - last_send_time >= small_write_delay) {  // Check if it's time to send a message
        Serial.println(" I will send that node the ack that I saved his data ");
        SendData(node_address, lux, recievedId, '1', '1', '1');  // Confirm that I saved the message
        last_send_time = current_time;                           // Update the last send time
      }

    }

    else if ((instruction == "111") && (!*ack)) { // Someone acknowledged me and I have not finished ack
      if (recievedId == myId) {  // Check if the ack message is to me
        //Serial.println("He was answering to me ");
        if (isNumberNotInList(NodesWhoAck, sender)) {  // Check if the node that sent the message is new and save it on the list
          Serial.println(" Someone new saved my data");
          NodesWhoAck[*ids] = sender;
          *ids = *ids + 1;
        }

        if (*ids == 2)  {  // 2 when all can bus are working
          Serial.println("Acknowledgment was done ");
          *ack = true;  // If I recieved confirmation that all nodes saved my data, I can continue
          *ids = 0;
          }
      }
    }

    else if ((instruction == "222") && (*ack)) {    // Someone says they finished and I all ready finished ack
      if (isNumberNotInList(finishedIds, recievedId)) {  // Check if the node that sent the message is new and save it on the list
        Serial.println("Someone new announced he finished ");
        finishedIds[finished] = recievedId;
        finished = finished + 1;
      }
      if (finished == 2) {
        *allfinished = true;  // If I recieved data from two different nodes, I can continue   
        Serial.println("All finished saving the data");
        finished = 0;
      }
    }

    else if ((instruction == "003") && (*allfinished)) {  // Someone turned off the led and I am sure that everybody finished
      Serial.println("Someone turned off a led");
      if (*off == false) ledsprendidos += 1; // ledsprendidos 
      if (current_time - last_send_time >= small_write_delay) {  // Check if it's time to send a message
        Serial.println(" I am confirming that now I now someone turned off the led ");
        SendData(node_address, 1, myId, '3', '3', '3');  // I confirme that I now someone turned off a led, 
        last_send_time = current_time;                    
      }
      *off = true;
    } 

    else if (instruction == "333") {  // Someone acknowledged I turned off the led
        if (isNumberNotInList(NodesWhoAck, sender)) {  // Check if the node that sent the message is new and save it on the list
          Serial.println(" Someone new knows someone turned off the led");
          NodesWhoAck[*ids] = sender;
          *ids = *ids + 1;
        }
        if (*ids == 2) {  // 
          Serial.println("Everyone knows I turned off the led ");
          *ackLED = true;  // AckLED says if everyone confirme that they now a led was turned off
          *ids = 0;
      }
      }
    
    else if (instruction == "444") {  
      Serial.println("SOME ONE FINISHED ALL");
    }


  }

  data_available = false;
}

bool WaitForReception(uint8_t myaddress, float lux, int myId, bool allfinished, bool off, int finished, int *ledsprendidos) {
  ids = 0;
  ack = false;
  unsigned long last_send_time = 0;  // Variable to track the time of the last message sent
  ownack = 0;
  while (!allfinished) { 
    unsigned long current_time = millis();  // Get the current time
    if (!ack) {
      if (current_time - last_send_time >= write_delay) {  // Check if it's time to send a message
        Serial.println(" I am sending nodes my data");
        SendData(myaddress, lux, myId, '0', '0', '1');  // Send message to request acknowledgment
        last_send_time = current_time;                  // Update the last send time
      }
      recieveData(&ackLED, &allfinished, &ack, &off, &ids, myId, row, ledsprendidos, &ownack);  // Process received data
      //Serial.println("State: waiting for ack");
    } 
    else {                                               // Check if it's time to send a message
      if (current_time - last_send_time >= write_delay) {  // Check if it's time to send a message
        Serial.println(" I am sending nodes that I finished");
        SendData(myaddress, lux, myId, '2', '2', '2');  // Send message to request acknowledgment
        last_send_time = current_time;                  // Update the last send time
      }
      recieveData(&ackLED,&allfinished, &ack, &off, &ids, myId, row, ledsprendidos, &ownack);  // Process received data
    }
  }
  return allfinished;
}



void Calibrate(int myId, uint8_t myaddress) {
  data_available = true;
  bool allfinished = false;
  while (row < 4) {  
    // Everybody reads the 0 pwm meassurement
    Serial.print("Current row is ");
    Serial.println(row);
    if (row == 0) {              // First meassurement
      float lux = row + myId;    // MEASSURE the current lux in the node 
      Kmatrix[row][myId] = lux;  // Save my date
      // I tell everyone to save my data and wait for all finished to be true
      allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, &ledsprendidos);
      if (allfinished) {
        Serial.println(" Since everybody finished, I add one row");
        allfinished = false;
        row += 1;
      }
    }

    else if (row != 0) {  // Calibration meassurements start
      Serial.print("Ahora leds encendidos es ");
      Serial.println(ledsprendidos);

      if (myId == ledsprendidos) { // My turn to turn on the ledsss
        Serial.println("My turn to turn on LED");  //It is my turn to turn on the led
        // Turn on led
        float lux = row + myId; // MEASSURE
        Kmatrix[row][myId] = lux;  // Almaceno en la matriz
        resetArray(NodesWhoAck, 9);
        resetArray(finishedIds, 9);
        allfinished = false;
        // I tell everyone to save my data and wait for all finished to be true
        allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, &ledsprendidos);

        // When everyone finishes meassuring and saving data, turn of the led and start again with another one

        if (allfinished) {
          Serial.println(" Everyone finished saving data, now I will turn off the led");
          // Turn off the led
          int i = 0;
          ids = 0;
          ackLED = false;
          unsigned long last_send_time = 0;  // Variable to track the time of the last message sent
          resetArray(NodesWhoAck, 9);
          while (!ackLED) {  // While people dont know I turned off the led
            unsigned long current_time = millis();               
            if (current_time - last_send_time >= write_delay) {  
              SendData(myaddress, 1, myId, '0', '0', '3');       // Tell everyone that a led was turned off
              last_send_time = current_time;                    
            }
            recieveData(&ackLED, &allfinished, &ack, &off, &ids, myId, row, &ledsprendidos, &ownack);  // Save what the others meassured
          }
          ledsprendidos += 1;
          row += 1;
          allfinished = false;
        }

      } 
      
      else { // It is not my turn to turn on the led
        float lux = row + myId;    // MEASSURE
        Kmatrix[row][myId] = lux;  // Save my meassure in the matrix
        resetArray(NodesWhoAck, 9);
        resetArray(finishedIds, 9);
        allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, &ledsprendidos);

        if (allfinished) {
          Serial.println(" Everyone saved the data, I am waiting for the led sign to go off");
          off = false;
          ids = 0;
          int i = 0;
          unsigned long last_send_time = 0;  // Variable to track the time of the last message sent
          while (!off) { //While the led sign is off, I keep telling everyone that I finished saving the data
            unsigned long current_time = millis();               // Get the current time
            if (current_time - last_send_time >= write_delay) {  // Check if it's time to send a message
              SendData(myaddress, 1, myId, '2', '2', '2');       // To make sre that everybody reaches the all finished
              last_send_time = current_time;                     // Update the last send time
            }
            recieveData(&ackLED,&allfinished, &ack, &off, &ids, myId, row, &ledsprendidos, &ownack);  
          }
          ledsprendidos += 1;
          allfinished = false;
        }
        row += 1;
      }
    }
  }
  unsigned long last_send_time = 0;  // Variable to track the time of the last message sent
  int i = 0;
  while (i < 15) {  // I send the finished message 15 times to make sre that everybody reaches the all finished
    unsigned long current_time = millis();               // Get the current time
    if (current_time - last_send_time >= write_delay) {  // Check if it's time to send a message
      SendData(myaddress, 1, myId, '2', '2', '2');       // To make sre that everybody reaches the all finished
      last_send_time = current_time;                     // Update the last send time
      i = i + 1;
      Serial.println("My ID is");
      Serial.println(myId);
      Serial.println("and I finished with this matrix");
      printMatrix(Kmatrix);
      
    }
  }
}

bool wakeup(bool wakingup) {
  listids[ids] = node_address;              // Add my address to the list of addresses
  unsigned long last_send_time = 0;         // Variable to track the time of the last message sent
  while (wakingup) {                        // Wait for all the addresses to contact each other
    unsigned long current_time = millis();  // Get the current time
    SendData(node_address, 1, 1, '0', '0', '0');

    if (data_available) {  // If there are messages, read them
      Serial.print("I recieved a message from ");
      can0.readMessage(&canMsgRx);
      String message = String(char(canMsgRx.data[5])) + String(char(canMsgRx.data[6])) + String(char(canMsgRx.data[7]));
      uint8_t sender = canMsgRx.can_id;  // Who sent the message
      Serial.println(sender);
      if (message == "000") {  /// 000 indicates a node woke up
        Serial.println("Someone woke up ");
        if (isNumberNotInList(listids, sender)) {  // Check if the node that sent the message is new and save it on the list
          Serial.println("The address is not in the list  ");
          ids = ids + 1;
          listids[ids] = sender;
          if (ids == 2) {  // 2 when all the can bus are working CHANGE
            wakingup = false;
            Serial.println("Everybody wokeup ");
          };
        }
      }
    }
  }
  data_available = false;
  Ranking(listids);                                    // Organize the addresses from bigger to smaller
  uint8_t myId = findPosition(listids, node_address);  // Define the indice of the martrix for this node depending on its position in the list
  Serial.print("My address is ");
  Serial.println(node_address);
  Serial.print("The id of my address is ");
  Serial.println(myId);
  Calibrate(myId, node_address);  // Begin calibration
  return wakingup;
}


void loop() {

  while (wakingup) {
    wakingup = wakeup(wakingup);  // La llamo una unica vez
  }
  if (millis() >= time_to_write) {
    SendData(node_address, 1, 1, '3', '3', '3');  // To make sre that everybody reaches the all finished
    Serial.println("I am letting every body know I finished alll");
    // Wakeup function calls calibrate inside itself
    time_to_write = millis() + write_delay;
  }
}
