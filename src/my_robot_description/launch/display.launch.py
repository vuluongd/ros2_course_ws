import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command
from launch_ros.parameter_descriptions import ParameterValue
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    pkg = get_package_share_directory("my_robot_description")
    robot_file = os.path.join(pkg, "urdf", "my_robot.urdf")
    world_file = os.path.join(pkg, "world", "my_world.sdf")

    
    with open(robot_file, "r") as f:
        robot_description = f.read()

    # Start Gazebo Harmonic
    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory("ros_gz_sim"),
                         "launch", "gz_sim.launch.py")),
        launch_arguments={"gz_args": f"-r {world_file}"}.items(),
    )

    # Robot State Publisher
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description,
                     "use_sim_time": True}],
    )

    # Spawn robot in Gazebo
    spawn_robot = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=["-world", "my_world", "-file", robot_file,"-name", "my_robot","-z", "0.2"],
        output="screen",
    )

    # Bridge Gz ↔ ROS 2 topics
    bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist",
            "/odom@nav_msgs/msg/Odometry[gz.msgs.Odometry",
            "/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan",
            "/joint_states@sensor_msgs/msg/JointState[gz.msgs.Model",
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
        ],
        parameters=[{"use_sim_time": True}],
    )

    return LaunchDescription([
        gz_sim,
        robot_state_publisher,
        spawn_robot,
        bridge,
    ])