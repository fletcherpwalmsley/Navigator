//
// Created by autoboat on 29/01/26.
//
#pragma once

class PIDController {
public:
  PIDController(double kp, double ki, double kd,
                double output_min, double output_max)
      : kp_(kp), ki_(ki), kd_(kd),
        output_min_(output_min), output_max_(output_max),
        integral_(0.0), prev_error_(0.0) {}

  double compute(double setpoint, double measurement, double dt) {
    // Protect against bad dt (still RT-safe)
    if (dt <= 0.0) {
      return 0.0;
    }

    double error = setpoint - measurement;

    // Derivative (on error)
    double derivative = (error - prev_error_) / dt;

    // Integrate
    integral_ += error * dt;

    // Compute unclamped output
    double output =
        kp_ * error +
        ki_ * integral_ +
        kd_ * derivative;

    // Clamp output
    double clamped_output = clamp(output, output_min_, output_max_);

    // --- Anti-windup (integral clamping) ---
    if (output != clamped_output) {
      // Undo last integration step
      integral_ -= error * dt;
    }

    prev_error_ = error;
    return clamped_output;
  }

  void reset() {
    integral_ = 0.0;
    prev_error_ = 0.0;
  }

private:
  static double clamp(double value, double min, double max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
  }

  // Gains
  double kp_;
  double ki_;
  double kd_;

  // Output limits
  double output_min_;
  double output_max_;

  // State
  double integral_;
  double prev_error_;
};

