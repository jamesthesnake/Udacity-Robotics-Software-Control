#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    //Request a service and pass the velocities to it to drive the robot
		ROS_INFO_STREAM("calling the command_robot service to drive the robot in specified direction");
		ball_chaser::DriveToTarget srv;
		srv.request.linear_x = lin_x;
		srv.request.angular_z = ang_z;
		
		// Call the drive_bot service and pass the requested velocities
    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

		int wid=0;
		int R=0,G=R,B=G;
		for (int i=0; i<img.height && !((R+G+B)==765);i++){
				int pos=img.step*i;
				for (int j=0; j<img.step;j+=3){
						R=img.data[pos+j];
						G=img.data[pos+j+1];
					  B=img.data[pos+j+2];
					 if((R+G+B)==765){

						ROS_INFO_STREAM("WHITE PIXEL FOUND!!!");
						wid=j/3;
						break;
								
													}
				 															}
                                                     } 
if((R+G+B)==765)
{
if(wid<(img.width/3))
{
drive_robot(0.0, 0.5);
ROS_INFO_STREAM("go left");
}
else if(wid>(img.width*2/3))
{
drive_robot(0.0, -0.5);
ROS_INFO_STREAM("go right");
}
else {
drive_robot(0.5, 0.0);
ROS_INFO_STREAM("go forward");
}
}
else{
drive_robot(0.0, 0.0);
ROS_INFO_STREAM("NO WHITE PIXEL HERE!!!");
}
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
