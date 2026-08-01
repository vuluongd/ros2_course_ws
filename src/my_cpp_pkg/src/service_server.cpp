#include <cinttypes>
#include <functional>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsServer : public rclcpp::Node {
public:
    AddTwoIntsServer(): Node("add_two_ints_server") {
        server_ = create_service<my_robot_interfaces::srv::AddTwoInts>(
            "add_two_ints",
            std::bind(&AddTwoIntsServer::callbackAddTwoInts, this, std::placeholders::_1, std::placeholders::_2));
    }
private:
    void callbackAddTwoInts(
        const my_robot_interfaces::srv::AddTwoInts::Request::SharedPtr req,
        const my_robot_interfaces::srv::AddTwoInts::Response::SharedPtr res){
        res->sum = req->a +req->b;
        RCLCPP_INFO(get_logger(), "%" PRId64 " + %" PRId64 " =  %" PRId64, req->a, req->b, res->sum);
        }
        rclcpp::Service<my_robot_interfaces::srv::AddTwoInts>::SharedPtr server_;
        
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AddTwoIntsServer>());
    rclcpp::shutdown();
    return 0;
}