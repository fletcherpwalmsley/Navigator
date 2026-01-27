//
// Created by autoboat on 21/12/25.
//

#ifndef MOVEMENT_CONTROLLER_H
#define MOVEMENT_CONTROLLER_H

#include <memory>
#include <cstdint>

#include "motor_driver.h"

class movement_controller {
public:
    movement_controller();
    ~movement_controller(){disable_motors();}

    void enable_motors()const;
    void disable_motors()const;
    void set_direction(int direction);
    void step_direction();

    // Direct control of motors
    void set_auto_control(int value);
    void forward(int motor, int pwm_value);
    void backward(int motor, int pwm_value);
    void soft_step_motor(int motor, int pwm_value);


private:
  std::unique_ptr<PCA9685> pwm_driver;
  const uint8_t pwm_left_pin = 2; //13
  const uint8_t in1_left_pin = 4; //11
  const uint8_t in2_left_pin = 3; //12

  const uint8_t pwm_right_pin = 13; //2
  const uint8_t in1_right_pin = 11; //4
  const uint8_t in2_right_pin = 12; //3

  // Values for calculating a smooth transition
  // value can vary do from 0 to 51
  uint16_t current_value = 25;
  uint16_t target_value = 25;
  const uint16_t step = 1;

  // Manual Control toggle
  int auto_control = true;
  int last_direction = 0;
  uint16_t current_left_pwm = 0;
  uint16_t current_right_pwm = 0;
};



#endif //MOVEMENT_CONTROLLER_H
