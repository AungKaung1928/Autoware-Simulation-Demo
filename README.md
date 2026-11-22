# Autoware Simulation Demo

A small ROS 2 Humble / C++17 vehicle simulation that uses the Autoware topic layout (`/points_raw`, `/image_raw`, `/odom`, `/goal_pose`): a planar kinematic vehicle with first-order command lag, placeholder LiDAR and camera publishers, and a proportional goal-seeking planner, shown in RViz with a sedan URDF. It does not depend on or run Autoware; it is a teaching scaffold for the message flow, not a physics or planning stack.

Verified 2026-09-05 (headless, no RViz): goal (20, 5) from the origin reached in 11 s, held at 0.44 m (tolerance 0.5 m), all three nodes visible in `ros2 node list`.

## Quick start

Requires Ubuntu 22.04 with ROS 2 Humble, colcon and a C++17 compiler; `rosdep install` brings `rviz2`, `robot_state_publisher`, `joint_state_publisher` and `xacro`, which the launch file starts. No screenshot here: RViz needs a ROS 2 desktop to capture.

```bash
mkdir -p ~/autoware-simulation-demo/src
git clone <this repo URL> ~/autoware-simulation-demo/src/autoware_sim_demo
cd ~/autoware-simulation-demo
rosdep install --from-paths src --ignore-src -y     # first time only
colcon build --packages-select autoware_sim_demo
source install/setup.bash
ros2 launch autoware_sim_demo basic_simulation.launch.py   # RViz opens; the car drives to the default target (5, 0)
```

Send a goal from a second terminal (after `source install/setup.bash`); the car drives there and stops within 0.5 m:

```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{header: {frame_id: 'odom'}, pose: {position: {x: 20.0, y: 5.0}, orientation: {w: 1.0}}}" --once
```

## How it works

| node | does | rate |
|---|---|---|
| `vehicle_controller` | integrates v and yaw rate from `/cmd_vel` with a first-order lag, publishes `/odom` and TF | 50 Hz |
| `sensor_simulator` | random-point `PointCloud2` on `/points_raw`, blank `Image` on `/image_raw` | 10 Hz / 30 Hz |
| `planning_simulator` | straight-line `/planned_path` to `/goal_pose`, proportional control on distance and heading | 20 Hz |

Gains, speed caps (2.0 m/s, 1.0 rad/s) and the goal tolerance live in `config/planning.param.yaml`. Rates are the defaults in `include/autoware_sim_demo/*.hpp`.

Test without RViz: `cd src/autoware_sim_demo && launch_test test/test_launch_pipeline.py` (after `source install/setup.bash`) runs the shipped launch file with `use_rviz:=false`, sends a goal at (3.0, 2.0) and checks the loop gets inside the 0.5 m tolerance and then stops. In CI (`ros:humble`, every push): reached in 3.7 s, final distance 0.44 m.

Limits: unicycle model, not a steering (bicycle) model; the sensors are placeholders, nothing consumes their data; the planner drives straight at the goal with no obstacle checks.

More: RViz setup, manual `/cmd_vel` driving, monitoring commands and troubleshooting in [docs/USAGE.md](docs/USAGE.md).

## License

Apache-2.0, see LICENSE.
