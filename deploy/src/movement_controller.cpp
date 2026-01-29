//
// Created by autoboat on 21/12/25.
//

#include "movement_controller.h"

#include <iostream>
#include <cmath>

#include "motor_driver.h"


int rightMotorOutputCurve(int x) {
  double value = std::pow(1.23, (-x + 18)) * (-1) + 32.0;
  return std::lround(value);
}

int leftMotorOutputCurve(int x) {
  double value = std::pow(1.23, (x - 46))  *(-1) + 32.0;
  return static_cast<int>(std::lround(value));
}


movement_controller::movement_controller()
:  pid(1.0, 0.2, 0.05, 1.0, 63.0)
{
  try {
    pwm_driver = std::make_unique<motors::PCA9685>(0x6f, false);
  } catch (const std::runtime_error& e) {
    std::cerr << "Error initializing motor driver: " << e.what() << std::endl;
    pwm_driver = nullptr;
  }
  disable_motors();
}

void movement_controller::disable_motors() const{
  if (pwm_driver) {
    while (
      (pwm_driver->setMotorSpeed(motors::Left, 0) != 0) &&
      (pwm_driver->setMotorSpeed(motors::Right, 0) != 0)) {
      usleep(1e4);
    }
  }
}

void movement_controller::set_direction(int direction) {
  float control_value = static_cast<int>(std::lround(pid.compute(32, direction, 0.067)));
  /*std::cout << "Control value: " << control_value << std::endl;*/
  int left_value = leftMotorOutputCurve(control_value);
  int right_value = rightMotorOutputCurve(control_value);
  /*std::cout << "Left motor value: " << left_value << std::endl;
  std::cout << "Right motor value: " << right_value << std::endl;*/
  pwm_driver->setMotorSpeed(motors::Left, left_value);
  pwm_driver->setMotorSpeed(motors::Right, right_value);
}


void movement_controller::set_auto_control(int value) {
(void) value;
}

