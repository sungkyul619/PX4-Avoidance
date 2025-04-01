#include "ros/ros.h"

int main(int argc, char** argv){
    ros::init(argc,argv,"base_node");
    ros::NodeHandle nh;

    ROS_INFO("Base node is running");
    ros::spin();
    return 0;
}