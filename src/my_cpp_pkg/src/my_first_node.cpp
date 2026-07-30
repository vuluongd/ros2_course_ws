#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"

class MyFirstNode : public rclcpp::Node {
public:
    MyFirstNode():Node("my_first_node"), counter_(0){
        RCLCPP_INFO(get_logger(), "Hello from my first C++ ROS2 node!");
        timer_ = create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&MyFirstNode::timerCallback, this));
    }
private:
    void timerCallback() {
        counter_++;
        RCLCPP_INFO(get_logger(), "Timer fired %d times", counter_);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    int counter_;

};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MyFirstNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}