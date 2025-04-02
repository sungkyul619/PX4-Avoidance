// #include <ros/ros.h>
// #include <mavros_msgs/CommandTOL.h>
// #include <mavros_msgs/CommandBool.h>
// #include <mavros_msgs/SetMode.h> //mavros_msgs에 자주사용하는 헤더파일,명령 서비드 메세지를 확인해보자.
// #include <geometry_msgs/PoseStamped.h>
// //헤더파일 바로 안들어가지는 이유는? 

// int main(int argc, char** argv){

//     // 서비스클라이언트 생성하여 서비스 요청해야함
//     ros::init(argc,argv,"takeoff_node");
//     ros::NodeHandle nh;

    
//     // setpoint 퍼블리셔 생성
//     ros::Publisher setpoint_pub = nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local", 10);

//     // 서비스클라이언트 생성
//     ros::ServiceClient tol_client = nh.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/takeoff");
//     ros::ServiceClient mode_client = nh.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
//     ros::ServiceClient arm_client = nh.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
   
//     ros::Rate rate(10.0);
//     // FCU와 연결될 때까지 대기
//     while(ros::ok() && setpoint_pub.getNumSubscribers() == 0){
//         ros::spinOnce();
//         rate.sleep();
//         ROS_INFO("connected");
//     }

//     // 목표 위치 (예: 3m 고도) 설정
//     geometry_msgs::PoseStamped pose;
//     pose.pose.position.x = 0;
//     pose.pose.position.y = 0;
//     pose.pose.position.z = 0;

//     // OFFBOARD 모드 전환 전 1초간 setpoint 전송 (10회, 10Hz)
//     for (int i = 0; i < 10; i++) {
//         setpoint_pub.publish(pose);
//         ros::spinOnce();
//         rate.sleep();
//     }


//     mavros_msgs::SetMode mode_cmd;
//     // mode_cmd.request.mode ="offboard" 헤더파일 못 보니까 이렇게 아무렇게 적었지..
//     mode_cmd.request.custom_mode ="OFFBOARD";
//     mode_client.call(mode_cmd);
//     if(mode_cmd.response.mode_sent){
//         ROS_INFO("OFFBOARD mode enable"); 
//     }//mode_sent 헤더봐야 알지..
//     else{
//         ROS_ERROR(" Failed to set OFFBOARD mode");
//         return -1;
//     }
//     ros::Duration(1.0).sleep();

//     mavros_msgs::CommandBool arm_cmd;
//     // mode_cmd.request.mode ="offboard" 헤더파일 못 보니까 이렇게 아무렇게 적었지..
//     // arm_cmd.request.?;
//     arm_cmd.request.value= true;
//     arm_client.call(arm_cmd);
//     if(arm_cmd.response.success) ROS_INFO("arm enable"); //mode_sent 헤더봐야 알지..


//     ros::Duration(5.0).sleep();
    
//     // 서비스 요청 메세지 생성
//     mavros_msgs::CommandTOL cmd;
//     cmd.request.altitude =10;


//     // 서비스 요청 보내기
//     if(tol_client.call(cmd)){
//         ROS_INFO("request success");
//     }else{
//         ROS_INFO("request fail");
//     }
//     return 0;
// }








#include <ros/ros.h>
#include <std_msgs/String.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <geometry_msgs/PoseStamped.h>
mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg) {
    current_state = *msg;
}
int main(int argc, char **argv) {
    ros::init(argc, argv, "px4_takeoff_node");
    ros::NodeHandle nh;
    // FCU 상태 구독
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);
    // 모드 변경 클라이언트
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
    // 암링 서비스 클라이언트
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
    // 목표 위치 퍼블리셔 (OFFBOARD 모드에서 필요)
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10);
    // ROS 주기 설정
    ros::Rate rate(20.0);
    // FCU 연결 대기
    while (ros::ok() && !current_state.connected) {
        ros::spinOnce();
        rate.sleep();
    }
    // 목표 위치 설정 (이륙 고도)
    geometry_msgs::PoseStamped pose;
    pose.pose.position.x = 0;
    pose.pose.position.y = 0;
    pose.pose.position.z = 2.5; // 2.5m 이륙
    // Setpoint 미리 보내기 (OFFBOARD 모드 요구사항)
    for (int i = 100; ros::ok() && i > 0; --i) {
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }
    // OFFBOARD 모드 설정
    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";
    // 암링(Arming) 요청
    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;
    ros::Time last_request = ros::Time::now();
    while (ros::ok()) {
        if (current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))) {
            if (set_mode_client.call(offb_set_mode) && offb_set_mode.response.mode_sent) {
                ROS_INFO("Offboard mode enabled");
            }
            last_request = ros::Time::now();
        } else if (!current_state.armed &&
                   (ros::Time::now() - last_request > ros::Duration(5.0))) {
            if (arming_client.call(arm_cmd) && arm_cmd.response.success) {
                ROS_INFO("Vehicle armed");
            }
            last_request = ros::Time::now();
        }
        // 목표 위치 계속 퍼블리시
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}
