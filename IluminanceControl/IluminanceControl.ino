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
float xold = 0;  // Entrada serial monitor.

// Variables controlador
float Td{ 0 };
float N{ 3 };
float Knew{ 3 };
float bnew{ 3 };

int idoficina = 1;

//Oficina [1] Node 60 COM 16
// Oficina [2] Node 18 COM 12
// Oficina [3] Node D1 COM 13

// States
boolean newref = false;

// Default, solo lo    h,   K,  b,  Ti,  Td, Tt   }
pidcontroller my_pid{ 0.01, 100, 1, 0.5, 0, 0.1 };
//                         100, 2, 0.5, 0, 0.1
// pidcontroller my_pid{ 0.01, 100, 1, 0.5, 0, 0.1 }; BAJADA BUENO
// B no uede ser mayor a 2

// Info of serial
readSerial leerserial{};
boolean newData = false;

int contador = 0;

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


//Circuito 1  // bSet es Para evitar que la luz haga flickering

float r{ 0.0 };
void setup() {
  analogReadResolution(12);
  analogWriteFreq(60000);       //60KHz
  analogWriteRange(DAC_RANGE);  //100% duty cycle
  Serial.begin(9600);


  // Select what office to control
  Serial.println(office);
  if (office == 1) {
    Serial.print("OFICINA 1");
    pidcontroller my_pid{ 0.01, 7, 0.98, 0.04, 0, 3 };
  }  //h, K, bSet, Ti, Td, N}
  else if (office == 2) {
    pidcontroller my_pid{ 0.01, 7, 0.98, 0.04, 0, 3 };
  }  //h, K, bSet, Ti, Td, N}
  else if (office == 3) {
    pidcontroller my_pid{ 0.01, 7, 0.98, 0.04, 0, 3 };
  }  //h, K, bSet, Ti, Td, N}
}

void loop() {

  // If I change the reference. 
  if (newref == true) {
    float dif = xref -xold;
    if (dif < 0.0) { //subiendo
      Knew = 90;
      bnew = 2;
    } else {
      Knew = 100;
      bnew = 2;
    }
    my_pid.bumpless_transfer(Knew, bnew, xref, lux);
    newref = false;
  }

  // Read orders from serial monitor
  newData = leerserial.recvData(newData);
  if (newData == true) {
    leerserial.parseData();
    newData = false;
  }

  // Transformar voltaje leido a Lux
  lux = voltsLux();
  // H_x = voltage / lux;
  // Calcular H(x) como funcion del voltaje y lux

  // Compute PID Control (desired, real)

  float v = my_pid.compute_control(xref, lux);
  float u = my_pid.saturateOutput(v);  // U es el dutycycle de salida, este entra al bloque G y la salida del sistema es lux
  int pwm = (int)u;
  analogWrite(LED_PIN, pwm);

  my_pid.update_integral(xref, lux, u, v);  //Integral, acumulo el error y reemplazo lux por luxold

  
  Serial.print(45);    //the first variable for plotting
  Serial.print(",");  //seperator
  Serial.print(xref);    //the first variable for plotting
  Serial.print(",");  //seperator
  Serial.print(lux);
  Serial.print(",");  //seperator
  Serial.println(0);    //the first variable for plotting
  xold = xref;

  if (contador == 60000) {
    xref = 50;
    newref = true;
  }
  
  contador += 1;
}