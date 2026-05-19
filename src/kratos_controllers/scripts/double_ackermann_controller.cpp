// src/double_ackermann_controller.cpp
#include "kratos_controllers/double_ackermann_controller.hpp"
#include <cmath>
#include "pluginlib/class_list_macros.hpp"

namespace kratos_controllers
{
DoubleAckermannController::DoubleAckermannController() : controller_interface::ControllerInterface() {}

controller_interface::CallbackReturn DoubleAckermannController::on_init()
{
  velocity_command_subscriber_ = get_node()->create_subscription<geometry_msgs::msg::Twist>(
    "cmd_vel", 10, [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
      current_command_ = *msg;
    });
  return controller_interface::CallbackReturn::SUCCESS;
}

// Boilerplate interface configuration omitted for brevity (you link the 8 joints here)

controller_interface::return_type DoubleAckermannController::update(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  double v_x = current_command_.linear.x;
  double omega_z = current_command_.angular.z;

  // Prevent divide by zero if driving straight
  if (std::abs(omega_z) < 0.001) {
    // Write 0 to all steering joints, and v_x to all drive joints
    // ... command_interfaces_[...].set_value(...)
    return controller_interface::return_type::OK;
  }

  // Double Ackermann Math
  double R = v_x / omega_z;
  
  double delta_in = std::atan((wheelbase_ / 2.0) / (std::abs(R) - track_width_ / 2.0));
  double delta_out = std::atan((wheelbase_ / 2.0) / (std::abs(R) + track_width_ / 2.0));

  // Determine which wheels are "inner" based on turn direction (omega_z sign)
  // Calculate specific wheel velocities based on distance from ICR
  // Write the calculated angles and velocities to the hardware interfaces
  // e.g., command_interfaces_[0].set_value(delta_in); // Front-Left Steering

  return controller_interface::return_type::OK;
}
}  // namespace kratos_controllers

// Crucial: Export the class as a plugin
PLUGINLIB_EXPORT_CLASS(kratos_controllers::DoubleAckermannController, controller_interface::ControllerInterface)
