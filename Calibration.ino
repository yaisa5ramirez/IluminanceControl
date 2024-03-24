#include "can.h"
#include "mcp2515.h"
#include <hardware/flash.h>  //for flash_get_unique_id

uint8_t this_pico_flash_id[8], node_address;
struct can_frame canMsgTx, canMsgRx;
unsigned long counterTx{ 0 }, counterRx{ 0 };
MCP2515::ERROR err;
unsigned long time_to_write;
unsigned long write_delay{ 100 };
const byte interruptPin{ 20 };
volatile byte data_available{ false };
//MCP2515 can0 {spi0, 1, 3, 0, 2, 10000000};
MCP2515 can0{ spi0, 17, 19, 16, 18, 10000000 };

//the interrupt service routine
void read_interrupt(uint gpio, uint32_t events) {
  data_available = true;
  Serial.println("READ INTERRU");
}

bool wakingup = true;
int ids = 0;
uint8_t listids[3];
int row = 0;
int ledsprendidos = 0;
int finished = 0;
uint8_t finishedIds[] = {};
uint8_t NodesWhoAck[] = {};
bool ack = false;
bool allfinished = false;
int acks = 0;

float Kmatrix[4][3] = {
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
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

void Ranking(uint8_t arr[]) {  // Make a ranking of the list
  //    Serial.print("The first element in the list is ");
  //    Serial.println(arr[0]);
  //    Serial.print("The second element in the list is ");
  //    Serial.println(arr[1]);
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
  //Serial.println("I finished organizing ");
  //Serial.print("The first element in the list is ");
  //    Serial.println(arr[0]);
  //Serial.print("The second element in the list is ");
  //    Serial.println(arr[1]);
}


bool isNumberNotInList(uint8_t arr[], uint8_t number) {  // Check if the id has not been recieved before
  for (int i = 0; i < 3; i++) {
    if (arr[i] == number) {
      // Number is found in the list, return false
      return false;
    }
  }
  // Number is not found in the list, return true
  return true;
}


int findPosition(uint8_t arr[], uint8_t target) {  // Find my indice in the list of ids
  for (int i = 0; i < 3; i++) {
    if (arr[i] == target) {
      // Return the position (index) if the target is found
      return i;
    }
  }
  return -1;
}


void SendData(unsigned int node_address, float message, int who, char instruc1, char instruc2, char instruc3) {
  delay(500);
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
  Serial.print("I am sending ");
  Serial.print(who);
  Serial.print(instruc1);
  Serial.print(instruc2);
  Serial.println(instruc3);
}


void recieveData(bool *allfinished, bool *ack, int ids, int myId, int row, int *ledsprendidos) {
  data_available = true;
  if (data_available) {
    delay(500);
    Serial.print("My ID is ");
    Serial.println(myId);
    can0.readMessage(&canMsgRx);
    String instruction = String(char(canMsgRx.data[5])) + String(char(canMsgRx.data[6])) + String(char(canMsgRx.data[7]));
    uint8_t recievedId = canMsgRx.data[4];  // Define the indice of the martrix for this node
    float message;
    byte *bytePointer = (byte *)&message;  // Get a pointer to the float variable
    for (int i = 0; i < 4; i++) {
      bytePointer[i] = canMsgRx.data[i];  // Copy each byte from the CAN message data to the float
    }
    Serial.print("I recieved the message  ");
    Serial.print(recievedId);
    Serial.println(instruction);
    uint8_t sender = canMsgRx.can_id;  // Who sent the message

    if (instruction == "001") {  // Recibi orden de guardar el dato, mandar ack y medir el mio y mandarlo
      delay(100);
      float lux = 1; // Mido mis lux
      Kmatrix[row][recievedId] = message;  // Almaceno en la matriz
      Serial.println("The matrix is now ");
      printMatrix(Kmatrix);
      SendData(node_address, lux, recievedId, '1', '1', '1');  // Mando confirmacion de que ya lo guarde
    }

    else if (instruction == "003") {  // Someone turned off the led
      ledsprendidos += 1;
    }

    else if (instruction == "111") {
      delay(100);
      if (recievedId == myId) {  // Check if the ack message is to me
        Serial.println("He was answering to me ");
        if (isNumberNotInList(NodesWhoAck, sender)) {  // Check if the node that sent the message is new and save it on the list
          Serial.println("He had not answered to me before ");
          NodesWhoAck[ids] = sender;
          ids = ids + 1;
        }
        else {
        Serial.println("He was already in the list ");
        }
        if (ids == 1) {  // 2 when all can bus are working CHANGE
          Serial.println("Acknowledgment done ");
          *ack = true;  // If I recieved confirmation that all nodes saved my data, I can continue
        }
      }
    }

    else if ((instruction == "222") && (*ack)) {  // System acknowldege, everybody saved all the data      if (isNumberNotInList(NodesWhoAck, recievedId)) {  // Check if the node that sent the message is new and save it on the list
      delay(100);
      if (isNumberNotInList(finishedIds, recievedId)) {  // Check if the node that sent the message is new and save it on the list
          Serial.println("He had not answered to me before ");
          finishedIds[finished] = recievedId;
          finished = finished + 1;
        }
      if (finished == 1) {
      *allfinished = true;  // If I recieved data from two different nodes, I can continue   2 CHANGE
      Serial.println("All finished saving the data");
    }
    }
  }
data_available = false;

}

bool WaitForReception(uint8_t myaddress, float lux, int myId, bool allfinished, bool ack, int finished, int ids, int *ledsprendidos){
  while (!allfinished) {
    delay(1000);
    if (!ack) {                                                     // I wait for the confirmation that everyone saved my data and I keep sending it until that happens
      SendData(myaddress, lux, myId, '0', '0', '1');                   // I wait for the confirmation that everyone saved my data and I keep sending it until that happens
      recieveData(&allfinished, &ack, ids, myId, row, ledsprendidos);  // Save what the others meassured
    }
    else {
      SendData(myaddress, 1, myId, '2', '2', '2');  // Tell the other ones that I finished
      recieveData(&allfinished, &ack, ids, myId, row, ledsprendidos);  // Save what the others meassured
    }
 }
 return allfinished;
  }


void Calibrate(int myId, uint8_t myaddress) {
  delay(2000);
  data_available = true;
  bool allfinished = false;
  while (row < 4) {
    // Everybody reads the 0 pwm meassurement
    Serial.print("Row is ");
    Serial.println(row);
    if (row == 0) {              // First meassurement
      float lux = 45.1 + myId;   // Meassure the current lux in the node
      Kmatrix[row][myId] = lux;  // Almaceno en la matriz
      // I tell everyone to save my data
      allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, 0, &ledsprendidos);
      
      if (allfinished) {
          // Turn off led
          SendData(myaddress, 1, myId, '0', '0', '3');  // Tell everyone that a led was turned off
        }
    }

    else if (row != 0) {                                                    // Calibration meassurements start
      Serial.print("Leds encendidos es "); 
      Serial.println(ledsprendidos); 
      
      if (myId == ledsprendidos) {  
        Serial.println("Es mi turno de encender el LED");                                   //It is my turn to turn on the led
        // Turn on led
        float lux = 12.1 + myId;
        Kmatrix[row][myId] = lux;  // Almaceno en la matriz
        resetArray(NodesWhoAck, 9);
        resetArray(finishedIds, 9);
        allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, 0, &ledsprendidos);
        
        // When everyone finishes meassuring and saving data, turn of the led and start again with another one
        if (allfinished) {
          // Turn off led
          SendData(myaddress, 1, myId, '0', '0', '3');  // Tell everyone that a led was turned off
        }

      } 
      else {
        delay(10);  //Esero a que el lider renda el led
        // Meassure
        float lux = 1.1 + myId;
        Kmatrix[row][myId] = lux;  // Almaceno en la matriz
        resetArray(NodesWhoAck, 9);
        resetArray(finishedIds, 9);
        allfinished = WaitForReception(myaddress, lux, myId, false, false, 0, 0, &ledsprendidos);
        
      }
    }

    row += 1;
  }
}

bool wakeup(bool wakingup) {
  listids[ids] = node_address;  // Add my address to the list of addresses
  while (wakingup) {            // Wait for all the addresses to contact each other
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
          if (ids == 1) {  // 2 when all the can bus are working CHANGE
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
  Serial.print("The id of my address is ");
  Serial.println(myId);
  Calibrate(myId, node_address);  // Begin calibration
  return wakingup;
}


void loop() {

  if (wakingup) {
    wakingup = wakeup(wakingup);  // La llamo una unica vez
  }

  // Wakeup function calls calibrate inside itself
}
