#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>

void gpsCb(const sensor_msgs::NavSatFixConstPtr &msg){
    ROS_INFO("gps : %.2f, %.2f, %.2f",msg->latitude, msg->altitude, msg->altitude);
} //한번 수신할때 

int main(int argc, char** argv){
    ros::init(argc,argv,"gps_lisenter");
    ros::NodeHandle nh;

    ros::Subscriber gsp_sub;
    gsp_sub = nh.subscribe("/mavros/global_position/global", 10, gpsCb);
    // ros::spin();
    ros::Rate loop_rate(1); 
    while (ros::ok())
    {
        /* code for loop body */

        ros::spinOnce(); //토픽을 계속수신하도록 루프를 돌린다.
        loop_rate.sleep(); //1초대기
    }

    return 0;
}