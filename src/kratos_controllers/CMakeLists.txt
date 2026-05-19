cmake_minimum_required(VERSION 3.8)
project(kratos_controllers)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(geometry_msgs REQUIRED)

# 1. Compile the C++ file into a shared library named after the package
add_library(${PROJECT_NAME} SHARED
  scripts/double_ackermann_controller.cpp
)

# 2. Tell CMake where to find the new Header file
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 3. Link the ROS 2 dependencies
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  controller_interface
  pluginlib
  geometry_msgs
)

# 4. Install the compiled library (.so) so the controller_manager can find it!
install(TARGETS ${PROJECT_NAME}
  DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include
)

# 5. Export the plugin XML
pluginlib_export_plugin_description_file(controller_interface kratos_controllers.xml)

ament_package()