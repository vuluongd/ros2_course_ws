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