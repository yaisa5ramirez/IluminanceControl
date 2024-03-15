#include "can.h"
#include "mcp2515.h"
#include <hardware/flash.h> //for flash_get_unique_id

uint8_t this_pico_flash_id[8], node_address;
struct can_frame canMsgTx, canMsgRx;
unsigned long counterTx {0}, counterRx {0};
MCP2515::ERROR err;
unsigned long time_to_write;
unsigned long write_delay {100};
const byte interruptPin {20};
volatile byte data_available {false};
//MCP2515 can0 {spi0, 1, 3, 0, 2, 10000000};
MCP2515 can0 {spi0, 17, 19, 16, 18, 10000000};

//the interrupt service routine
void read_interrupt(uint gpio, uint32_t events) {
 data_available = true;
}

int oficina = 1;
//Oficina [1] Node 61 COM 16
// Oficina [2] Node 18 COM 12
// Oficina [3] Node D1 COM 13

void setup() {
  flash_get_unique_id(this_pico_flash_id);
  node_address = this_pico_flash_id[3];
  Serial.begin();
  can0.reset();
  can0.setBitrate(CAN_1000KBPS);
  can0.setNormalMode();
  gpio_set_irq_enabled_with_callback(
    interruptPin, GPIO_IRQ_EDGE_FALL,
    true, &read_interrupt);
  time_to_write = millis() + write_delay;
}


void loop() {
  if (millis() >= time_to_write) {
    canMsgTx.can_id = node_address;
    canMsgTx.can_dlc = 8;
    unsigned long div = counterTx * 10;
    for (int i = 0; i < 8; i++)
      canMsgTx.data[7 - i] = '0' + ((div /= 10) % 10);
    err = can0.sendMessage(&canMsgTx);
    Serial.print("Sending message ");
    Serial.print(counterTx);
    Serial.print(" from node ");
    Serial.println(node_address, HEX);
    counterTx++;
    time_to_write = millis() + write_delay;
  }
  if (data_available) {
    can0.readMessage(&canMsgRx);
    Serial.print("Received message number ");
    Serial.print(counterRx++);
    Serial.print(" from node ");
    Serial.print(canMsgRx.can_id, HEX);
    Serial.print(" : ");
    for (int i = 0; i < canMsgRx.can_dlc; i++)
      Serial.print((char)canMsgRx.data[i]);
    Serial.println(" ");
    data_available = false;
  }
}