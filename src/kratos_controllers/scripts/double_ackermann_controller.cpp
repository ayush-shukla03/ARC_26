#include "kratos_controllers/double_ackermann_controller.hpp"
#include <cmath>
#include <algorithm>
#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/logging.hpp"

namespace kratos_controllers
{

DoubleAckermannController::DoubleAckermannController() : controller_interface::ControllerInterface() {}

controller_interface::CallbackReturn DoubleAckermannController::on_init()
{
  try {
    // Subscribe to Nav2's cmd_vel topic
    velocity_command_subscriber_ = get_node()->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10, [this](const geometry_msgs::msg::Twist::SharedPtr msg) {
        current_command_ = *msg;
      });
  } catch (const std::exception & e) {
    fprintf(stderr, "Exception thrown during init stage with message: %s \n", e.what());
    return controller_interface::CallbackReturn::ERROR;
  }
  
  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration DoubleAckermannController::command_interface_configuration() const
{
  std::vector<std::string> conf_names;
  
  // These exact names must match the joints defined in your URDF and controllers.yaml
  // 4 Steering Joints (Position Control)
  conf_names.push_back("fl_steer_joint/position"); // Index 0
  conf_names.push_back("fr_steer_joint/position"); // Index 1
  conf_names.push_back("rl_steer_joint/position"); // Index 2
  conf_names.push_back("rr_steer_joint/position"); // Index 3

  // 4 Drive Joints (Velocity Control)
  conf_names.push_back("fl_drive_joint/velocity"); // Index 4
  conf_names.push_back("fr_drive_joint/velocity"); // Index 5
  conf_names.push_back("rl_drive_joint/velocity"); // Index 6
  conf_names.push_back("rr_drive_joint/velocity"); // Index 7

  return {controller_interface::interface_configuration_type::INDIVIDUAL, conf_names};
}

controller_interface::InterfaceConfiguration DoubleAckermannController::state_interface_configuration() const
{
  std::vector<std::string> conf_names;
  
  // We monitor the exact same joints we command
  conf_names.push_back("fl_steer_joint/position");
  conf_names.push_back("fr_steer_joint/position");
  conf_names.push_back("rl_steer_joint/position");
  conf_names.push_back("rr_steer_joint/position");

  conf_names.push_back("fl_drive_joint/velocity");
  conf_names.push_back("fr_drive_joint/velocity");
  conf_names.push_back("rl_drive_joint/velocity");
  conf_names.push_back("rr_drive_joint/velocity");

  return {controller_interface::interface_configuration_type::INDIVIDUAL, conf_names};
}

controller_interface::return_type DoubleAckermannController::update(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  // 1. Read the incoming command from Nav2
  double v_x = current_command_.linear.x;
  double omega_z = current_command_.angular.z;
  
  // Physical rover properties (matching your URDF)
  double L = 0.4; // Wheelbase
  double W = 0.6; // Track width
  double wheel_radius = 0.12;

  // Variables to hold calculated targets
  double steer_fl = 0.0, steer_fr = 0.0, steer_rl = 0.0, steer_rr = 0.0;
  double drive_fl = 0.0, drive_fr = 0.0, drive_rl = 0.0, drive_rr = 0.0;

  // 2. Handle Straight Driving (Prevent divide by zero)
  if (std::abs(omega_z) < 0.001) {
    steer_fl = steer_fr = steer_rl = steer_rr = 0.0;
    
    // v = r * omega -> omega = v / r
    double base_wheel_rpm = v_x / wheel_radius;
    drive_fl = drive_fr = drive_rl = drive_rr = base_wheel_rpm;
  } 
  // 3. Handle Turning (Double Ackermann Kinematics)
  else {
    // Calculate turning radius to the center of the chassis
    double R = v_x / omega_z;

    // A. CALCULATE STEERING ANGLES
    steer_fl = std::atan2(L / 2.0, R - W / 2.0);
    steer_fr = std::atan2(L / 2.0, R + W / 2.0);
    
    // Rear wheels mirror the front wheels
    steer_rl = -steer_fl;
    steer_rr = -steer_fr;

    // B. CALCULATE WHEEL SPEEDS
    double dist_y_left = R - W / 2.0;
    double dist_y_right = R + W / 2.0;

    double hypot_left = std::hypot(L / 2.0, dist_y_left);
    double hypot_right = std::hypot(L / 2.0, dist_y_right);

    double v_mag_left = std::abs(v_x) * (hypot_left / std::abs(R));
    double v_mag_right = std::abs(v_x) * (hypot_right / std::abs(R));

    double v_fl = std::copysign(v_mag_left, v_x);
    double v_fr = std::copysign(v_mag_right, v_x);
    
    // Rear wheels travel the same radius as front wheels
    double v_rl = v_fl;
    double v_rr = v_fr;

    // Convert linear wheel speeds (m/s) to rotational speeds (rad/s)
    drive_fl = v_fl / wheel_radius;
    drive_fr = v_fr / wheel_radius;
    drive_rl = v_rl / wheel_radius;
    drive_rr = v_rr / wheel_radius;
  }

  // 4. Send Commands to Hardware Interfaces
  // Make sure to match the indices defined in command_interface_configuration()
  command_interfaces_[0].set_value(steer_fl);
  command_interfaces_[1].set_value(steer_fr);
  command_interfaces_[2].set_value(steer_rl);
  command_interfaces_[3].set_value(steer_rr);

  command_interfaces_[4].set_value(drive_fl);
  command_interfaces_[5].set_value(drive_fr);
  command_interfaces_[6].set_value(drive_rl);
  command_interfaces_[7].set_value(drive_rr);

  return controller_interface::return_type::OK;
}

}  // namespace kratos_controllers

// Crucial: Export the class as a plugin so controller_manager can find it
PLUGINLIB_EXPORT_CLASS(kratos_controllers::DoubleAckermannController, controller_interface::ControllerInterface)