#include <rclcpp/rclcpp.hpp>
#include <stepper_interfaces/msg/stepper_msgs.hpp>
#include <std_msgs/msg/string.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <unistd.h>
using namespace std;
using stepper_interfaces::msg::StepperMsgs;

int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("UD_05");
    auto pubCmdR = node->create_publisher<std_msgs::msg::String>("interrCmdR", 10);
    auto pubStepperR = node->create_publisher<StepperMsgs>("stepperCmdR", 10);
    auto pubCmdL = node->create_publisher<std_msgs::msg::String>("interrCmdL", 10);
    auto pubStepperL = node->create_publisher<StepperMsgs>("stepperCmdL", 10);
    RCLCPP_INFO(node->get_logger(), "Start ud05 package");
    rclcpp::WallRate loop(1);
    std_msgs::msg::String cmd;
    StepperMsgs motorR;
    StepperMsgs motorL;
    motorR.step_specify = motorL.step_specify = false;
    motorR.step_speed = motorL.step_speed = 50.0;
    motorR.step_power = motorL.step_power = false;
    motorR.step_val = 1;
    motorL.step_val = -1;
    cmd.data = "STOP";
    sleep(2);
    RCLCPP_INFO(node->get_logger(), "Publishing motor data");
    RCLCPP_INFO(node->get_logger(), "motorR specify:%d, step_val:%d, step_speed:%f, step_power:%d", motorR.step_specify, motorR.step_val, motorR.step_speed, motorR.step_power);
    RCLCPP_INFO(node->get_logger(), "motorL specify:%d, step_val:%d, step_speed:%f, step_power:%d", motorL.step_specify, motorL.step_val, motorL.step_speed, motorL.step_power);
    pubStepperR->publish(motorR);
    pubStepperL->publish(motorL);
    loop.sleep();
    sleep(3);
    pubCmdR->publish(cmd);
    pubCmdL->publish(cmd);
    loop.sleep();
    RCLCPP_INFO(node->get_logger(), "Finish ud05 package");
    rclcpp::shutdown();
    return 0;
}


