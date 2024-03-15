#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <arduino.h>

class pidcontroller {
  float I, D, K, Ti, Td, Tt, b, h, y_old, N;
public:
  bool antiwindup = false;
  explicit pidcontroller(float _h, float _K = 1, float b_ = 1, float Ti_ = 1, float Td_ = 0, float Tt_ = 1, float N_ = 10);  //constructor
  ~pidcontroller(){};                                                                                                        //destructor
  float compute_control(float r, float y);                                                                                   //function
  float saturateOutput(float v);                                                                                             //function that updates the controller with new error, integral and yold
  void update_integral(float r, float y, float v, float u);                                                                   //function that updates the controller with new error, integral and yold
  void bumpless_transfer(float K_new, float b_new, float r, float y);
};

// Compute control calculates V = P + I + D
// Now we saturate the output
inline float pidcontroller::saturateOutput(float v) {
  //Saturate de output
  float u = v;
  if (u < 0) u = 0;
  if (u > 4095) u = 4095;
  return u;
}

inline void pidcontroller::update_integral(float r, float y, float u, float v) {  
  float e = r - y;
  float bi = K * h / Ti;
  float ao = h / Tt;  // Anti-Windup Coeficient
  I += bi * e;
  if (antiwindup == true) I += ao * (u - v);
  y_old = y;
}

// Function in case the reference changes

inline void pidcontroller::bumpless_transfer(float K_new, float b_new, float r, float y) {  // This just happens when I change the reference
  I += K * (b * r - y) - K_new * (b_new * r - y);
  K = K_new;
  b = b_new;
}


#endif
