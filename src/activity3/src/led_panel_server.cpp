#include <memory>
#include <array>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"

using SetLed = my_robot_interfaces::srv::SetLed;

class LedPanelServer : public rclcpp :: Node {
public:
  LedPanelServer():Node("led_panel_server"){
    service_ = create_service<SetLed>(
      "set_led",
      std::bind(&LedPanelServer::callback, this, std::placeholders::_1, std::placeholders::_2));
    RCLCPP_INFO(get_logger(),"LED Panel Server Started");
  }
private:
  void callback(const SetLed::Request::SharedPtr req, SetLed::Response::SharedPtr res){
    if (req->led_number < 1 || req->led_number >3){
      res->success = false;
      res->message = "invalid led number";
      return;
    }
    int index = req->led_number - 1;
    leds_[index] = req->state;
    res->success = true;
    
    res->message = "LED " + std::to_string(req->led_number) + (req->state? " on" : " off");
    printStatus();
  }
  void printStatus(){
    RCLCPP_INFO(get_logger(), "LED1=%s  LED2=%s  LED3=%s", leds_[0] ? "ON" : "OFF", leds_[1] ? "ON" : "OFF",leds_[2] ? "ON" : "OFF");
  }

   std::array<bool,3> leds_;

   rclcpp::Service<SetLed>::SharedPtr service_;
};
int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);

    auto node = std::make_shared<LedPanelServer>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}

