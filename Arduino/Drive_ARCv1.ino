#include <micro_ros_arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32_multi_array.h>


rcl_subscription_t subscriber1;


std_msgs__msg__Int32MultiArray msg;



rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;


#define LED_PIN 13
#define DEADZONE 10

int rw1 = 0;
int rw2 = 0;
int rw3 = 0;
int lw1 = 0;
int lw2 = 0;
int lw3 = 0;


#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

void subscription_callback1(const void * msgin)
{  
  const std_msgs__msg__Int32MultiArray * msg0 = (const std_msgs__msg__Int32MultiArray*)msgin;
  rw1=msg0->data.data[0];
  lw1=msg0->data.data[1];
  rw2=msg0->data.data[2];
  lw2=msg0->data.data[3];
  rw3=msg0->data.data[4];
  lw3=msg0->data.data[5];
  //digitalWrite(LED_PIN, (msg->data.data[0] == 0) ? LOW : HIGH);  
}


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
void setup() {
  set_microros_transports();
  
  pinMode(LED_PIN, OUTPUT);
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


  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  

  msg.data.data = (int32_t*)malloc(6 * sizeof(int32_t)); 
  msg.data.size = 6;
  msg.data.capacity = 6;

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber1,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
    "rover"));


  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber1, &msg, &subscription_callback1, ON_NEW_DATA));

}

void loop() {
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10))); //100Hz

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