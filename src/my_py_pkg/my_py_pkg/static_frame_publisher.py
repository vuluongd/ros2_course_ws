import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped
from tf2_ros import StaticTransformBroadcaster
import tf_transformations

class StaticFramePublisher(Node):
  def __init__(self):
    super().__init__("static_frame_publisher")
    self.broadcaster_ = StaticTransformBroadcaster(self)
    self.send_static_transform()

  def send_static_transform(self):
    t = TransformStamped()
    t.header.stamp = self.get_clock().now().to_msg()
    t.header.frame_id = "base_link"
    t.child_frame_id = "laser_link"
    t.transform.translation.x = 0.2
    t.transform.translation.y = 0.0
    t.transform.translation.z = 0.1
    q = tf_transformations.quaternion_from_euler(0, 0, 0)
    t.transform.rotation.x = q[0]
    t.transform.rotation.y = q[1]
    t.transform.rotation.z = q[2]
    t.transform.rotation.w = q[3]
    self.broadcaster_.sendTransform(t)

def main(args=None):
    rclpy.init(args=args)
    node = StaticFramePublisher()
    rclpy.spin(node)
    rclpy.shutdown()