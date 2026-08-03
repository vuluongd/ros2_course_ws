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

| ROS1 | ROS2 |
| ------ | ----- |
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
>
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

```cpp
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

```cpp
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
| --------- | --------- | ----- |
| `std_msgs` | `String`, `Int32`, `Float64`, `Bool` | Simple data |
| `geometry_msgs` | `Twist`, `Pose`, `Point`, `Quaternion` | Robot motion |
| `sensor_msgs` | `LaserScan`, `Image`, `Imu`, `JointState` | Sensor data |
| `nav_msgs` | `Odometry`, `Path`, `OccupancyGrid` | Navigation |

### 2.8 Custom Message (Interface)

Create a package for interfaces:

```bash
ros2 pkg create my_robot_interfaces --build-type ament_cmake
mkdir -p my_robot_interfaces/msg
```

Create `msg/HardwareStatus.msg`:

```
int64 temperature
bool are_motors_ready
string debug_message
```

Update `CMakeLists.txt`:

```cmake
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/HardwareStatus.msg"
)
```

Add to `package.xml`:

```xml
<buildtool_depend>rosidl_default_generators</buildtool_depend>
<exec_depend>rosidl_default_runtime</exec_depend>
<member_of_group>rosidl_interface_packages</member_of_group>
```

### Activity 2
>
> Build a number publisher that sends integers from 1 to 100 on `/number` at 1 Hz, and a counter subscriber that accumulates the sum and publishes it on `/number_count`. Run both nodes and verify the count is increasing correctly.

## Module 3 -Service & parameters

### 3.1 Topic & Service

| | Topic | Service |
| - | ----- | ------- |
| Pattern | Publish/ Subcribe | Resquet / Respond |
| Direction | One-way | Two - way |
| Timing | Async, Continous | Sync, on - demand |
| Use case | Sensor streams, commands | Configuration, one-off actions |

### 3.2 Custom service definition

Create `srv/AddTwoInts.srv` in your interfaces package:

```bash
mkdir -p my_robot_interfaces/srv
```

```
int64 a
int64 b
---
int64 sum
```

Register it in `CMakeLists.txt`:

```cmake
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/HardwareStatus.msg"
  "srv/AddTwoInts.srv"
)
```

### 3.3 Python Service Server

```python
import rclpy
from rclpy.node import Node
from my_robot_interfaces.srv import AddTwoInts

class AddTwoIntsServer(Node):
    def __init__(self):
        super().__init__("add_two_init_server")
        self.server_ = self.create_service(AddTwoInts, "add_two_ints", self.callback_add_two_ints)
        self.get_logger().info("Add Two Ints server started")

    def callback_add_two_ints(self, request, respone):
        respone.sum = request.a + request.b
        self.get_logger().info(f"{request.a} + {request.b} = {respone.sum}")
        return respone
    
def main(args = None):
    rclpy.init(args=args)
    rclpy.spin(AddTwoIntsServer())
    rclpy.shutdown()

if __name__ =="__main__":
    main()
```

### 3.4 Python Service Client

```python
import rclpy
from rclpy.node import Node
from my_robot_interfaces.srv import AddTwoInts
from functools import partial


class AddTwoIntsClient(Node):
    def __init__(self):
        super().__init__("add_two_ints_client")
        self.call_add_two_ints(3, 4)

    def call_add_two_ints(self, a, b):
        client = self.create_client(AddTwoInts, "add_two_ints")
        while not client.wait_for_service(timeout_sec =1.0):
            self.get_logger().warn("Waiting for the server.....")
        request = AddTwoInts.Request()
        request.a = a
        request.b = b
        future = client.call_async(request)
        future.add_done_callback(partial(self.callback_call, a=a, b=b))

    def callback_call(self, future, a, b):
        response = future.result()
        self.get_logger().info(f"{a}+{b} = {response.sum}")

def main(args = None):
    rclpy.init(args=args)
    rclpy.spin(AddTwoIntsClient())
    rclpy.shutdown()

if __name__ =="__main__":
    main()
```

### 3.5 Cpp Service Server

```cpp
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
```

### 3.6 Cpp service client

```cpp
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
```

### 3.7 Service CLI Commands

```bash
ros2 service list                                    # list all services
ros2 service type /add_two_ints                      # get type
ros2 service call /add_two_ints my_robot_interfaces/srv/AddTwoInts "{a: 3, b: 4}"
```

### 3.8 Parameters

Parameters are named values stored per-node. Use them to make nodes configurable without recompilation

#### Python — declare and use parameters

```python
class MyNode(Node):
    def __init__(self):
        super().__init__("my_node")
        self.declare_parameter("robot_name", "my_robot")
        self.declare_parameter("move_speed", 1.5)
        name = self.get_parameter("robot_name").value
        speed = self.get_parameter("move_speed").value
        self.get_logger().info(f"Robot: {name}, Speed: {speed}")
```

#### C++ - declare and use parameters

```C++
class MyNode(Node):
    def __init__(self):
        super().__init__("my_node")
        self.declare_parameter("robot_name", "my_robot")
        self.declare_parameter("move_speed", 1.5)
        name = self.get_parameter("robot_name").value
        speed = self.get_parameter("move_speed").value
        self.get_logger().info(f"Robot: {name}, Speed: {speed}")
```

#### Parameter CLI commands

```bash
ros2 param list /my_node
ros2 param get /my_node robot_name
ros2 param set /my_node robot_name "robo_007"
ros2 param dump /my_node                    # dump to YAML
ros2 param load /my_node params.yaml       # load from YAML
```

#### Parameter file params.yaml

```yaml
my_node:
  ros__parameters:
    robot_name: "robo_007"
    move_speed: 2.0
```

Load at launch:

```bash
ros2 run my_pkg my_node --ros-args --params-file params.yaml
```

Activity 3
Create a led_panel service server that maintains a list of 3 LEDs (on/off). Accept a `SetLed` service call with `int64 led_number` and 'bool state', returning `bool success` and `string message`. Test it from the CLI.

server C++

```cpp
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
```

client CLI

```bash
ros2 service call /set_led my_robot_interfaces/srv/SetLed "{led_number: 3, state: true}"
```

## Module 4 — Launch Files & Workspaces

### 4.1 Why Launch Files?

A launch file starts multiple nodes with a single command and lets you:

- Configure parameters inline
- Remap topic names
- Set namespaces isolation
- Conditionally include other launch files

### 4.2 XML Launch File

Create `my_py_pkg/launch/my_first_launch.xml`:

```xml
<launch>
    <!-- Start the robot news station -->
    <node pkg="my_py_pkg" exec="robot_news_station" name="robot_news_station">
        <param name="robot_name" value="R2D2" />
    </node>

    <!-- Start the smartphone subscriber -->
    <node pkg="my_py_pkg" exec="smartphone" name="smartphone" />
</launch>
```

### 4.3 Python Launch File

```python
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    robot_name_arg = DeclareLaunchArgument(
        "robot_name",
        default_value="my_robot",
        description="Name of the robot"
    )

    robot_news_station_node = Node(
        package="my_py_pkg",
        executable="robot_news_station",
        name="robot_news_station",
        parameters=[{"robot_name": LaunchConfiguration("robot_name")}],
        remappings=[("/robot_news", "/robot_news_v2")],
    )

    smartphone_node = Node(
        package="my_py_pkg",
        executable="smartphone",
        name="smartphone",
        remappings=[("/robot_news", "/robot_news_v2")],
    )

    return LaunchDescription([
        robot_name_arg,
        robot_news_station_node,
        smartphone_node,
    ])
```

### 4.4 Install Launch Files

```python
import os
from glob import glob

data_files=[
    ("share/ament_index/resource_index/packages",
     ["resource/" + package_name]),
    (os.path.join("share", package_name), ["package.xml"]),
    (os.path.join("share", package_name, "launch"),
     glob("launch/*.py") + glob("launch/*.xml")),
],
```

For CMake packages, add to `CMakeLists.txt`

```cmake
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/)
```

### 4.5 Run a Launch File

```bash
ros2 launch my_py_pkg my_first_launch.py
ros2 launch my_py_pkg my_first_launch.py robot_name:=R2D2

# XML
ros2 launch my_py_pkg my_first_launch.xml
```

### 4.6 Workspace Organization Best Practices

```
ros2_course_ws/
├── src/
│   ├── my_robot_interfaces/   ← all custom msg/srv/action definitions
│   ├── my_py_pkg/             ← Python nodes
│   ├── my_cpp_pkg/            ← C++ nodes
│   └── my_robot_bringup/      ← launch files and configs for the full system
├── install/
├── build/
└── log/
```

Rebuild only changed packages:

```bash
colcon build --packages-select my_py_pkg
colcon build --symlink-install   # Python: edits take effect without rebuild
```

Activity 4

Write a Python launch file that starts three nodes: `robot_news_station`, `smartphone`, and `add_two_ints_server`. Pass `robot_name` as a launch argument with default "CourseBot". Run it and verify all three nodes appear in ```bash ros2 node list```.
