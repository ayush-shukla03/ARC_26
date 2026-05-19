// include/kratos_controllers/double_ackermann_controller.hpp
#ifndef KRATOS_CONTROLLERS__DOUBLE_ACKERMANN_CONTROLLER_HPP_
#define KRATOS_CONTROLLERS__DOUBLE_ACKERMANN_CONTROLLER_HPP_

#include "controller_interface/controller_interface.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/subscription.hpp"

namespace kratos_controllers
{
class DoubleAckermannController : public controller_interface::ControllerInterface
{
public:
  DoubleAckermannController();
  
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;
  
  controller_interface::return_type update(const rclcpp::Time & time, const rclcpp::Duration & period) override;
  controller_interface::CallbackReturn on_init() override;

private:
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr velocity_command_subscriber_;
  geometry_msgs::msg::Twist current_command_;
  
  // Rover dimensions
  double wheelbase_ = 0.5; 
  double track_width_ = 0.4;
};
}  // namespace kratos_controllers
#endif
