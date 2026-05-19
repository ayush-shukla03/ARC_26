import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro

def generate_launch_description():
    # 1. Get the path to your package and xacro file
    pkg_path = os.path.join(get_package_share_directory('rover_description'))
    xacro_file = os.path.join(pkg_path, 'urdf', 'kratos_rover.urdf.xacro')

    # 2. Process the Xacro file into a raw URDF string
    robot_description_config = xacro.process_file(xacro_file)
    robot_description = {'robot_description': robot_description_config.toxml()}

    # 3. Start the Robot State Publisher Node
    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[robot_description]
    )

    # 4. Include the standard Gazebo launch file
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('gazebo_ros'), 'launch', 'gazebo.launch.py')]),
    )

    # 5. Spawn the robot in Gazebo
    spawn_entity = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', 'robot_description',
                   '-entity', 'kratos_rover'],
        output='screen'
    )

    # Launch them all!
    return LaunchDescription([
        node_robot_state_publisher,
        gazebo,
        spawn_entity,
    ])