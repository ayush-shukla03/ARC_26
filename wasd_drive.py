import rclpy
from geometry_msgs.msg import Twist
import sys, select, termios, tty

def get_key(settings):
    # Wait for 0.1 seconds to see if a key is being held
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
    else:
        key = ''
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key

def main():
    rclpy.init()
    node = rclpy.create_node('wasd_teleop')
    pub = node.create_publisher(Twist, 'cmd_vel', 10)
    settings = termios.tcgetattr(sys.stdin)

    print("=============================")
    print("🚗 KRATOS WASD CONTROLLER 🚗")
    print("=============================")
    print("Hold W / S to move straight")
    print("Hold A / D to curve at a set angle")
    print("Release all keys to stop")
    print("Press CTRL+C to quit")

    try:
        while rclpy.ok():
            key = get_key(settings)
            msg = Twist()

            if key == 'w':
                msg.linear.x = 0.5
            elif key == 's':
                msg.linear.x = -0.5
            elif key == 'a':
                # Move forward + Turn Left (Sets a fixed turning radius)
                msg.linear.x = 0.5
                msg.angular.z = 0.8
            elif key == 'd':
                # Move forward + Turn Right
                msg.linear.x = 0.5
                msg.angular.z = -0.8
            elif key == '\x03': # CTRL+C
                break
            else:
                # DEADMAN SWITCH: No key detected? Send all zeros!
                msg.linear.x = 0.0
                msg.angular.z = 0.0

            pub.publish(msg)
    except Exception as e:
        print(e)
    finally:
        # Force stop when exiting
        pub.publish(Twist())
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()