#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"

class NumberCounter : public rclcpp ::Node {
public:
    NumberCounter():rclcpp::Node("numbercount"), sum_(0){
        sub_ = this -> create_subscription<std_msgs::msg::Int64>("number", 10, std::bind(&NumberCounter::callback, this, std::placeholders::_1));
        pub_ = this->create_publisher<std_msgs::msg::Int64>("number_count", 10);
        }
private:
    void callback(const std_msgs::msg::Int64::SharedPtr msg) {
        sum_ += msg-> data;
        auto count_msg = std_msgs::msg::Int64();
        count_msg.data = sum_;
        pub_ -> publish(count_msg);
        RCLCPP_INFO(get_logger(), "Received: %ld, Sum: %ld", msg->data, sum_);

    }
    rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
    int64_t sum_;
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCounter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}