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
|roscore required|no master required|
|TCP/IP custom|DDS|
|Python 2/3 mixed|Python3.10(humble), Python3.12(jazzy)|
|No security|SROS2 security|
|Ubuntu 20.04 max|Ubuntu22.04(humble), Ubuntu24.04(jazzy)|
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