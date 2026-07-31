#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class RobotNewsStation : public rclcpp::Node {
public:
    RobotNewsStation(): rclcpp::Node("robot_news_station"), counter_(0){
        pub_ = this -> create_publisher<std_msgs::msg::String>("robot_news", 10);
        timer_ = create_wall_timer (
            std::chrono::milliseconds(500),
            std::bind(&RobotNewsStation::timerCallback, this));
    }
private:
    void timerCallback() {
        counter_++;
        auto msg = std_msgs::msg::String();
        msg.data = "New from" + std::string(get_name());
        pub_ -> publish(msg);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    int counter_;
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotNewsStation>();
    rclcpp::spin(node);
    rclcpp::shutdown();

}