import rclpy
from rclpy.node import Node
from my_robot_interfaces.srv import AddTwoInts

class AddTwoIntsServer(Node):
    def __init__(self):
        super().__init__("add_two_init_server")
        self.server_ = self.create_service(AddTwoInts, "add_two_ints", self.callback_add_two_ints)
        self.get_logger().info("Add Two Ints server started")

    def callback_add_two_ints(self, request, respone):
        respone.sum = request.a + request.b
        self.get_logger().info(f"{request.a} + {request.b} = {respone.sum}")
        return respone
    
def main(args = None):
    rclpy.init(args=args)
    rclpy.spin(AddTwoIntsServer())
    rclpy.shutdown()

if __name__ =="__main__":
    main()

