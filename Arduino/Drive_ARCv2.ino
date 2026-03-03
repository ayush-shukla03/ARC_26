/*
 * This is a simple template to use microros with ESP32-Arduino
 * This sketch has sample of publisher to publish from timer_callback
 * and subscrption to listen of new data from other ROS node.
 * 
 * Some of the codes below are gathered from github and forums
 * 
 * Made by Rasheed Kittinanthapanya
 * 
*/


/*
 * TODO : Include your necessary header here
*/


/////////////////////
/// For Micro ROS ///
/////////////////////
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>
/*
 * TODO : include your desired msg header file
*/

#include <std_msgs/msg/int32_multi_array.h>


int rw1 = 0;
int rw2 = 0;
int rw3 = 0;
int lw1 = 0;
int lw2 = 0;
int lw3 = 0;

#define lw1_speed 4
#define rw1_speed 6
#define lw2_speed 10
#define rw2_speed 12
#define lw3_speed 42
#define rw3_speed 40

#define lw1_dir 5
#define rw1_dir 7
#define lw2_dir 11
#define rw2_dir 13
#define lw3_dir 41
#define rw3_dir 39



int32_t array_data[6] = {0, 0, 0, 0, 0, 0};

/*
 * Optional,
 * LED pin to check connection
 * between micro-ros-agent and ESP32
*/
#define LED_PIN 23
#define LED_PIN_TEST 38 // subscription LED
bool led_test_state = false;

/*
 * Helper functions to help reconnect
*/
#define EXECUTE_EVERY_N_MS(MS, X)  do { \
    static volatile int64_t init = -1; \
    if (init == -1) { init = uxr_millis();} \
    if (uxr_millis() - init > MS) { X; init = uxr_millis();} \
  } while (0)

enum states {
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;

/*
 * Declare rcl object
*/
rclc_support_t support;
rcl_init_options_t init_options;
rcl_node_t node;
rclc_executor_t executor;
rcl_allocator_t allocator;

/*
 * TODO : Declare your 
 * publisher & subscription objects below
*/

rcl_subscription_t subscriber;
/*
 * TODO : Define your necessary Msg
 * that you want to work with below.
*/

std_msgs__msg__Int32MultiArray msg;

/*
 * TODO : Define your subscription callbacks here
 * leave the last one as timer_callback()
*/


void subscription_callback(const void * msgin)
{
  const std_msgs__msg__Int32MultiArray * msg0 =
    (const std_msgs__msg__Int32MultiArray *)msgin;

  if (msg0->data.size < 6) {
    // Ignore invalid message
    return;
  }

  rw1 = msg0->data.data[0];
  lw1 = msg0->data.data[1];
  rw2 = msg0->data.data[2];
  lw2 = msg0->data.data[3];
  rw3 = msg0->data.data[4];
  lw3 = msg0->data.data[5];

  digitalWrite(LED_PIN_TEST, !digitalRead(LED_PIN_TEST));
}


/*
   Create object (Initialization)
*/
bool create_entities()
{
  /*
     TODO : Define your
     - ROS node name
     - namespace
     - ROS_DOMAIN_ID
  */
  const char * node_name = "drive_node";
  const char * ns = "";
  const int domain_id = 0;
  
  /*
   * Initialize node
   */
  allocator = rcl_get_default_allocator();
  init_options = rcl_get_zero_initialized_init_options();
  rcl_init_options_init(&init_options, allocator);
  rcl_init_options_set_domain_id(&init_options, domain_id);
  rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
  rclc_node_init_default(&node, node_name, ns, &support);

  
  /*
   * TODO : Init your publisher and subscriber 
   */

  rclc_subscription_init(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
    "/rover", 
    &rmw_qos_profile_sensor_data);


  /*
   * Init Executor
   * TODO : make sure the num_handles is correct
   * num_handles = total_of_subscriber + timer
   * publisher is not counted
   * 
   * TODO : make sure the name of sub msg and callback are correct
   */
  unsigned int num_handles = 1;
  executor = rclc_executor_get_zero_initialized_executor();
  rclc_executor_init(&executor, &support.context, num_handles, &allocator);
  rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA);

  return true;
}
/*
 * Clean up all the created objects
 */
void destroy_entities()
{
  rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

  // rclc_executor_fini(&executor);
  // rcl_init_options_fini(&init_options);
  // rcl_node_fini(&node);
  // rclc_support_fini(&support);

  rcl_subscription_fini(&subscriber, &node);
  rclc_executor_fini(&executor);
  rcl_node_fini(&node);
  rclc_support_fini(&support);
  rcl_init_options_fini(&init_options);
  /*
   * TODO : Make sue the name of publisher and subscriber are correct
   */
  // rcl_subscription_fini(&subscriber, &node);
  
}

void setup() {
  /*
   * TODO : select either of USB or WiFi 
   * comment the one that not use
   */
  set_microros_transports();
  //set_microros_wifi_transports("WIFI-SSID", "WIFI-PW", "HOST_IP", 8888);

  /*
   * Optional, setup output pin for LEDs
   */
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_TEST, OUTPUT);

  pinMode(rw1_dir, OUTPUT);
  pinMode(rw1_speed, OUTPUT);
  pinMode(rw2_dir, OUTPUT);
  pinMode(rw2_speed, OUTPUT);
  pinMode(rw3_dir, OUTPUT);
  pinMode(rw3_speed, OUTPUT);
  pinMode(lw1_dir, OUTPUT);
  pinMode(lw1_speed, OUTPUT);
  pinMode(lw2_dir, OUTPUT);
  pinMode(lw2_speed, OUTPUT);
  pinMode(lw3_dir, OUTPUT);
  pinMode(lw3_speed, OUTPUT);


  /*
   * TODO : Initialze the message data variable
   */
  // int32_t array_data[6] = {0, 0, 0, 0, 0, 0};
  // msg.data.capacity = 6; // number of array length
  // msg.data.size = 6;     // number of array length
  // msg.data.data = array_data;
  std_msgs__msg__Int32MultiArray__init(&msg);
  msg.data.data = (int32_t*) malloc(6 * sizeof(int32_t));
  msg.data.capacity = 6;
  msg.data.size = 0;  // important: start at 0

  /*
   * Setup first state
   */
  state = WAITING_AGENT;

}

void loop() {
  /*
   * Try ping the micro-ros-agent (HOST PC), then switch the state 
   * from the example
   * https://github.com/micro-ROS/micro_ros_arduino/blob/galactic/examples/micro-ros_reconnection_example/micro-ros_reconnection_example.ino
   * 
   */
  switch (state) {
    case WAITING_AGENT:
      EXECUTE_EVERY_N_MS(500, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : WAITING_AGENT;);
      break;
    case AGENT_AVAILABLE:
      state = (true == create_entities()) ? AGENT_CONNECTED : WAITING_AGENT;
      if (state == WAITING_AGENT) {
        destroy_entities();
      };
      break;
    case AGENT_CONNECTED:
      EXECUTE_EVERY_N_MS(200, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);
      if (state == AGENT_CONNECTED) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
      }
      break;
    case AGENT_DISCONNECTED:
      destroy_entities();
      state = WAITING_AGENT;
      break;
    default:
      break;
  }
  /*
   * Output LED when in AGENT_CONNECTED state
   */
  if (state == AGENT_CONNECTED) {
    digitalWrite(LED_PIN, 1);

    if(lw1 > 0 && rw1 > 0){
      digitalWrite(lw1_dir, LOW);
      digitalWrite(lw2_dir, LOW);
      digitalWrite(lw3_dir, LOW);
      digitalWrite(rw1_dir, LOW);
      digitalWrite(rw2_dir, LOW);
      digitalWrite(rw3_dir, LOW);
  }

    
  else if(lw1 < 0 && rw1 < 0){
      digitalWrite(lw1_dir, HIGH);
      digitalWrite(lw2_dir, HIGH);
      digitalWrite(lw3_dir, HIGH);
      digitalWrite(rw1_dir, HIGH);
      digitalWrite(rw2_dir, HIGH);
      digitalWrite(rw3_dir, HIGH);
  }

  else if(lw1 > 0 && rw1 < 0){
      digitalWrite(lw1_dir, HIGH);
      digitalWrite(lw2_dir, HIGH);
      digitalWrite(lw3_dir, HIGH);
      digitalWrite(rw1_dir, LOW);
      digitalWrite(rw2_dir, LOW);
      digitalWrite(rw3_dir, LOW);
  }

  else if(lw1 < 0 && rw1 > 0){
      digitalWrite(lw1_dir, LOW);
      digitalWrite(lw2_dir, LOW);
      digitalWrite(lw3_dir, LOW);
      digitalWrite(rw1_dir, HIGH);
      digitalWrite(rw2_dir, HIGH);
      digitalWrite(rw3_dir, HIGH);
  }

  else if(lw1 == 0 || rw1 == 0){
      analogWrite(lw1_speed, 0);
      analogWrite(lw2_speed, 0);
      analogWrite(lw3_speed, 0);
      analogWrite(rw1_speed, 0);
      analogWrite(rw2_speed, 0);
      analogWrite(rw3_speed, 0);
  }


    analogWrite(lw1_speed, abs(lw1));
    analogWrite(lw2_speed, abs(lw2));
    analogWrite(lw3_speed, abs(lw3));
    analogWrite(rw1_speed, abs(rw1));
    analogWrite(rw2_speed, abs(rw2));
    analogWrite(rw3_speed, abs(rw3));
  } 
  else {
    digitalWrite(LED_PIN, 0);
  }

  /*
   * TODO : 
   * Do anything else you want to do here,
   * like read sensor data,  
   * calculate something, etc.
   */

}
