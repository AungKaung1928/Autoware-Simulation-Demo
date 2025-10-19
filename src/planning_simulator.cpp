#include "autoware_sim_demo/planning_simulator.hpp"
#include <cmath>

namespace autoware_sim_demo {

PlanningSimulator::PlanningSimulator() : Node("planning_simulator") {
    
    // Publishers
    cmd_vel_pub_ = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    path_pub_ = create_publisher<nav_msgs::msg::Path>("/planned_path", 10);
    
    // Subscribers
    odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
        "/odom", 10,
        std::bind(&PlanningSimulator::odom_callback, this, std::placeholders::_1));
        
    goal_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>(
        "/goal_pose", 10,
        std::bind(&PlanningSimulator::goal_callback, this, std::placeholders::_1));
    
    // Timer for the planning loop
    planning_timer_ = create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(1000.0 / planning_frequency_)),
        std::bind(&PlanningSimulator::planning_callback, this));
    
    // Set default target (simple circle)
    target_pose_.position.x = 5.0;
    target_pose_.position.y = 0.0;
    target_pose_.position.z = 0.0;
    target_pose_.orientation.w = 1.0;
    has_target_ = true;
    
    RCLCPP_INFO(get_logger(), "Planning Simulator initialized");
    RCLCPP_INFO(get_logger(), "Default target set to (5.0, 0.0). Publish to /goal_pose to change target.");
}

void PlanningSimulator::planning_callback() {
    if (!has_pose_ || !has_target_) {
        return;
    }
    
    // Generate simple path
    generate_simple_path();
    
    // Calculate control command
    calculate_control_command();
    
    // Publish path
    path_pub_->publish(current_path_);
}

void PlanningSimulator::odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    current_pose_ = msg->pose.pose;
    has_pose_ = true;
}

void PlanningSimulator::goal_callback(const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
    target_pose_ = msg->pose;
    has_target_ = true;
    RCLCPP_INFO(get_logger(), "New goal received: (%.2f, %.2f)", 
                target_pose_.position.x, target_pose_.position.y);
}

void PlanningSimulator::generate_simple_path() {
    current_path_.header.stamp = now();
    current_path_.header.frame_id = "odom";
    current_path_.poses.clear();
    
    // Simple straight line path from current to target
    const int num_points = 20;
    
    for (int i = 0; i <= num_points; ++i) {
        geometry_msgs::msg::PoseStamped pose;
        pose.header.stamp = now();
        pose.header.frame_id = "odom";
        
        double t = static_cast<double>(i) / num_points;
        pose.pose.position.x = current_pose_.position.x + 
            t * (target_pose_.position.x - current_pose_.position.x);
        pose.pose.position.y = current_pose_.position.y + 
            t * (target_pose_.position.y - current_pose_.position.y);
        pose.pose.position.z = 0.0;
        
        // Simple orientation interpolation
        pose.pose.orientation = current_pose_.orientation;
        
        current_path_.poses.push_back(pose);
    }
}

void PlanningSimulator::calculate_control_command() {
    // Calculate distance and angle to target
    double dx = target_pose_.position.x - current_pose_.position.x;
    double dy = target_pose_.position.y - current_pose_.position.y;
    
    double distance = std::sqrt(dx * dx + dy * dy);
    double target_yaw = std::atan2(dy, dx);
    
    // Current yaw from quaternion (simplified)
    double current_yaw = std::atan2(
        2.0 * (current_pose_.orientation.w * current_pose_.orientation.z + 
               current_pose_.orientation.x * current_pose_.orientation.y),
        1.0 - 2.0 * (current_pose_.orientation.y * current_pose_.orientation.y + 
                      current_pose_.orientation.z * current_pose_.orientation.z));
    
    double yaw_error = target_yaw - current_yaw;
    
    // Normalize yaw error
    while (yaw_error > M_PI) yaw_error -= 2.0 * M_PI;
    while (yaw_error < -M_PI) yaw_error += 2.0 * M_PI;
    
    // Simple proportional controller
    auto cmd_msg = geometry_msgs::msg::Twist();
    
    if (distance > goal_tolerance_) {
        // Linear velocity proportional to distance
        cmd_msg.linear.x = std::min(max_linear_speed_, 0.5 * distance);
        
        // Angular velocity proportional to yaw error
        cmd_msg.angular.z = std::clamp(2.0 * yaw_error, -max_angular_speed_, max_angular_speed_);
    } else {
        cmd_msg.linear.x = 0.0;
        cmd_msg.angular.z = 0.0;
        RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 2000, "Goal reached!");
    }
    
    cmd_vel_pub_->publish(cmd_msg);
}

}  // namespace autoware_sim_demo