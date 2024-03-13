#include "pidcontroller.h"
#include "readSerial.h"
#include "can.h"
#include "mcp2515.h"
#include "metrics.h"
#include <hardware/flash.h>  //for flash_get_unique_id

// Select Office
const int office = 1;

// No modificar
const int LED_PIN = 15;
const int DAC_RANGE = 4095;
const float G = 0.245;
const float r2 = 10000;

// Variables calculadas
float b = 6.1;
float m = -0.8;

// Variables a recalcular
float voltage = 0;
float r1 = 0;
float lux = 0;
float H_x = 0;
float u_ff = 0;
int counter = 0;
float xref = 0;  // Entrada serial monitor.

// Variables controlador
float Td{ 0 };
float N{ 3 };

// CAN BUS Communication
uint8_t this_pico_flash_id[8], node_address;
struct can_frame canMsgTx, canMsgRx;
unsigned long counterTx{ 0 }, counterRx{ 0 };
MCP2515::ERROR err;
unsigned long time_to_write;
unsigned long write_delay{ 1000 };
const byte interruptPin{ 20 };
volatile byte data_available{ false };
MCP2515 can0{ spi0, 17, 19, 16, 18, 10000000 };
int idoficina = 1;

//Oficina [1] Node 60 COM 16
// Oficina [2] Node 18 COM 12
// Oficina [3] Node D1 COM 13

//the interrupt service routine
void read_interrupt(uint gpio, uint32_t events) {
  data_available = true;
}

// Default, solo lo declaro 
pidcontroller my_pid {0.01, 7, 0.98, 0.04, 0, 3};


// Info of serial

const byte numChars = 32;
char recievedCommand[numChars];
char tempChars[numChars];  // temporary array for use when parsing
boolean newData = false;
int newdutycycle;
int iluminaire;
int currentdutycycle;
int newreference;

float voltsLux() {
  int read_adc;
  read_adc = analogRead(A0);  // read analog voltage
  // convertir analog_in a voltaje
  voltage = read_adc * 3.3 / 4096;
  // calcular la resistencia del LDR segun el voltaje que ingresa
  r1 = (3.3 * r2 / voltage) - r2;
  // calcular lux a partir de r1, considerando relacion lineal en log-log entre r1 y lux
  lux = pow(10, (log10(r1) - b) / -0.8);
  return lux;
}

/*
  void feedforward(float Xref) {
    float counter_ff;
    u_ff = Xref / G;
    counter_ff = 4096 * u_ff;
    Serial.println(counter_ff);
    analogWrite(LED_PIN, int(counter_ff));  // set led PWM
  }
  */

//Circuito 1  // bSet es Para evitar que la luz haga flickering

float r{ 0.0 };
void setup() {
  analogReadResolution(12);
  analogWriteFreq(60000);       //30KHz
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  Serial.begin(115200);
  //CAN BUS
  flash_get_unique_id(this_pico_flash_id);
  node_address = this_pico_flash_id[idoficina];
  can0.reset();
  can0.setBitrate(CAN_1000KBPS);
  can0.setNormalMode();
  gpio_set_irq_enabled_with_callback(
  interruptPin, GPIO_IRQ_EDGE_FALL,
  true, &read_interrupt);
  time_to_write = millis() + write_delay;

  // Select what office to control
  Serial.println(office);
  if (office == 1) {
    Serial.print("OFICINA 1");
    pidcontroller my_pid {0.01, 7, 0.98, 0.04, 0, 3};
    }  //h, K, bSet, Ti, Td, N}
    else if (office == 2) {
    pidcontroller my_pid {0.01, 7, 0.98, 0.04, 0, 3};
  }  //h, K, bSet, Ti, Td, N}
    else if (office == 3) {
    pidcontroller my_pid {0.01, 7, 0.98, 0.04, 0, 3};
  }  //h, K, bSet, Ti, Td, N}
 
}

void loop() {
  
  // Read orders from serial monitor
  /*
  recvWithStartEndMarkers();
  if (newData == true) {
    parseData();
    newData = false;
  }
  */
   if (Serial.available() > 0) {
      // Read the integer from the Serial Monitor
      xref = Serial.parseFloat(); // Leer el comando desde el monitor serial
        //Transformar voltaje in a lux 
	while (Serial.available() > 0) {
       Serial.read();
}
}
  // Transformar voltaje leido a Lux
  lux = voltsLux();
   // Calcular H(x) como funcion del voltaje y lux
  // H_x = voltage / lux;

  // Compute PID Control (desired, real)
  
  float v = my_pid.compute_control(xref, lux);
  float u = my_pid.saturateOutput(v);  // U es el dutycycle de salida, este entra al bloque G y la salida del sistema es lux
  int pwm = (int)u;
  analogWrite(LED_PIN, pwm);
  my_pid.updateIntegral(xref, lux, v, u);  //Integral, acumulo el error y reemplazo lux por luxold

  /*
  // Send data via CAN BUS
  if (millis() >= time_to_write) {
    canMsgTx.can_id = node_address;
    canMsgTx.can_dlc = 8;
    unsigned long div = counterTx * 10;
    for (int i = 0; i < 8; i++)
      canMsgTx.data[7 - i] = '0' + ((div /= 10) % 10);
    err = can0.sendMessage(&canMsgTx);
    //Serial.print("Sending message ");
    //Serial.print(counterTx);
    //Serial.print(" from node ");
    //Serial.println(node_address, HEX);
    counterTx++;
    time_to_write = millis() + write_delay;
  }

  // Recieve data from CAN BUS

  if (data_available) {
    can0.readMessage(&canMsgRx);
    //Serial.print("Received message number ");
    //Serial.print(counterRx++);
    //Serial.print(" from node ");
    //Serial.print(canMsgRx.can_id, HEX);
    //Serial.print(" : ");
    for (int i = 0; i < canMsgRx.can_dlc; i++)
      //Serial.print((char)canMsgRx.data[i]);
    //Serial.println(" ");
    data_available = false;
  }
  */
  
  Serial.print(45);    //the first variable for plotting
  Serial.print(",");  //seperator
  Serial.print(xref);    //the first variable for plotting
  Serial.print(",");  //seperator
  Serial.print(lux);
  Serial.print(",");  //seperator
  Serial.println(0);    //the first variable for plotting

}