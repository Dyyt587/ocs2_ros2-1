from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import ThisLaunchFileDir
from launch_ros.actions import Node


def is_wsl():
    try:
        with open('/proc/version', 'r') as f:
            version_info = f.read().lower()
            return 'microsoft' in version_info or 'wsl' in version_info
    except FileNotFoundError:
        return False


def generate_launch_description():
    prefix = "gnome-terminal --"
    if is_wsl():
        prefix = "xterm -e"
        print("Current system is WSL, use xterm as terminal")
    else:
        print("Current system is not WSL, use gnome-terminal as terminal")

    return LaunchDescription([
        DeclareLaunchArgument(
            name='rviz',
            default_value='true'
        ),
        DeclareLaunchArgument(
            name='task_name',
            default_value='mpc'
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                [ThisLaunchFileDir(), '/visualize.launch.py']),
            launch_arguments={
                'use_joint_state_publisher': 'false'
            }.items()
        ),
        Node(
            package='ocs2_quadrotor_ros',
            executable='quadrotor_mpc',
            name='quadrotor_mpc',
            arguments=[LaunchConfiguration('task_name')],
            output='screen'
        ),
        Node(
            package='ocs2_quadrotor_ros',
            executable='quadrotor_dummy_test',
            name='quadrotor_dummy_test',
            arguments=[LaunchConfiguration('task_name')],
            prefix= prefix,
            output='screen'
        ),
        Node(
            package='ocs2_quadrotor_ros',
            executable='quadrotor_target',
            name='quadrotor_target',
            arguments=[LaunchConfiguration('task_name')],
            prefix= prefix,
            output='screen'
        )
    ])