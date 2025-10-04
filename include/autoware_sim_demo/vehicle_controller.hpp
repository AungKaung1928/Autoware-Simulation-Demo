#pragma once

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <memory>

namespace autoware_sim_demo {

class VehicleController : public rclcpp::Node {
public:
    VehicleController();
    
private:
    void control_callback();
    void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);
    void publish_odometry();
    void publish_transform();
    
    // Publishers
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    
    // Subscribers
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    
    // Timer
    rclcpp::TimerBase::SharedPtr control_timer_;
    
    // Transform broadcaster
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    
    // Vehicle state
    double x_{0.0}, y_{0.0}, yaw_{0.0};
    double vx_{0.0}, vy_{0.0}, vyaw_{0.0};
    
    // Control commands
    double target_linear_{0.0}, target_angular_{0.0};
    
    // Parameters
    double wheel_base_{2.7};
    double control_frequency_{50.0};
};

}  // namespace autoware_sim_demo