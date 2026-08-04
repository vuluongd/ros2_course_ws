import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster
import tf_transformations

class DynamicFramePublisher(Node):
  def __init__(self):
    super().__init__("dynamic_frame_publisher")
    self.broadcaster_ = TransformBroadcaster(self)
    self.timer_ = self.create_timer(0.1, self.broadcast_transform)
    self.angle = 0.0

  def broadcast_transform(self):
    t = TransformStamped()
    t.header.stamp = self.get_clock().now().to_msg()
    t.header.frame_id = "odom"
    t.child_frame_id = "base_link"
    t.transform.translation.x = 1.0
    t.transform.translation.y = 0.0
    t.transform.translation.z = 0.0
    self.angle_ += 0.01
    q = tf_transformations.quaternion_from_euler(0, 0, self.angle_)
    t.transform.rotation.x, t.transform.rotation.y = q[0], q[1]
    t.transform.rotation.z, t.transform.rotation.w = q[2], q[3]
    self.broadcaster_.sendTransform(t)

def main(args=None):
    rclpy.init(args=args)
    node = DynamicFramePublisher()
    rclpy.spin(node)
    rclpy.shutdown()
