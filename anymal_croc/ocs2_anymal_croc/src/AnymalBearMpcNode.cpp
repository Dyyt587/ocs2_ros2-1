/*
 * AnymalMPC.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: farbod
 */

#include <ros/package.h>

#include <ocs2_comm_interfaces/ocs2_ros_interfaces/mpc/MPC_ROS_Interface.h>

#include "ocs2_anymal_croc/AnymalBearInterface.h"

int main(int argc, char* argv[]) {
  static constexpr size_t STATE_DIM = 24;
  static constexpr size_t INPUT_DIM = 24;
  static constexpr size_t JOINT_DIM = 12;
  const std::string robotName = "anymal";
  using interface_t = anymal::AnymalBearInterface;
  using mpc_ros_t = ocs2::MPC_ROS_Interface<STATE_DIM, INPUT_DIM>;

  // task file
  if (argc <= 1) {
    throw std::runtime_error("No task file specified. Aborting.");
  }
  std::string taskFolder = ros::package::getPath("ocs2_anymal_croc") + "/config/" +
                           std::string(argv[1]);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::cerr << "Loading task file from: " << taskFolder << std::endl;

  // Initialize ros node
  ros::init(argc, argv, robotName + "_mpc");
  ros::NodeHandle n;

  // Set up interface
  anymal::AnymalBearInterface anymalInterface(taskFolder);

  // launch MPC nodes
  auto mpcPtr = anymalInterface.getMpc();
  mpc_ros_t mpcNode(*mpcPtr, robotName);
  mpcNode.launchNodes(n);

  return 0;
}
