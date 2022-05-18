from os.path import join
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_prefix = get_package_share_directory('ud05_package')
    ud05_node = Node(
        package = 'ud05_package',
        executable = 'ud05_runnning'
    )
    #return LaunchDescription([ud05_node])
    
    rightMotor = Node(
        package = 'bipolar_stepper_package',
        executable = 'bipolar_stepper',
        name = 'RightMotor',
        remappings=[('/stepperCmd', '/stepperCmdR'), ('/interrCmd', '/interrCmdR')],
        parameters=[join(pkg_prefix, 'cfg/right_motor_parameter.yaml')]
    )
    leftMotor = Node(
        package = 'bipolar_stepper_package',
        executable = 'bipolar_stepper',
        name = 'LeftMotor',
        remappings=[('/stepperCmd', '/stepperCmdL'), ('/interrCmd', '/interrCmdL')],
        parameters=[join(pkg_prefix, 'cfg/left_motor_parameter.yaml')]
    )
    return LaunchDescription([ud05_node, rightMotor, leftMotor ])
    
