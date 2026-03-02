ARC_26

ARC_26 is a ROS2 + micro-ROS based rover control stack.

It consists of:

A ROS2 control package running on a host PC / Raspberry Pi

A micro-ROS motor driver running on an ESP32

Communication over micro-ROS Agent

This repository is structured to control a 6-wheel differential drive rover.

📁 Repository Structure
ARC_26/
│
├── arduino/
│   └── Drive_ARCv1.ino
│
└── src/
    └── drive_controls/scripts/drive.py
🧠 System Architecture
Joystick → ROS2 Node (drive_controls) → /rover topic
        → micro-ROS Agent
        → ESP32 (Drive_ARCv1.ino)
        → Motor Drivers

ROS2 publishes motor commands on /rover

ESP32 subscribes using micro-ROS

ESP32 sets motor directions and PWM speeds

🖥️ ROS2 Side

Location:

src/drive_controls

This is a ROS2 package responsible for:

Subscribing to joystick input

Computing differential drive wheel speeds

Publishing Int32MultiArray on /rover

🔧 Build Instructions

Inside your ROS2 workspace:

colcon build --packages-select drive_controls
source install/setup.bash
▶️ Run
ros2 run drive_controls drive.py

Make sure:

joy_node is running

/joy topic exists

micro-ROS agent is running

🔌 micro-ROS Agent

Start the agent on your PC:

USB transport
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
Or for WiFi transport
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
🤖 Arduino Side (ESP32)

Location:

arduino/drive.ino

This sketch:

Connects to micro-ROS agent

Subscribes to /rover

Controls 6 motor drivers

Handles reconnection automatically

📦 Required Libraries

Install in Arduino IDE:

micro_ros_arduino

Board:

ESP32
⚙️ Upload Steps

Select correct ESP32 board

Select correct COM port

Upload drive.ino

Start micro-ROS agent on PC

Power ESP32

LED on pin 23 indicates connection state.

📡 Topic Interface
/rover

Type:

std_msgs/msg/Int32MultiArray

Format:

[
  right_front,
  left_front,
  right_middle,
  left_middle,
  right_back,
  left_back
]

Values:

-255 to 255

Positive → forward
Negative → reverse

⚙️ Control Logic

Differential drive

Left wheels share direction

Right wheels share direction

PWM uses absolute value

Direction pins handle polarity

🔄 Reconnection Handling

The ESP32 firmware includes:

Agent ping monitoring

Automatic entity destruction

Automatic re-creation

Safe disconnect handling

If the agent dies, motors stop.

🚀 Future Improvements

Planned enhancements:

Move PID control onto ESP32

Add encoder feedback

Add watchdog timeout

Add velocity-based control instead of raw PWM

Add diagnostics topic

📌 Requirements

ROS2 (Humble recommended)

micro_ros_agent

ESP32

6 motor drivers

Joystick (optional)

🛠️ Troubleshooting

If ESP32 keeps reconnecting:

Ensure QoS is BEST_EFFORT

Ensure agent is running

Check serial port

Verify matching ROS_DOMAIN_ID

📄 License

MIT License (or specify your license here)
