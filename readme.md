# ros2 course
Create Your Course Workspace
```
mkdir -p ~/ros2_course_ws/src
cd ~/ros2_course_ws
colcon build
source install/setup.bash
echo "source ~/ros2_course_ws/install/setup.bash" >> ~/.bashrc

```

## Module 1 — ROS 2 Fundamentals

### 1.1 What is ROS2
ROS 2 is a middleware framework for robot software. It provides:

- **Communication** between the processes(nodes) via topic, action, service
- **Hardware abstraction** through a driver ecosystems
- **Tools** for visualization, debugging, simulation
- **Build system** colcon and ament for managing packages


Key differences from ROS 1:
| ROS1 | ROS2|
|------|-----|
| roscore required | no master required |
| TCP/IP custom | DDS |
| Python 2/3 mixed | Python3.10(humble), Python3.12(jazzy) |
| No security | SROS2 security |
| Ubuntu 20.04 max | Ubuntu22.04(humble), Ubuntu24.04(jazzy) |

### 1.2  ROS 2 Graph Concepts

```
┌──────────┐   topic /cmd_vel   ┌──────────────┐
│ Teleop   ├──────────────────► │ Robot Driver │
│ Node     │                    │ Node         │
└──────────┘                    └──────┬───────┘
                                       │ topic /odom
                                       ▼
                                ┌──────────────┐
                                │ Nav Stack    │
                                │ Node         │
                                └──────────────┘
```
- **Node**: a single excutable that perform a logical task
- **Topic**: named bus for streaming data (publisher → subscriber)
- **Service**: synchronous request/response call 
- **Action**: asynchronous goal with feedback and result
- **Parameter**: configurable value stored per-node

### 1.3 Create a python packages

```bash
cd ros2_course_ws/src
ros2 pkg create --build-type ament_python my_py_pkg --dependencies rclpy
```
Package structure:
```
my_py_pkg/
├── my_py_pkg/
│   ├── __init__.py
│   └── my_first_node.py   ← your code goes here
├── package.xml
├── resource/
│   └── my_py_pkg
├── setup.cfg
└── setup.py
```
### 1.4 Your First Python Node 
Create my_py_pkg/my_py_pkg/my_first_node.py:
```python
import rclpy
from rclpy.node import Node


class MyFirstNode(Node):
    def __init__(self):
        super().__init__("my_first_node")
        self.get_logger().info("Hello from my first ROS 2 node!")
        self.counter = 0
        self.create_timer(1.0, self.timer_callback)

    def timer_callback(self):
        self.counter += 1
        self.get_logger().info(f"Timer fired {self.counter} times")


def main(args=None):
    rclpy.init(args=args)
    node = MyFirstNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()
```

Register the entry point in setup.py:
```
entry_points={
    "console_scripts": [
        "my_first_node = my_py_pkg.my_first_node:main",
    ],
},
```
### 1.5 Your First Cpp Node

```bash
cd ~/ros2_course_ws/src
ros2 pkg create --build-type ament_cmake my_cpp_pkg --dependencies rclcpp
```
Create my_cpp_pkg/src/my_first_node.cpp:

```Cpp
#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"

class MyFirstNode : public rclcpp::Node {
public:
    MyFirstNode() : Node("my_first_node"), counter_(0) {
        RCLCPP_INFO(get_logger(), "Hello from my first C++ ROS 2 node!");
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

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MyFirstNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

Update CMakeLists.txt:
```cmake
add_executable(my_first_node src/my_first_node.cpp)
ament_target_dependencies(my_first_node rclcpp)
install(TARGETS my_first_node DESTINATION lib/${PROJECT_NAME})
```
### 1.6 Build and run pkg
```bash
cd ~/ros2_course_ws
colcon build --packages-select my_py_pkg my_cpp_pkg
source install/setup.bash

ros2 run my_py_pkg my_first_node
# In another terminal:
ros2 run my_cpp_pkg my_first_node
```
### Activity 1
> Create a node called `robot_news_station` that publishes a "news ticker" string every 0.5 seconds using a timer. The string should include the node name and a counter. Build and run it, verify the output with `ros2 node list` and `ros2 node info /robot_news_station`.

node
```cpp
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
```
```bash
#terminal 1
ros2 run activity1 robot_new_station
#terminal 2
ros2 node list
/robot_new_station
ros2 node info /robot_new_station
ros2 echo topic /robot_new_station
#result
#data: 'robot_new_station: 274'
#---
```

## Module 2 - Topic & Communications

### 2.1  Publisher/Subscriber Architecture

```
Publisher Node ──[/topic_name: MsgType]──► Subscriber Node
               ──[/topic_name: MsgType]──► Subscriber Node 2
```
- Many publishers → one topic → many subscribers
- Decoupled: publisher doesn't know who subscribes
- Asynchronous: non-blocking

### 2.2 Python Publisher
```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class RobotNewsStation(Node):
    def __init__(self):
        super().__init__("robot_news_station")
        self.pub_ = self.create_publisher(String, "robot_news", 10)
        self.timer = self.create_timer(0.5, self.publish_news)
        self.get_logger().info("Robot News Station started")

    def publish_news(self):
        msg = String()
        msg.data = "Breaking news from" + self.get_name()
        self.pub_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = RobotNewsStation()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ =="__main__":
    main()
```
### 2.3 Python Subscriber
```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class SmartPhone(Node):
    def __init__(self):
        super().__init__("smartphone")
        self.sub = self.create_subscription(String, "robot_news", self.callback_robot_news, 10)

    def callback_robot_news(self, msg: String):
        self.get_logger().info(f"Received: {msg.data}")

def main(args=None):
    rclpy.init(args=args)
    node = SmartPhone()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ =="__main__":
    main()
```
Add both Python nodes to the existing console_scripts list in setup.py, and add <depend>std_msgs</depend> to package.xml:

```
entry_points={
    "console_scripts": [
        "robot_news_station = my_py_pkg.robot_news_station:main",
        "smartphone = my_py_pkg.smartphone:main",
    ],
},
```
### 2.4 C++ Publisher
```
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
```
### 2.5 C++ Subscriber
```
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class SmartphoneNode : public rclcpp::Node {
public:
    SmartphoneNode() : rclcpp::Node("smartphone"){
        sub_ = this -> create_subscription<std_msgs::msg::String>(
            "robot_news", 10,
            std::bind(&SmartphoneNode::callbackRobotNews, this, std::placeholders::_1));
    }
private:
    void callbackRobotNews(const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(get_logger(), "Received: %s", msg->data.c_str());
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    
};
int main(int argc, char **argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SmartphoneNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```
For the C++ package, add `<depend>std_msgs</depend>` to `package.xml`, then add these lines to `CMakeLists.txt`:

```cmake
find_package(std_msgs REQUIRED)

add_executable(robot_news_station src/robot_news_station.cpp)
ament_target_dependencies(robot_news_station rclcpp std_msgs)

add_executable(smartphone src/smartphone.cpp)
ament_target_dependencies(smartphone rclcpp std_msgs)

install(TARGETS
  robot_news_station
  smartphone
  DESTINATION lib/${PROJECT_NAME})
```
### 2.6 Topic CLI commands
```bash
ros2 topic list                          # list all active topics
ros2 topic info /robot_news              # publisher/subscriber count & type
ros2 topic echo /robot_news              # print messages in terminal
ros2 topic hz /robot_news                # measure publish rate
ros2 topic bw /robot_news                # measure bandwidth
ros2 topic pub /robot_news std_msgs/msg/String "data: 'hello'"
```
### 2.7 Common message types

| Package | Message | Use |
|---------|---------|-----|
| `std_msgs` | `String`, `Int32`, `Float64`, `Bool` | Simple data |
| `geometry_msgs` | `Twist`, `Pose`, `Point`, `Quaternion` | Robot motion |
| `sensor_msgs` | `LaserScan`, `Image`, `Imu`, `JointState` | Sensor data |
| `nav_msgs` | `Odometry`, `Path`, `OccupancyGrid` | Navigation |

