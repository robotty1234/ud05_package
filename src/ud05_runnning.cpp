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
bool flug = false;

void teleope(const Twist::SharedPtr msgs){
    flug = true;
    float speed_R = msgs->linear.x;
    float speed_L = (-1) * msgs->linear.x;
    //RCLCPP_INFO(g_node->get_logger(), "speed_R:%f, speed_L:%f", speed_R, speed_L);
    //Plus:turn right Minuse:turn left
    if(msgs->angular.z > 0.0){
        if(msgs->linear.x == 0.0){
            //Super-credit turn right
            speed_R = msgs->angular.z;
            speed_L = msgs->angular.z;
            //RCLCPP_INFO(g_node->get_logger(), "Super-credit turn right");
        }else{
            //Turn right
            speed_R = 0.0;
            speed_L = msgs->angular.z;
            //RCLCPP_INFO(g_node->get_logger(), "Turn right");
        }
    }else if(msgs->angular.z < 0.0){
        if(msgs->linear.x == 0.0){
            //Super-credit turn left
            speed_R = msgs->angular.z;
            speed_L = msgs->angular.z;
            //RCLCPP_INFO(g_node->get_logger(), "Super-credit turn left");
        }else{
            //Turn left
            speed_R = msgs->angular.z;
            speed_L = 0.0;
            //RCLCPP_INFO(g_node->get_logger(), "Turn left");
        }
    }
    if(speed_R > 100.0){
        speed_R = 100.0;
    }
    else if(speed_R < -100.0){
        speed_R = -100.0;
    }
    if(speed_L > 100.0){
        speed_L = 100.0;
    }
    else if(speed_L < -100.0){
        speed_L = -100.0;
    }
    //RCLCPP_INFO(g_node->get_logger(), "speed_R:%f, speed_L:%f", speed_R, speed_L);
    if(speed_R > 0.0){
        g_motorR.step_val = 1;
        g_motorR.step_speed = speed_R;
    }else if(speed_R < 0.0){
        g_motorR.step_val = -1;
        g_motorR.step_speed = speed_R * -1;
    }else{
        g_motorR.step_val = 0;
        g_motorR.step_speed = 0.0;
    }
    if(speed_L > 0.0){
        g_motorL.step_val = 1;
        g_motorL.step_speed = speed_L;
    }else if(speed_L < 0.0){
        g_motorL.step_val = -1;
        g_motorL.step_speed = speed_L * -1;
    }else{
        g_motorL.step_val = 0;
        g_motorL.step_speed = 0.0;
    }
    flug = false;
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
    rclcpp::WallRate loop(1);
    pubStepperR->publish(motorR);
    pubStepperL->publish(motorL);
    loop.sleep();
    //Set subscriber
    auto subTwist = g_node->create_subscription<Twist>("cmd_vel", 10, teleope);
    //Set mult threed to callback function
    std::thread([]{rclcpp::spin(g_node);}).detach();
    while (rclcpp::ok()){
        if(flug == false){
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


