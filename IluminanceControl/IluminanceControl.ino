#include "pidcontroller.h"
#include "can.h"
#include "mcp2515.h"
#include "metrics.h"
#include <hardware/flash.h> //for flash_get_unique_id

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
float xref{ 0 };  // Entrada serial monitor. 

// Variables controlador
float Td{ 0 };
float N{ 3 };

// CAN BUS Communication
uint8_t this_pico_flash_id[8], node_address;
struct can_frame canMsgTx, canMsgRx;
unsigned long counterTx {0}, counterRx {0};
MCP2515::ERROR err;
unsigned long time_to_write;
unsigned long write_delay {1000};
const byte interruptPin {20};
volatile byte data_available {false};
MCP2515 can0 {spi0, 17, 19, 16, 18, 10000000};
int idoficina = 1;
//Oficina [1] Node 60 COM 16
// Oficina [2] Node 18 COM 12
// Oficina [3] Node D1 COM 13

//the interrupt service routine
void read_interrupt(uint gpio, uint32_t events) {
 data_available = true;
}

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
pidcontroller my_pid1 { 0.01, 7, 0.98, 0.04, 0, 3 };  //h, K, bSet, Ti, Td, N
pidcontroller my_pid2 { 0.01, 7, 0.98, 0.04, 0, 3 };  //h, K, bSet, Ti, Td, N
pidcontroller my_pid3 { 0.01, 11, 0.96, 0.05, 0, 3 };  //h, K, bSet, Ti, Td, N

float r{ 0.0 };
void setup() {
  analogReadResolution(12);
  analogWriteFreq(60000);       //30KHz
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  Serial.begin(115200);
  //CAN BUS 
  flash_get_unique_id(this_pico_flash_id);
  node_address = this_pico_flash_id[idoficina];
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
  
  if (Serial.available() > 0) {
      // Read the integer from the Serial Monitor
      char solicitud = Serial.read(); // Leer el comando desde el monitor serial
      readmySerial(solicitud);

        //Transformar voltaje in a lux 
	while (Serial.available() > 0) {
       Serial.read();
}
} 
    lux = voltsLux();
  // Calcular H(x) como funcion del voltaje y lux
  // H_x = voltage / lux;
  float v = my_pid1.compute_control(xref, lux);
  float u = my_pid1.saturateOutput(v);
  int pwm = (int)u;
  analogWrite(LED_PIN, pwm);
  my_pid1.updateIntegral(xref, lux, v, u);  //Integral, acumulo el error y reemplazo lux por luxold
  delay(10);


  Serial.print(u);  //the first variable for plotting
  Serial.print(",");   //seperator
  Serial.println(lux);
}