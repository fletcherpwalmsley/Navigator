/*
 * PCA9685 16-Channel PWM Driver
 * Single-file Linux implementation
 * Python-compatible Raspi_I2C helpers
 */

#include <string>
#include <iostream>
#include <cstdio>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>

#include "motor_driver.h"

/* ============================================================
 * Raspi_I2C – Python-compatible I2C helper
 * ============================================================ */

Raspi_I2C::Raspi_I2C(uint8_t addr, const std::string& dev)
        : address(addr) {
      file = open(dev.c_str(), O_RDWR);
      if (file < 0) {
        throw std::runtime_error("Failed to open I2C device");
      }
      if (ioctl(file, I2C_SLAVE, address) < 0) {
        throw std::runtime_error("Failed to select I2C slave");
      }
      std::cout << "Accessed I2C device at address 0x" << std::hex << int(address) << std::dec << std::endl;
}

Raspi_I2C::~Raspi_I2C() {
    if (file >= 0)
        close(file);
}

int Raspi_I2C::errMsg() const {
    std::cerr << "Error accessing 0x"
              << std::hex << int(address)
              << ": Check your I2C address"
              << std::dec << std::endl;
    return -1;
}

int Raspi_I2C::write8(uint8_t reg, uint8_t value) const {
    uint8_t buf[2] = { reg, value };
    if (::write(file, buf, 2) != 2)
        return errMsg();

    if (debug)
        printf("I2C: Wrote 0x%02X to register 0x%02X\n", value, reg);
    return 0;
}

int Raspi_I2C::readU8(uint8_t reg) const  {
    if (::write(file, &reg, 1) != 1)
        return errMsg();

    uint8_t result;
    if (::read(file, &result, 1) != 1)
        return errMsg();

    if (debug)
        printf("I2C: Device 0x%02X returned 0x%02X from reg 0x%02X\n",
               address, result, reg);
    return result;
}


/* ============================================================
 * PCA9685 PWM Driver
 * ============================================================ */
PCA9685::PCA9685(uint8_t address, bool dbg)
    : i2c(address), debug(dbg)
{
    i2c.debug = debug;

    setAllPWM(0, 0);
    i2c.write8(MODE2, OUTDRV);
    i2c.write8(MODE1, ALLCALL);

    usleep(5000);

    uint8_t mode1 = i2c.readU8(MODE1);
    mode1 &= ~SLEEP;
    i2c.write8(MODE1, mode1);

    usleep(5000);
}

void PCA9685::setPWMFreq() {
    uint8_t prescale = 3;
    uint8_t oldmode = i2c.readU8(MODE1);
    uint8_t newmode = (oldmode & 0x7F) | SLEEP;

    i2c.write8(MODE1, newmode);
    i2c.write8(PRESCALE, prescale);
    i2c.write8(MODE1, oldmode);

    usleep(5000);
    i2c.write8(MODE1, oldmode | RESTART);
}

void PCA9685::setPWM(uint8_t channel, uint16_t on, uint16_t off) {
        i2c.write8(LED0_ON_L+4*channel, on & 0xFF);
        i2c.write8(LED0_ON_H+4*channel, on >> 8);
        i2c.write8(LED0_OFF_L+4*channel, off & 0xFF);
        i2c.write8(LED0_OFF_H+4*channel, off >> 8);
    }

void PCA9685::setAllPWM(uint16_t on, uint16_t off) {
    i2c.write8(ALL_LED_ON_L,  on & 0xFF);
    i2c.write8(ALL_LED_ON_H,  on >> 8);
    i2c.write8(ALL_LED_OFF_L, off & 0xFF);
    i2c.write8(ALL_LED_OFF_H, off >> 8);
}


/* ============================================================
 * Example usage
 * ============================================================ */
int main_2() {

    const int pwm_pin_2 = 13;
    const int in2_2 = 12;
    const int in1_2 = 11;

    const int pwm_pin_3 = 2;
    const int in2_3 = 3;
    const int in1_3 = 4;

    try {
        PCA9685 pwm(0x6f, true);

        pwm.setPWMFreq();      // Servo frequency

        // M2 (Right motor)
        pwm.setPWM(pwm_pin_2, 0, 4080);
        pwm.setPWM(in2_2, 0, 4096);
        pwm.setPWM(in1_2, 4096, 0);

        // M3 (Left Motor)
        pwm.setPWM(pwm_pin_3, 0, 4080);
        pwm.setPWM(in2_3, 0, 4096);
        pwm.setPWM(in1_3, 4096, 0);

        sleep(2);
        pwm.setPWM(pwm_pin_2, 0, 2040);
        pwm.setPWM(pwm_pin_3, 0, 2040);

        sleep(2);
        pwm.setPWM(pwm_pin_2, 0, 1020);
        pwm.setPWM(pwm_pin_3, 0, 1020);

        sleep(2);
        pwm.setPWM(pwm_pin_2, 0, 510);
        pwm.setPWM(pwm_pin_3, 0, 510);

        sleep(2);
        pwm.setPWM(in2_2, 0, 4096);
        pwm.setPWM(in1_2, 0, 4096);
        pwm.setPWM(in2_3, 0, 4096);
        pwm.setPWM(in1_3, 0, 4096);

        // Speed set with variing the large value by (0-255)*16 on the PWM pin
        // Flip in1 and 2 values for reverse
        // Set both the same (exmaple uses 0, 4096) for Release

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
