#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H
#include <cstdint>
static constexpr uint8_t pwm_left_pin = 2; //13
static constexpr uint8_t in1_left_pin = 4; //11
static constexpr uint8_t in2_left_pin = 3; //12

static constexpr uint8_t pwm_right_pin = 13; //2
static constexpr uint8_t in1_right_pin = 11; //4
static constexpr uint8_t in2_right_pin = 12; //3

namespace motors {
struct Motor {
  int motorSpeed;
  uint8_t pwm;
  uint8_t in1;
  uint8_t in2;
};

enum Motors {Left, Right};

class Raspi_I2C {
public:
  explicit Raspi_I2C(uint8_t addr, const std::string& dev = "/dev/i2c-1");
  ~Raspi_I2C();

  int write8(uint8_t reg, uint8_t value) const;
  int readU8(uint8_t reg) const;
  bool debug;

private:
  int errMsg() const;
  int file;
  uint8_t address;
};

class PCA9685 {
public:
  explicit PCA9685(uint8_t address = 0x40, bool dbg = false);
  int setMotorSpeed(Motors selected_motor, int speed);

private:
  Raspi_I2C i2c;
  bool debug;
  Motor m_leftMotor = {0, pwm_left_pin, in1_left_pin, in2_left_pin};
  Motor m_rightMotor = {0, pwm_right_pin, in1_right_pin, in2_right_pin};

  void setPWM(uint8_t channel, uint16_t on, uint16_t off);
  void setAllPWM(uint16_t on, uint16_t off);
  void setPWMFreq();
};
}
#endif //MOTOR_DRIVER_H
