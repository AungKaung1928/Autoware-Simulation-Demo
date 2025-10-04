#pragma once

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <memory>
#include <random>

namespace autoware_sim_demo {

class SensorSimulator : public rclcpp::Node {
public:
    SensorSimulator();
    
private:
    void publish_lidar_data();
    void publish_camera_data();
    void generate_point_cloud();
    void generate_camera_image();
    
    // Publishers
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr lidar_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr camera_pub_;
    
    // Timers
    rclcpp::TimerBase::SharedPtr lidar_timer_;
    rclcpp::TimerBase::SharedPtr camera_timer_;
    
    // Random number generator for simulation
    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dis_;
    
    // Sensor parameters
    double lidar_frequency_{10.0};
    double camera_frequency_{30.0};
    int lidar_points_{1000};
    int camera_width_{640};
    int camera_height_{480};
};

}  // namespace autoware_sim_demo