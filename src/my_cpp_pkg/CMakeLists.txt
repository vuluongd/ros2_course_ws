cmake_minimum_required(VERSION 3.8)
project(my_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(my_robot_interfaces REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Nodes
add_executable(my_first_node src/my_first_node.cpp)
ament_target_dependencies(my_first_node
  rclcpp
)

add_executable(robot_news_station src/robot_news_station.cpp)
ament_target_dependencies(robot_news_station
  rclcpp
  std_msgs
)

add_executable(smartphone src/smartphone.cpp)
ament_target_dependencies(smartphone
  rclcpp
  std_msgs
)

add_executable(service_server src/service_server.cpp)
ament_target_dependencies(service_server
  rclcpp
  my_robot_interfaces
)

add_executable(service_client src/service_client.cpp)
ament_target_dependencies(service_client
  rclcpp
  my_robot_interfaces
)

# Install
install(TARGETS
  my_first_node
  robot_news_station
  smartphone
  service_server
  service_client
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()