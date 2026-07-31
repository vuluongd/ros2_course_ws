#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"

class NumberPublisher : public rclcpp::Node {
public:
    NumberPublisher(): rclcpp::Node("number"), number_(1){
        pub_ = this -> create_publisher<std_msgs::msg::Int64>("/number", 10);
        timer_ = create_wall_timer(
            std::chrono::milliseconds(1000),
            std::bind(&NumberPublisher::timerCallback, this)
        );
    }
private:
    void timerCallback(){
        if (number_ > 100) {
            RCLCPP_INFO(this ->get_logger(), "cancel");
            timer_ ->cancel();
        }
        auto msg = std_msgs::msg::Int64();
        msg.data = number_;
        pub_ -> publish(msg);
        number_ ++;
    }
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int64_t number_;
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
}