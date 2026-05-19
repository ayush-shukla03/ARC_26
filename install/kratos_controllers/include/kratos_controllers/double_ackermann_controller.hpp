#ifndef KRATOS_CONTROLLERS__DOUBLE_ACKERMANN_CONTROLLER_HPP_
#define KRATOS_CONTROLLERS__DOUBLE_ACKERMANN_CONTROLLER_HPP_

#include "controller_interface/controller_interface.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

namespace kratos_controllers
{

class DoubleAckermannController : public controller_interface::ControllerInterface
{
public:
  DoubleAckermannController();

  controller_interface::InterfaceConfiguration command_interface_configuration() const override;
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;
  controller_interface::CallbackReturn on_init() override;
  controller_interface::return_type update(const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
  // Listens to keyboard/Nav2 commands
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr velocity_command_subscriber_;
  geometry_msgs::msg::Twist current_command_;
};

}  // namespace kratos_controllers

#endif  // KRATOS_CONTROLLERS__DOUBLE_ACKERMANN_CONTROLLER_HPP_