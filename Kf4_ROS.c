// khepera_ros_bridge_node.cpp

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Range.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include <khepera/khepera.h>
#include <signal.h>
#include <tf/transform_broadcaster.h>
#include <math.h>

#define KH4_WHEELBASE 105.4
#define WHEEL_RADIUS 21.0
#define KH4_PULSE_TO_MM (1000.0 / 370.0)  // Assuming 370 pulses per revolution

// IR sensor indices and threshold
#define FRONT_IR_SENSOR_INDEX 3
#define FRONT_LEFT_IR_SENSOR_INDEX 2
#define FRONT_RIGHT_IR_SENSOR_INDEX 4
#define IR_THRESHOLD 800

// Global Variables
static knet_dev_t *dsPic; // robot pic microcontroller access
static int quitReq = 0;
int lpos, rpos, lpos_prev = 0, rpos_prev = 0;
double vx = 0.0, vy = 0.0, vth = 0.0;
double x = 0.0, y = 0.0, th = 0.0;

// Function prototypes
void velocityCallback(const geometry_msgs::Twist::ConstPtr& cmd_vel);
void publishOdometry(ros::Publisher& odom_pub, tf::TransformBroadcaster& odom_broadcaster);
void publishIRSensors(ros::Publisher& ir_pub);
int areIRSensorsTriggered(int frontIRReading, int frontLeftIRReading, int frontRightIRReading);
void checkIRSensorsAndStop();
void shutdownHandler(int sig);

int main(int argc, char **argv)
{
    // ROS node initialization
    ros::init(argc, argv, "khepera_ros_bridge_node");
    ros::NodeHandle nh;

    // Subscriber for velocity commands
    ros::Subscriber vel_sub = nh.subscribe("cmd_vel", 10, velocityCallback);

    // Publisher for odometry
    ros::Publisher odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);

    // Publisher for IR sensor readings
    ros::Publisher ir_pub = nh.advertise<sensor_msgs::Range>("ir_sensors", 10);

    // TF broadcaster for odometry
    tf::TransformBroadcaster odom_broadcaster;

    // Initialize khepera
    if (kh4_init(0, NULL) != 0) {
        ROS_ERROR("Failed to initialize Khepera library");
        return -1;
    }

    // Open communication with the Khepera's microcontroller
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        ROS_ERROR("Failed to open communication with Khepera4 dsPic");
        return -2;
    }

    // Reset encoders
    kh4_ResetEncoders(dsPic);

    // Set up signal handler for safe shutdown
    signal(SIGINT, shutdownHandler);

    ros::Rate loop_rate(10);  // 10 Hz loop rate

    while (ros::ok() && !quitReq) {
        // Publish odometry and sensor readings
        publishOdometry(odom_pub, odom_broadcaster);
        publishIRSensors(ir_pub);

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}

void velocityCallback(const geometry_msgs::Twist::ConstPtr& cmd_vel)
{
    // Convert linear and angular velocity to wheel speeds
    double linear = cmd_vel->linear.x;
    double angular = cmd_vel->angular.z;

    // Compute wheel velocities
    double vl = (linear - (KH4_WHEELBASE / 2.0) * angular) / KH4_PULSE_TO_MM;
    double vr = (linear + (KH4_WHEELBASE / 2.0) * angular) / KH4_PULSE_TO_MM;

    // Set wheel speeds
    kh4_set_speed((int)vl, (int)vr, dsPic);
}

void publishOdometry(ros::Publisher& odom_pub, tf::TransformBroadcaster& odom_broadcaster)
{
    static struct timeval last_time;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    long dt_us = 1000000LL * (current_time.tv_sec - last_time.tv_sec) + (current_time.tv_usec - last_time.tv_usec);
    double dt = dt_us / 1000000.0;

    if (dt == 0) return;

    last_time = current_time;

    // Get encoder positions
    kh4_get_position(&lpos, &rpos, dsPic);

    // Calculate linear velocities
    double vl = (lpos - lpos_prev) * KH4_PULSE_TO_MM / dt;
    double vr = (rpos - rpos_prev) * KH4_PULSE_TO_MM / dt;

    // Update positions
    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / KH4_WHEELBASE;

    // Compute odometry
    double delta_x = v * cos(th) * dt;
    double delta_y = v * sin(th) * dt;
    double delta_th = omega * dt;

    x += delta_x;
    y += delta_y;
    th += delta_th;

    // Save current encoder positions for next iteration
    lpos_prev = lpos;
    rpos_prev = rpos;

    // Publish the odometry message
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    nav_msgs::Odometry odom;
    odom.header.stamp = ros::Time::now();
    odom.header.frame_id = "odom";

    // Set position
    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    // Set velocity
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = v;
    odom.twist.twist.linear.y = 0.0;
    odom.twist.twist.angular.z = omega;

    // Publish the message
    odom_pub.publish(odom);

    // Broadcast the transform over tf
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = ros::Time::now();
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    // Send the transform
    odom_broadcaster.sendTransform(odom_trans);
}

void publishIRSensors(ros::Publisher& ir_pub)
{
    kh4_proximity_ir(Buffer, dsPic);

    // Extract readings of the specified IR sensors
    int frontIRReading = (Buffer[FRONT_IR_SENSOR_INDEX * 2] | Buffer[FRONT_IR_SENSOR_INDEX * 2 + 1] << 8);
    int frontLeftIRReading = (Buffer[FRONT_LEFT_IR_SENSOR_INDEX * 2] | Buffer[FRONT_LEFT_IR_SENSOR_INDEX * 2 + 1] << 8);
    int frontRightIRReading = (Buffer[FRONT_RIGHT_IR_SENSOR_INDEX * 2] | Buffer[FRONT_RIGHT_IR_SENSOR_INDEX * 2 + 1] << 8);

    sensor_msgs::Range range;
    range.header.stamp = ros::Time::now();
    range.header.frame_id = "base_ir_sensor";
    range.radiation_type = sensor_msgs::Range::INFRARED;
    range.field_of_view = 0.1; // Assume some value
    range.min_range = 0.0;
    range.max_range = 2.0; // Assume some value

    // Publish range for each IR sensor
    range.range = frontIRReading;
    ir_pub.publish(range);

    // Stop if IR sensors detect an obstacle
    if (areIRSensorsTriggered(frontIRReading, frontLeftIRReading, frontRightIRReading)) {
        checkIRSensorsAndStop();
    }
}

int areIRSensorsTriggered(int frontIRReading, int frontLeftIRReading, int frontRightIRReading)
{
    return (frontIRReading > IR_THRESHOLD || frontLeftIRReading > IR_THRESHOLD || frontRightIRReading > IR_THRESHOLD);
}

void checkIRSensorsAndStop()
{
    kh4_SetMode(kh4RegIdle, dsPic);
    kh4_set_speed(0, 0, dsPic);
    ROS_WARN("Obstacle detected! Stopping the robot.");
}

void shutdownHandler(int sig)
{
    quitReq = 1;
    kh4_set_speed(0, 0, dsPic);
    ROS_INFO("Shutting down safely.");
    ros::shutdown();
}
