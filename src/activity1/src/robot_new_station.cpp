#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class RobotNewStation : public rclcpp::Node{
public:
    RobotNewStation():Node("robot_new_station"), counter_(0){
        publisher_ = this -> create_publisher<std_msgs::msg::String>("new_ticker", 10);
        timer_ = create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&RobotNewStation::timerCallback, this));
    }
private:
    void timerCallback() {
        counter_++;
        auto message = std_msgs::msg::String();
        message.data = "robot_new_station: " + std::to_string(counter_);
        publisher_ -> publish(message);
        
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    int counter_;
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotNewStation>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}