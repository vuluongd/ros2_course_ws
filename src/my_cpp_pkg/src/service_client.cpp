#include <cinttypes>
#include <functional>
#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsClient : public rclcpp::Node {
public:
    AddTwoIntsClient():Node("add_two_ints_client"){
        client_ = create_client<my_robot_interfaces::srv::AddTwoInts>(
            "add_two_ints");

        while (!client_->wait_for_service(std::chrono::seconds(1))){
             RCLCPP_WARN(this->get_logger(), "Waiting for server...");
        }
        send_request(3, 4);
    }
private:
    void send_request(int64_t a, int64_t b){
        auto request = std::make_shared<my_robot_interfaces::srv::AddTwoInts::Request>();

        request->a = a;
        request->b = b;

        auto future = client_ -> async_send_request(
            request,
            std::bind(
                &AddTwoIntsClient::response_callback,
                this,
                std::placeholders::_1));    
    }
    void response_callback(rclcpp::Client<my_robot_interfaces::srv::AddTwoInts>::SharedFuture future)
    {
        auto response = future.get();
        RCLCPP_INFO(this->get_logger(), "Result = %ld", response->sum);
        rclcpp::shutdown();
    }
    rclcpp::Client<my_robot_interfaces::srv::AddTwoInts>::SharedPtr client_;

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<AddTwoIntsClient>());

    rclcpp::shutdown();
    return 0;
}