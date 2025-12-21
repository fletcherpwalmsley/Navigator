#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H
#include <cstdint>

class PCA9685 {
public:
  explicit PCA9685(uint8_t address = 0x40, bool dbg = false);
  void setPWMFreq();
  void setPWM(uint8_t channel, uint16_t on, uint16_t off);
  void setAllPWM(uint16_t on, uint16_t off);
};

#endif //MOTOR_DRIVER_H
