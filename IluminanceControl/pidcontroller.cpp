#include "pidcontroller.h"

pidcontroller::pidcontroller(float _h, float _K, float b_, float Ti_, float Td_, float Tt_, float N_)  // member variable initialization list
  : h{ _h }, K{ _K }, b{ b_ }, Ti{ Ti_ }, Td{ Td_ }, Tt{ Tt_ }, N{ N_ }, I{ 0.0 }, D{ 0.0 }, y_old{ 0.0 } {}      // should check arguments validity

float pidcontroller::compute_control(float r, float y) {
  float P = K * (b * r - y);
  float ad = Td / (Td + N * h);
  float bd = Td * K * N / (Td + N * h);
  D = ad * D - bd * (y - y_old);
  float v = P + I + D;
  return v;
}  // Equivalente a compute not saturated contorl

