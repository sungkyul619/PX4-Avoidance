#include "ros/ros.h"
#include <mavros_msgs/State.h>
#include <mavros_msgs/CommandBool.h>



void stateCb(const mavros_msgs::State::ConstPtr &msg)
{
    ROS_INFO("connected: %d",msg->connected);    
}


int main(int argc, char** argv){
    ros::init(argc,argv, "heartbeat_node");
    ros::NodeHandle nh;
    //mavros가 발행한 heartbeat신호를 구독한다.
    ros::Subscriber sub;
    sub = nh.subscribe<mavros_msgs::State>("/mavros/state", 10, stateCb);
    //계속구독한다.
    ros::spin();
    return 0;

}






