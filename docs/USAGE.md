# Usage details

Long-form notes moved out of the README. Paths assume the workspace from the README quick start (`~/autoware-simulation-demo`).

## Verify nodes are running

```bash
# In a new terminal
ros2 node list

# Expected output:
# /joint_state_publisher
# /planning_simulator
# /robot_state_publisher
# /rviz2
# /sensor_simulator
# /vehicle_controller
```

## Check active topics

```bash
ros2 topic list

# Key topics:
# /cmd_vel          - Velocity commands
# /odom             - Vehicle odometry
# /planned_path     - Planned trajectory
# /points_raw       - LiDAR point cloud
# /image_raw        - Camera images
# /tf               - Transform tree
# /joint_states     - Vehicle joint states
```

## RViz Configuration

Follow these steps to properly configure RViz for visualization:

### 1. Set Fixed Frame
- Left panel → **Global Options**
- **Fixed Frame**: Change from `map` to **`odom`**

### 2. Add Robot Model (3D Vehicle)
- Click **Add** (bottom left)
- **By display type** tab
- Select **RobotModel**
- Click **OK**
- Settings:
  - **Description Topic**: `/robot_description`
  - **Visual Enabled**: ✓

### 3. Add Path Visualization
- Click **Add**
- **By topic** tab → `/planned_path` → **Path**
- Click **OK**
- Settings:
  - **Color**: Red (255, 0, 0)
  - **Line Width**: 0.05
  - **Alpha**: 1.0

### 4. Add LiDAR Point Cloud
- Click **Add**
- **By topic** → `/points_raw` → **PointCloud2**
- Click **OK**
- Settings:
  - **Size (m)**: 0.05
  - **Style**: Points
  - **Color Transformer**: FlatColor
  - **Color**: Green (0, 255, 0)

### 5. Add Odometry
- Click **Add**
- **By topic** → `/odom` → **Odometry**
- Settings:
  - **Keep**: 10
  - **Shape**: Arrow
  - **Color**: Red (255, 0, 0)
  - **Alpha**: 0.5

### 6. Add TF (Transform Frames)
- Click **Add**
- **By display type** → **TF**
- Settings:
  - **Show Names**: ✓
  - **Show Axes**: ✓
  - **Show Arrows**: ✓
  - **Marker Scale**: 1.0

### 7. Add Grid
- Click **Add**
- **By display type** → **Grid**
- Settings:
  - **Plane**: XY
  - **Cell Size**: 1.0
  - **Color**: Light gray (200, 200, 200)

### 8. Configure Camera View
- **Views** panel → Change to **ThirdPersonFollower**
- Settings:
  - **Target Frame**: `base_link`
  - **Distance**: 8.0 meters
  - **Yaw**: 0.785 (45°)
  - **Pitch**: 0.6

**Mouse Controls**:
- **Rotate**: Middle mouse button + drag
- **Zoom**: Scroll wheel
- **Pan**: Shift + middle mouse button + drag

## Testing and Interaction

### Sending Navigation Goals

Open a new terminal and source the workspace:

```bash
source ~/autoware-simulation-demo/install/setup.bash
```

#### Goal 1: Forward-Right (20, 5)
```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{
  header: {frame_id: 'odom'},
  pose: {
    position: {x: 20.0, y: 5.0, z: 0.0},
    orientation: {w: 1.0}
  }
}" --once
```

#### Goal 2: Backward-Left (-5, -5)
```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{
  header: {frame_id: 'odom'},
  pose: {
    position: {x: -5.0, y: -5.0, z: 0.0},
    orientation: {w: 1.0}
  }
}" --once
```

#### Goal 3: Return to Origin
```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{
  header: {frame_id: 'odom'},
  pose: {
    position: {x: 0.0, y: 0.0, z: 0.0},
    orientation: {w: 1.0}
  }
}" --once
```

#### Goal 4: Large Circle (10, 10)
```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{
  header: {frame_id: 'odom'},
  pose: {
    position: {x: 10.0, y: 10.0, z: 0.0},
    orientation: {w: 1.0}
  }
}" --once
```

### Manual Vehicle Control

#### Drive Forward
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 2.0},
  angular: {z: 0.0}
}"
# Press Ctrl+C to stop
```

#### Drive Backward
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: -1.0},
  angular: {z: 0.0}
}"
```

#### Turn Left While Moving
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 1.0},
  angular: {z: 0.5}
}"
```

#### Turn Right While Moving
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 1.0},
  angular: {z: -0.5}
}"
```

#### Spin in Place
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 0.0},
  angular: {z: 1.0}
}"
```

#### Emergency Stop
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 0.0},
  angular: {z: 0.0}
}" --once
```

### Square Path Pattern

Execute these goals in sequence (wait for each to complete):

```bash
# Point 1
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{header: {frame_id: 'odom'}, pose: {position: {x: 10.0, y: 0.0, z: 0.0}, orientation: {w: 1.0}}}" --once

# Point 2 (after arrival)
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{header: {frame_id: 'odom'}, pose: {position: {x: 10.0, y: 10.0, z: 0.0}, orientation: {w: 1.0}}}" --once

# Point 3
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{header: {frame_id: 'odom'}, pose: {position: {x: 0.0, y: 10.0, z: 0.0}, orientation: {w: 1.0}}}" --once

# Point 4 (back to origin)
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "{header: {frame_id: 'odom'}, pose: {position: {x: 0.0, y: 0.0, z: 0.0}, orientation: {w: 1.0}}}" --once
```

### Monitoring and Debugging

#### Monitor Odometry
```bash
ros2 topic echo /odom
```

#### Monitor Velocity Commands
```bash
ros2 topic echo /cmd_vel
```

#### Monitor Planned Path
```bash
ros2 topic echo /planned_path
```

#### Check Topic Frequencies
```bash
# Odometry frequency (should be ~50 Hz)
ros2 topic hz /odom

# LiDAR frequency (should be ~10 Hz)
ros2 topic hz /points_raw

# Camera frequency (should be ~30 Hz)
ros2 topic hz /image_raw
```

#### Check Topic Bandwidth
```bash
ros2 topic bw /points_raw
ros2 topic bw /image_raw
```

#### View Transform Tree
```bash
ros2 run tf2_tools view_frames
# This creates frames.pdf showing the transform hierarchy
evince frames.pdf
```

#### Node Information
```bash
# List all nodes
ros2 node list

# Get detailed info about a specific node
ros2 node info /autoware_sim_demo

# Check node parameters
ros2 param list /autoware_sim_demo
```

## Troubleshooting

### Build Failures

**Problem**: CMake errors or compilation failures

**Solution**:
```bash
# Clean build artifacts
cd ~/autoware-simulation-demo
rm -rf build install log

# Rebuild
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
```

**Problem**: Missing dependencies

**Solution**:
```bash
# Install missing ROS2 dependencies
rosdep install --from-paths src --ignore-src -r -y
```

### Launch Issues

**Problem**: `package 'autoware_sim_demo' not found`

**Solution**:
```bash
# Make sure workspace is sourced
source ~/autoware-simulation-demo/install/setup.bash

# Verify package exists
ros2 pkg list | grep autoware_sim_demo
```

**Problem**: URDF parsing errors

**Solution**:
- Check that `vehicle.urdf` has no XML comments with special characters
- Ensure xacro is installed: `sudo apt install ros-humble-xacro`

### Runtime Issues

**Problem**: Vehicle doesn't move

**Solution**:
- Check that planning node received odometry: `ros2 topic echo /odom`
- Verify goal was published: Check terminal output
- Goal might already be reached (within 0.5m tolerance)

**Problem**: No visualization in RViz

**Solution**:
- Verify Fixed Frame is set to `odom`
- Check that RobotModel display is added
- Ensure robot_state_publisher is running: `ros2 node list`

**Problem**: "Goal reached!" spam in console

**Solution**:
- Vehicle is at the default goal (5, 0)
- Send a new goal to a different location

### Performance Issues

**Problem**: Slow frame rate or lag

**Solution**:
- Reduce the point count or sensor rates: they are constants in `include/autoware_sim_demo/sensor_simulator.hpp` (`lidar_points_` 1000, `lidar_frequency_` 10 Hz, `camera_frequency_` 30 Hz); rebuild after editing. `sensor_kit.param.yaml` is not read by any node.
- Reduce RViz display elements

## Technical Details

### Architecture

The simulation uses a multi-node architecture:

1. **Vehicle Controller** (`vehicle_controller.cpp`)
   - Integrates a planar unicycle model (v, yaw rate) with a first-order lag on commands
   - Publishes odometry at 50 Hz
   - Subscribes to velocity commands
   - Broadcasts TF transforms

2. **Sensor Simulator** (`sensor_simulator.cpp`)
   - Generates synthetic LiDAR point clouds (10 Hz)
   - Publishes camera images (30 Hz)
   - Simulates sensor noise and patterns

3. **Planning Simulator** (`planning_simulator.cpp`)
   - Implements proportional controller
   - Generates straight-line paths to goals
   - Publishes velocity commands at 20 Hz
   - Monitors goal achievement

### Vehicle Dynamics

**Limits** (speeds and tolerance from `config/planning.param.yaml`; the wheelbase and dimensions come from `vehicle_info.param.yaml`, which no node reads, so they are reference values only):
- Wheelbase: 2.7m
- Max linear velocity: 2.0 m/s
- Max angular velocity: 1.0 rad/s
- Goal tolerance: 0.5m

**Vehicle Dimensions**:
- Length: 4.6m
- Width: 1.8m
- Height: 1.5m
- Wheel radius: 0.32m (18-inch wheels)

### Control Algorithm

Simple proportional controller:
- **Linear velocity**: Proportional to distance to goal
- **Angular velocity**: Proportional to heading error
- **Smoothing**: First-order low-pass filter on commands

### Coordinate Frames

Transform tree hierarchy:
```
odom
└── base_link
    ├── lidar
    ├── camera
    ├── front_left_wheel
    ├── front_right_wheel
    ├── rear_left_wheel
    ├── rear_right_wheel
    ├── hood
    ├── cabin
    ├── trunk
    └── [other vehicle components]
