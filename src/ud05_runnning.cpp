#include <rclcpp/rclcpp.hpp>
#include <stepper_interfaces/msg/stepper_msgs.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <unistd.h>
using namespace std;
using stepper_interfaces::msg::StepperMsgs;
using geometry_msgs::msg::Twist;

rclcpp::Node::SharedPtr g_node = nullptr;
//Set variable to global publish infomations
std_msgs::msg::String g_cmd;
StepperMsgs g_motorR;
StepperMsgs g_motorL;

void teleope(const Twist::SharedPtr msgs){
    g_motorR.step_speed = -msgs->linear.x;
    g_motorL.step_speed = msgs->linear.x;
    //Plus:turn right Minuse:turn left
    if((msgs->angular.z / 2.0) > 0.0){
        g_motorR.step_speed -= (msgs->angular.z / 2.0);
        g_motorR.step_speed += (msgs->angular.z / 2.0);
    }
    else if((msgs->angular.z / 2.0) < 0.0){
        g_motorR.step_speed += (msgs->angular.z / 2.0);
        g_motorR.step_speed -= (msgs->angular.z / 2.0);
    }
    if(msgs->linear.x > 100.0){
        g_motorR.step_speed = -100;
        g_motorL.step_speed = 100.0;
    }
    else if(msgs->linear.x < -100.0){
        g_motorR.step_speed = 100.0;
        g_motorL.step_speed = -100.0;
    }
    if(g_motorR.step_speed > 0.0){
        g_motorR.step_val = 1;
        g_motorR.step_speed = g_motorR.step_speed;
    }else if(g_motorR.step_speed < 0.0){
        g_motorR.step_val = -1;
        g_motorR.step_speed = g_motorR.step_speed * -1;
    }else{
        g_motorR.step_val = 0;
    }
    if(g_motorL.step_speed > 0.0){
        g_motorL.step_val = 1;
        g_motorL.step_speed = g_motorL.step_speed;
    }else if(g_motorL.step_speed < 0.0){
        g_motorL.step_val = -1;
        g_motorL.step_speed = g_motorL.step_speed * -1;
    }else{
        g_motorL.step_val = 0;
    }
}

int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    //Start UD_05 node
    g_node = rclcpp::Node::make_shared("UD_05");
    //Set publisher
    //Right motor
    auto pubCmdR = g_node ->create_publisher<std_msgs::msg::String>("interrCmdR", 10);
    auto pubStepperR = g_node->create_publisher<StepperMsgs>("stepperCmdR", 10);
    //Left motor
    auto pubCmdL = g_node->create_publisher<std_msgs::msg::String>("interrCmdL", 10);
    auto pubStepperL = g_node->create_publisher<StepperMsgs>("stepperCmdL", 10);
    RCLCPP_INFO(g_node->get_logger(), "Start ud05 package");
    //Init value
    StepperMsgs motorR;
    StepperMsgs motorL;
    motorR.step_specify = motorL.step_specify = false;
    motorR.step_speed = motorL.step_speed = 0.0;
    motorR.step_power = motorL.step_power = true;
    motorR.step_val = 0;
    motorL.step_val = 0;
    //Publish init values
    RCLCPP_INFO(g_node->get_logger(), "Publishing motor data");
    pubStepperR->publish(motorR);
    pubStepperL->publish(motorL);
    rclcpp::WallRate loop(1);
    loop.sleep();
    //Set subscriber
    auto subTwist = g_node->create_subscription<Twist>("cmd_vel", 10, teleope);
    //Set mult threed to callback function
    std::thread([]{rclcpp::spin(g_node);}).detach();
    while (rclcpp::ok()){
        if(motorR != g_motorR || motorL != g_motorL){
            motorR = g_motorR;
            motorL = g_motorL;
            pubStepperR->publish(motorR);
            pubStepperL->publish(motorL);
            loop.sleep();
            RCLCPP_INFO(g_node->get_logger(), "motorR specify:%d, step_val:%d, step_speed:%f, step_power:%d", motorR.step_specify, motorR.step_val, motorR.step_speed, motorR.step_power);
            RCLCPP_INFO(g_node->get_logger(), "motorL specify:%d, step_val:%d, step_speed:%f, step_power:%d", motorL.step_specify, motorL.step_val, motorL.step_speed, motorL.step_power);
        }
    }
    RCLCPP_INFO(g_node->get_logger(), "Finish ud05 loop");
    motorR.step_speed = motorL.step_speed = 0.0;
    pubStepperR->publish(motorR);
    pubStepperL->publish(motorL);
    loop.sleep();
    RCLCPP_INFO(g_node->get_logger(), "Finish ud05 package");
    rclcpp::shutdown();
    return 0;
}


