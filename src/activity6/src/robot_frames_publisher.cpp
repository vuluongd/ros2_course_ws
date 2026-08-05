#include <vector>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/static_transform_broadcaster.h"
#include "tf2/LinearMath/Quaternion.h"

class RobotFramesPublisher : public rclcpp::Node
{
public:
  RobotFramesPublisher():Node("robot_frames_publisher"){
    broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
    std::vector<geometry_msgs::msg::TransformStamped> transforms;
    transforms.push_back(createTransform(
      "base_link",
      "camera_link",
      0.15, 0.0, 0.20,
      0.0, 0.0, 0.0));
    transforms.push_back(createTransform(
      "base_link",
      "lidar_link",
      0.0, 0.0, 0.25,
      0.0, 0.0, 0.0));
    broadcaster_ -> sendTransform(transforms);
    RCLCPP_INFO(get_logger(), "Published static transform. ");
  }
private:
  geometry_msgs::msg::TransformStamped createTransform(
    const std::string &parent,
    const std::string &child,
    double x,
    double y,
    double z,
    double pitch,
    double roll,
    double yaw)

  {
    geometry_msgs::msg::TransformStamped t;
    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = parent;
    t.child_frame_id = child;

    t.transform.translation.x = x;
    t.transform.translation.y = y;
    t.transform.translation.z = z;
    
    tf2::Quaternion q;
    q.setRPY(roll, pitch, yaw);

    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();

    return t;
  }
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
};
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<RobotFramesPublisher>());

    rclcpp::shutdown();
    return 0;
}