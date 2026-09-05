#!/usr/bin/env python3
"""Vehicle kinematics + fake sensors + goal-seeking planner, plus the URDF and optional RViz.

The simulation executable hosts three nodes (vehicle_controller, sensor_simulator,
planning_simulator). It must not get a `name` here: a node-name remap applies to every node in
the process and made all three collide on rosout.
"""
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg = get_package_share_directory('autoware_sim_demo')
    use_rviz = LaunchConfiguration('use_rviz')
    return LaunchDescription([
        DeclareLaunchArgument('use_rviz', default_value='true'),
        DeclareLaunchArgument(
            'params_file',
            default_value=os.path.join(pkg, 'config', 'planning.param.yaml'),
            description='planner / vehicle loop parameters'),
        Node(
            package='robot_state_publisher', executable='robot_state_publisher', output='screen',
            parameters=[{'robot_description': Command(['xacro ', os.path.join(pkg, 'urdf', 'vehicle.urdf')])}]),
        Node(
            package='joint_state_publisher', executable='joint_state_publisher', output='screen',
            parameters=[{'use_gui': False}]),
        Node(
            package='autoware_sim_demo', executable='autoware_sim_demo', output='screen',
            parameters=[LaunchConfiguration('params_file'),
                        os.path.join(pkg, 'config', 'vehicle_info.param.yaml'),
                        os.path.join(pkg, 'config', 'sensor_kit.param.yaml')]),
        Node(
            package='rviz2', executable='rviz2', output='screen',
            arguments=['-d', os.path.join(pkg, 'rviz', 'autoware_sim.rviz')],
            condition=IfCondition(use_rviz)),
    ])
