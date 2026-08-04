import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class RobotNewsStation(Node):
    def __init__(self):
        super().__init__("robot_news_station")
        self.declare_parameter("robot_name", "my_robot")
        self.robot_name = self.get_parameter("robot_name").value

        self.pub_ = self.create_publisher(String, "robot_news", 10)
        self.timer = self.create_timer(0.5, self.publish_news)
        self.get_logger().info("Robot News Station started")

    def publish_news(self):
        msg = String()
        msg.data = "Breaking news from" + self.robot_name
        self.pub_.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = RobotNewsStation()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ =="__main__":
    main()

    