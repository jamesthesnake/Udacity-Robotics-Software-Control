#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "nav_msgs/Odometry.h"

ros::Publisher marker_pub;
visualization_msgs::Marker marker;

float pickUpX = -0.5;
float pickUpY = -2.0;

float dropOutX = -4.0;
float dropOutY = 3.0;

void createMarker()
{
    // Set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::CUBE;

    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "/map";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "add_markers";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW,and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/timespecified in the header
    marker.pose.position.x = pickUpX;
    marker.pose.position.y = pickUpY;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 1.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();
}

/**
* Callback function executes when new topic data comes.
* Task of the callback function is to print data to screen.
*/
void robotPosCallback(const nav_msgs::Odometry::ConstPtr &msg)
{
    bool isNotMoving = msg->twist.twist.linear.x < 0.01 && msg->twist.twist.angular.z < 0.01;

    // At Pickup location
    if (isNotMoving && std::abs(msg->pose.pose.position.x - pickUpX) < 0.3 && std::abs(msg->pose.pose.position.y - pickUpY) < 0.3
        && marker.action == visualization_msgs::Marker::ADD)
    {
        marker.action = visualization_msgs::Marker::DELETE;
        marker_pub.publish(marker);
        ROS_INFO("Marker is picked up");
    }

    // At Pickup location
    if (isNotMoving && std::abs(msg->pose.pose.position.x - dropOutX) < 0.3 && std::abs(msg->pose.pose.position.y - dropOutY) < 0.3
        && marker.action == visualization_msgs::Marker::DELETE)
    {
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = dropOutX;
        marker.pose.position.y = dropOutY;
        marker_pub.publish(marker);
        ROS_INFO("Marker is droped out");
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "add_markers");
    ros::NodeHandle n;
    marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
    ros::Subscriber sub = n.subscribe("odom", 1000, robotPosCallback);

    createMarker();

    // Publish the marker
    while (marker_pub.getNumSubscribers() < 1)
    {
        if (!ros::ok())
        {
            return 0;
        }
        //ROS_WARN_ONCE("Please create a subscriber to the marker");
        sleep(1);
    }
    ROS_INFO("Marker is ready to be picked up");
    marker_pub.publish(marker);

    ros::spin();
}
