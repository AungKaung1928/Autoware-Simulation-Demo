"""Launch test of the shipped pipeline: launch/basic_simulation.launch.py with use_rviz:=false.

That starts robot_state_publisher, joint_state_publisher and the demo process (vehicle,
sensor and planning nodes). The test sends one goal on /goal_pose and checks the closed loop
planner -> /cmd_vel -> vehicle -> /odom brings the vehicle inside goal_tolerance
(0.5 m, config/planning.param.yaml) and that the planner then commands zero velocity.
Run: launch_test test/test_launch_pipeline.py (after sourcing the workspace).
"""
import math
import os
import time
import unittest

import launch
import launch_testing
import launch_testing.actions
import pytest
import rclpy
from ament_index_python.packages import get_package_share_directory
from geometry_msgs.msg import PoseStamped, Twist
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from nav_msgs.msg import Odometry

GOAL = (3.0, 2.0)
TOLERANCE = 0.5


@pytest.mark.launch_test
def generate_test_description():
    shipped = os.path.join(
        get_package_share_directory('autoware_sim_demo'), 'launch', 'basic_simulation.launch.py')
    return launch.LaunchDescription([
        IncludeLaunchDescription(PythonLaunchDescriptionSource(shipped),
                                 launch_arguments={'use_rviz': 'false'}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


class TestPipeline(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_goal_reached_then_stop(self):
        probe = rclpy.create_node('pipeline_probe')
        odom, cmds = [], []
        probe.create_subscription(Odometry, '/odom', odom.append, 10)
        probe.create_subscription(Twist, '/cmd_vel', cmds.append, 10)
        goal_pub = probe.create_publisher(PoseStamped, '/goal_pose', 10)
        goal = PoseStamped()
        goal.header.frame_id = 'odom'
        goal.pose.position.x, goal.pose.position.y = GOAL
        goal.pose.orientation.w = 1.0

        def dist():
            p = odom[-1].pose.pose.position
            return math.hypot(p.x - GOAL[0], p.y - GOAL[1])

        try:
            start = time.monotonic()
            while not odom and time.monotonic() - start < 30.0:
                rclpy.spin_once(probe, timeout_sec=0.1)
            self.assertTrue(odom, 'no /odom: the pipeline did not start')
            reached = None
            start = time.monotonic()
            while time.monotonic() - start < 40.0:
                goal_pub.publish(goal)
                rclpy.spin_once(probe, timeout_sec=0.1)
                if odom and dist() < TOLERANCE:
                    reached = time.monotonic() - start
                    break
            self.assertIsNotNone(reached, f'not within {TOLERANCE} m of {GOAL} in 40 s; last distance {dist():.2f} m')
            cmds.clear()
            end = time.monotonic() + 1.0
            while time.monotonic() < end:
                rclpy.spin_once(probe, timeout_sec=0.1)
            self.assertTrue(cmds and all(c.linear.x == 0.0 and c.angular.z == 0.0 for c in cmds[-3:]),
                            'planner still commands motion inside the goal tolerance')
            print(f'goal {GOAL} reached in {reached:.1f} s, final distance {dist():.2f} m')
        finally:
            probe.destroy_node()
