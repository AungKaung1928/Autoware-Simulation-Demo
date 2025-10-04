#include "autoware_sim_demo/sensor_simulator.hpp"
#include <sensor_msgs/msg/point_field.hpp>

namespace autoware_sim_demo {

SensorSimulator::SensorSimulator() 
    : Node("sensor_simulator"), gen_(rd_()), dis_(-10.0, 10.0) {
    
    // Publishers
    lidar_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/points_raw", 10);
    camera_pub_ = create_publisher<sensor_msgs::msg::Image>("/image_raw", 10);
    
    // Timers
    lidar_timer_ = create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(1000.0 / lidar_frequency_)),
        std::bind(&SensorSimulator::publish_lidar_data, this));
        
    camera_timer_ = create_wall_timer(
        std::chrono::milliseconds(static_cast<int>(1000.0 / camera_frequency_)),
        std::bind(&SensorSimulator::publish_camera_data, this));
    
    RCLCPP_INFO(get_logger(), "Sensor Simulator initialized");
}

void SensorSimulator::publish_lidar_data() {
    auto cloud_msg = sensor_msgs::msg::PointCloud2();
    
    // Header
    cloud_msg.header.stamp = now();
    cloud_msg.header.frame_id = "lidar";
    
    // PointCloud2 fields
    cloud_msg.fields.resize(3);
    
    cloud_msg.fields[0].name = "x";
    cloud_msg.fields[0].offset = 0;
    cloud_msg.fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg.fields[0].count = 1;
    
    cloud_msg.fields[1].name = "y";
    cloud_msg.fields[1].offset = 4;
    cloud_msg.fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg.fields[1].count = 1;
    
    cloud_msg.fields[2].name = "z";
    cloud_msg.fields[2].offset = 8;
    cloud_msg.fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud_msg.fields[2].count = 1;
    
    cloud_msg.is_bigendian = false;
    cloud_msg.point_step = 12;
    cloud_msg.row_step = lidar_points_ * cloud_msg.point_step;
    cloud_msg.width = lidar_points_;
    cloud_msg.height = 1;
    cloud_msg.is_dense = true;
    
    // Generate random point cloud data
    cloud_msg.data.resize(cloud_msg.row_step);
    float* data_ptr = reinterpret_cast<float*>(cloud_msg.data.data());
    
    for (int i = 0; i < lidar_points_; ++i) {
        data_ptr[i * 3 + 0] = static_cast<float>(dis_(gen_));      // x
        data_ptr[i * 3 + 1] = static_cast<float>(dis_(gen_));      // y  
        data_ptr[i * 3 + 2] = static_cast<float>(dis_(gen_) * 0.1); // z
    }
    
    lidar_pub_->publish(cloud_msg);
}

void SensorSimulator::publish_camera_data() {
    auto img_msg = sensor_msgs::msg::Image();
    
    // Header
    img_msg.header.stamp = now();
    img_msg.header.frame_id = "camera";
    
    // Image properties
    img_msg.height = camera_height_;
    img_msg.width = camera_width_;
    img_msg.encoding = "bgr8";
    img_msg.is_bigendian = false;
    img_msg.step = camera_width_ * 3;
    
    // Generate simple gradient image
    img_msg.data.resize(img_msg.step * img_msg.height);
    
    for (int i = 0; i < camera_height_; ++i) {
        for (int j = 0; j < camera_width_; ++j) {
            int idx = i * img_msg.step + j * 3;
            img_msg.data[idx + 0] = static_cast<uint8_t>((i * 255) / camera_height_);     // Blue
            img_msg.data[idx + 1] = static_cast<uint8_t>((j * 255) / camera_width_);      // Green
            img_msg.data[idx + 2] = static_cast<uint8_t>(128);                            // Red
        }
    }
    
    camera_pub_->publish(img_msg);
}

}  // namespace autoware_sim_demo