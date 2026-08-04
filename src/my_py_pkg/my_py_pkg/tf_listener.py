import rclpy 
from rclpy.node import Node
from tf2_ros import Buffer, TransformListener

class TFListenerNode(Node):
  def __init__(self):
    super().__init__("tf_listener")
    self.tf_buffer_ = Buffer()
    self.tf_listener_ = TransformListener(self.tf_buffer_, self)
    self.timer_ = self.create_timer(0.5, self.lookup_transform)
  
  def lookup_transform(self):
    try:
      t = self.tf_buffer_.lookup_transform(
        "base_link", "laser_link", rclpy.time.Time())
      self.get_logger().info(f"laser_link is at x={t.transform.translation.x:.2f} "
                           f"relative to base_link")
    except Exception as e:
      self.get_logger().warn(str(e))

def main(args=None):
    rclpy.init(args=args)
    node = TFListenerNode()
    rclpy.spin(node)
    rclpy.shutdown()



  
