#pragma once

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav_msgs/msg/path.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <memory>
#include <vector>

namespace autoware_sim_demo {

class PlanningSimulator : public rclcpp::Node {
public:
    PlanningSimulator();
    
private:
    void planning_callback();
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg);
    void goal_callback(const geometry_msgs::msg::PoseStamped::SharedPtr msg);
    
    void generate_simple_path();
    void calculate_control_command();
    
    // Publishers
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    
    // Subscribers
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr goal_sub_;
    
    // Timer
    rclcpp::TimerBase::SharedPtr planning_timer_;
    
    // Current state
    geometry_msgs::msg::Pose current_pose_;
    bool has_pose_{false};
    
    // Target state
    geometry_msgs::msg::Pose target_pose_;
    bool has_target_{false};
    
    // Path
    nav_msgs::msg::Path current_path_;
    
    // Parameters
    double planning_frequency_{20.0};
    double max_linear_speed_{2.0};
    double max_angular_speed_{1.0};
    double goal_tolerance_{0.5};
    double linear_gain_{0.5};
    double angular_gain_{2.0};
};

}  // namespace autoware_sim_demo