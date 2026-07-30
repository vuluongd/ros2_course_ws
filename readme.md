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
```
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

