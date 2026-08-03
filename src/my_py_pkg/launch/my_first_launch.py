from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArguments
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
  robot_name_arg = DeclareLaunchArguments(
    "robot_name",
    default_value = "my_robot",
    description = "Name of the robot")
  
  robot_news_station_node = Node(
    package="my_py_pkg",
    executable="robot_news_station",
    name = "robot_news_station",
    parameters=[{"robot_name": LaunchConfiguration("robot_name")}],
    remappings=[("/robot_news", "/robot_news_v2")],)
  
  smartphone_node = Node(
    package="my_py_pkg",
    executable="smartphone",
    name="smartphone",
    remappings=[("/robot_news", "/robot_news_v2")],)
  
  return LaunchDescription([
    robot_name_arg,
    robot_news_station_node,
    smartphone_node,
  ])

  
