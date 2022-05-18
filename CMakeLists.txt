cmake_minimum_required(VERSION 3.5)
project(ud05_package)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(stepper_interfaces REQUIRED)
find_package(bipolar_stepper_package REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(ud05_runnning src/ud05_runnning)
ament_target_dependencies(ud05_runnning rclcpp std_msgs stepper_interfaces geometry_msgs)
install(TARGETS ud05_runnning DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY cfg DESTINATION share/${PROJECT_NAME}/)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)

ament_package()
