//
// Created by autoboat on 21/12/25.
//

#include "movement_controller.h"
#include "motor_driver.h"

movement_controller::movement_controller() {
  try {
    pwm_driver = std::make_unique<PCA9685>(0x6f, false);
    pwm_driver->setPWMFreq();
  } catch (const std::runtime_error&) {
    pwm_driver = nullptr;
  }

  disable_motors();
}

void movement_controller::disable_motors() const{
  if (pwm_driver) {
    pwm_driver->setPWM(pwm_left_pin, 0, 0);
    pwm_driver->setPWM(pwm_right_pin, 0, 0);
    pwm_driver->setPWM(in1_left_pin, 0, 4096);
    pwm_driver->setPWM(in2_left_pin, 0, 4096);
    pwm_driver->setPWM(in1_right_pin, 0, 4096);
    pwm_driver->setPWM(in2_right_pin, 0, 4096);
  }
}


void movement_controller::enable_motors() const{
  if (pwm_driver) {
    pwm_driver->setPWM(in1_left_pin, 4096, 0);
    pwm_driver->setPWM(in2_left_pin, 0, 4096);
    pwm_driver->setPWM(in1_right_pin, 4096, 0);
    pwm_driver->setPWM(in2_right_pin, 0, 4096);
  }
}

void movement_controller::set_direction(int direction) {
  if (direction > 51) {
    target_value = 51;
  } else {
    target_value = direction;
  }
}

void movement_controller::step_direction() {

  if (pwm_driver == nullptr) {
    return;
  }

  if ( (target_value == current_value) || (auto_control == false)) {
      return;
  }

  if (target_value > current_value) {
    if (target_value >= 51) {
      target_value = 51;
    }
    else {
      current_value += step;
    }
  }

  else {
    if (target_value <= step) {
      target_value = 0;
    } else {
      current_value -= step;
    }
  }

  /*std::cout << "Current Value: " << current_value << " Target Value: " << target_value <<  std::endl;*/
  uint16_t right_value = current_value * 5;

  if (right_value > 255) {
    right_value = 255;
  }

  uint16_t left_value = 255 - right_value;

  // Final values
  right_value = right_value * 16;
  left_value = left_value * 16;

  /*std::cout << "Left: " << left_value << " Right: " << right_value << std::endl;*/

  pwm_driver->setPWM(pwm_left_pin, 0, left_value);
  pwm_driver->setPWM(pwm_right_pin, 0, right_value);
}

void movement_controller::set_auto_control(int value) {
  auto_control = value;

  if (auto_control == 1) {
    enable_motors();
    last_direction = 0;
  }
  else if (auto_control == 0) {
    disable_motors();
  }
}

void movement_controller::forward(int motor, int pwm_value){
  if (pwm_driver) {
    if (last_direction != 1) {
      pwm_driver->setPWM(in1_left_pin, 4096, 0);
      pwm_driver->setPWM(in2_left_pin, 0, 4096);

      pwm_driver->setPWM(in1_right_pin, 4096, 0);
      pwm_driver->setPWM(in2_right_pin, 0, 4096);\
      last_direction = 1;

      if (motor == 0) {
        current_left_pwm = 0;
      }
      if (motor == 1) {
        current_right_pwm = 0;
      }
    }
    soft_step_motor(motor, pwm_value);
  }
}


void movement_controller::backward(int motor, int pwm_value){
  if (pwm_driver) {
    if (last_direction != 2) {
      pwm_driver->setPWM(in2_left_pin, 4096, 0);
      pwm_driver->setPWM(in1_left_pin, 0, 4096);

      pwm_driver->setPWM(in2_right_pin, 4096, 0);
      pwm_driver->setPWM(in1_right_pin, 0, 4096);
      last_direction = 2;

      if (motor == 0) {
        current_left_pwm = 0;
      }
      if (motor == 1) {
        current_right_pwm = 0;
      }
    }
    soft_step_motor(motor, pwm_value);
  }
}

void movement_controller::soft_step_motor(int motor, int pwm_value) {
  if (pwm_driver) {
    if (motor == 0) {
      if (pwm_value > current_left_pwm) {
        while (pwm_value > (current_left_pwm+=10)) {
          if (current_left_pwm >= 254) {
            current_left_pwm = 255;
            break;
          }
          pwm_driver->setPWM(pwm_left_pin, 0, current_left_pwm*16);
          usleep(30);
        }

      } else if (pwm_value < current_left_pwm) {
        while (pwm_value < (current_left_pwm-=10)) {
          if (current_left_pwm <= 1) {
            current_left_pwm = 0;
            break;
          }
          pwm_driver->setPWM(pwm_left_pin, 0, current_left_pwm*16);
          usleep(30);
        }
      }
    }

    else if (motor == 1) {
      if (pwm_value > current_right_pwm) {
        while (pwm_value > (current_right_pwm+=10)) {
          if (current_right_pwm >= 254) {
            current_right_pwm = 255;
            break;
          }
          pwm_driver->setPWM(pwm_right_pin, 0, current_right_pwm*16);
          usleep(30);
        }

      } else if (pwm_value < current_right_pwm) {
        while (pwm_value < (current_right_pwm-=10)) {
          if (current_right_pwm <= 1) {
            current_right_pwm = 0;
            break;
          }
          pwm_driver->setPWM(pwm_right_pin, 0, current_right_pwm*16);
          usleep(30);
        }
      }
    }
  }
}