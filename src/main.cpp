#include <rclcpp/rclcpp.hpp>
#include "autoware_sim_demo/vehicle_controller.hpp"
#include "autoware_sim_demo/sensor_simulator.hpp"
#include "autoware_sim_demo/planning_simulator.hpp"
#include <memory>

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    
    // Create nodes
    auto vehicle_controller = std::make_shared<autoware_sim_demo::VehicleController>();
    auto sensor_simulator = std::make_shared<autoware_sim_demo::SensorSimulator>();
    auto planning_simulator = std::make_shared<autoware_sim_demo::PlanningSimulator>();
    
    // Create executor for multi-threaded execution
    rclcpp::executors::MultiThreadedExecutor executor;
    
    // Add nodes to executor
    executor.add_node(vehicle_controller);
    executor.add_node(sensor_simulator);
    executor.add_node(planning_simulator);
    
    RCLCPP_INFO(rclcpp::get_logger("autoware_sim_demo"), "Starting Autoware Simulation Demo");
    
    // Spin
    executor.spin();
    
    rclcpp::shutdown();
    return 0;
}