#include "autoware_sim_demo/vehicle_controller.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <cmath>

namespace autoware_sim_demo {

VehicleController::VehicleController() 
    : Node("vehicle_controller") {
    
    // Publishers
    odom_pub_ = create_publisher<nav_msgs::msg::Odometry>("/odom", 10);
    
    // Subscribers  
    cmd_vel_sub_ = create_subscription<geometry_msgs::msg::Twist>(
        "/cmd_vel", 10,
        std::bind(&VehicleController::cmd_vel_callback, this, std::placeholders::_1));
    
    // Transform broadcaster
    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    
    // Timer for control loop
    control_timer_ = create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(1000.0 / control_frequency_)),
        std::bind(&VehicleController::control_callback, this));
    
    RCLCPP_INFO(get_logger(), "Vehicle Controller initialized");
}

void VehicleController::control_callback() {
    // Simple bicycle model integration
    double dt = 1.0 / control_frequency_;
    
    // Update velocities with simple first-order dynamics
    double alpha = 0.1;  // Smoothing factor
    vx_ = alpha * target_linear_ + (1.0 - alpha) * vx_;
    vyaw_ = alpha * target_angular_ + (1.0 - alpha) * vyaw_;
    
    // Update position
    x_ += vx_ * cos(yaw_) * dt;
    y_ += vx_ * sin(yaw_) * dt;
    yaw_ += vyaw_ * dt;
    
    // Normalize yaw
    while (yaw_ > M_PI) yaw_ -= 2.0 * M_PI;
    while (yaw_ < -M_PI) yaw_ += 2.0 * M_PI;
    
    // Publish odometry and transform
    publish_odometry();
    publish_transform();
}

void VehicleController::cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
    target_linear_ = msg->linear.x;
    target_angular_ = msg->angular.z;
}

void VehicleController::publish_odometry() {
    auto odom_msg = nav_msgs::msg::Odometry();
    
    // Header
    odom_msg.header.stamp = now();
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "base_link";
    
    // Position
    odom_msg.pose.pose.position.x = x_;
    odom_msg.pose.pose.position.y = y_;
    odom_msg.pose.pose.position.z = 0.0;
    
    // Orientation
    tf2::Quaternion q;
    q.setRPY(0, 0, yaw_);
    odom_msg.pose.pose.orientation.x = q.x();
    odom_msg.pose.pose.orientation.y = q.y();
    odom_msg.pose.pose.orientation.z = q.z();
    odom_msg.pose.pose.orientation.w = q.w();
    
    // Velocity
    odom_msg.twist.twist.linear.x = vx_;
    odom_msg.twist.twist.linear.y = 0.0;
    odom_msg.twist.twist.angular.z = vyaw_;
    
    odom_pub_->publish(odom_msg);
}

void VehicleController::publish_transform() {
    geometry_msgs::msg::TransformStamped t;
    
    t.header.stamp = now();
    t.header.frame_id = "odom";
    t.child_frame_id = "base_link";
    
    t.transform.translation.x = x_;
    t.transform.translation.y = y_;
    t.transform.translation.z = 0.0;
    
    tf2::Quaternion q;
    q.setRPY(0, 0, yaw_);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();
    
    tf_broadcaster_->sendTransform(t);
}

}  // namespace autoware_sim_demo