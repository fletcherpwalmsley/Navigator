//
// Created by autoboat on 21/12/25.
//

#ifndef MOVEMENT_CONTROLLER_H
#define MOVEMENT_CONTROLLER_H

#include <memory>
#include <cstdint>

#include "motor_driver.h"
#include "pid.h"

class movement_controller {
public:
    movement_controller();
    ~movement_controller(){disable_motors();}

    void disable_motors()const;
    void set_direction(int direction);

    // Direct control of motors
    void set_auto_control(int value);


private:
  std::unique_ptr<motors::PCA9685> pwm_driver;

  PIDController pid;


  // Values for calculating a smooth transition
  // value can vary do from 0 to 51
  uint16_t current_value = 25;
  uint16_t target_value = 25;
  const uint16_t step = 1;

  // Manual Control toggle
  int auto_control = true;
  int last_direction = 0;
};



#endif //MOVEMENT_CONTROLLER_H
