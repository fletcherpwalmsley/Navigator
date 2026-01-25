#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H
#include <cstdint>

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
  void setPWMFreq();
  void setPWM(uint8_t channel, uint16_t on, uint16_t off);
  void setAllPWM(uint16_t on, uint16_t off);

private:
  Raspi_I2C i2c;
  bool debug;

  static constexpr uint8_t MODE1 = 0x00;
  static constexpr uint8_t MODE2 = 0x01;
  static constexpr uint8_t PRESCALE = 0xFE;

  static constexpr uint8_t LED0_ON_L = 0x06;
  static constexpr uint8_t LED0_ON_H = 0x07;
  static constexpr uint8_t LED0_OFF_L = 0x08;
  static constexpr uint8_t LED0_OFF_H = 0x09;
  static constexpr uint8_t ALL_LED_ON_L  = 0xFA;
  static constexpr uint8_t ALL_LED_ON_H  = 0xFB;
  static constexpr uint8_t ALL_LED_OFF_L = 0xFC;
  static constexpr uint8_t ALL_LED_OFF_H = 0xFD;

  static constexpr uint8_t RESTART = 0x80;
  static constexpr uint8_t SLEEP   = 0x10;
  static constexpr uint8_t ALLCALL = 0x01;
  static constexpr uint8_t OUTDRV  = 0x04;
};

#endif //MOTOR_DRIVER_H
