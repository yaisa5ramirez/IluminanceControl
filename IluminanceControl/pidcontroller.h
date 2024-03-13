#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

class pidcontroller {
  float I, D, K, Ti, Td, Tt, b, h, y_old, N;
public:
  explicit pidcontroller(float _h, float _K = 1, float b_ = 1, float Ti_ = 1, float Td_ = 0, float Tt_ = 1, float N_ = 10);  //constructor
  ~pidcontroller(){};                                                                                                        //destructor
  float compute_control(float r, float y);                                                                                   //function
  void updateIntegral(float r, float y, float v, float u);                                                                   //function that updates the controller with new error, integral and yold
  float saturateOutput(float v);                                                                                             //function that updates the controller with new error, integral and yold
};

inline void pidcontroller::updateIntegral(float r, float y, float v, float u) {
  float e = r - y;
  // Antiwindup
  bool antibol = false;
  if (antibol) {
    float anti = u - v;
    I += (K * h / Ti * e) + (h * anti / Tt);
  } else {
    I += K * h / Ti * e;
  }

  y_old = y;
}

inline float pidcontroller::saturateOutput(float v) {
  //Saturate de output
  float u = v;
  if (u < 0) u = 0;
  if (u > 4095) u = 4095;
  return u;
}

#endif
