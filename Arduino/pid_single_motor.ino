#include <ESP32Encoder.h>

ESP32Encoder encoder;
float lastCount=0.0;
float count=0.0;
float lastReadTime=0.0;
float currentTime=0.0;
float currentRPM=0.0;
float currentPosition=0.0;

int goal=30;             //decent response //slow response
float kp=-0.3;            //2               //1.5
float kd=0.0;            //2               //2
float ki=0.0;            //0.11            //0.085
float motor_pwm=0;
float motor_dir=0;
float error=0; 
float lastError=0; 
float dt;
float differential;
float integral; 

float cpr=80548.0;//full quad cpr.(13PPR, 1:181 gearing, fullquad so x4)


#define PWM 47
#define DIR 16

void setup(){
	Serial.begin(115200);

  pinMode(PWM,OUTPUT);
  pinMode(DIR,OUTPUT);

	//ESP32Encoder::useInternalWeakPullResistors = puType::down;
	// Enable the weak pull up resistors
	ESP32Encoder::useInternalWeakPullResistors = puType::up;

	// use pin 19 and 18 for the first encoder
	encoder.attachFullQuad(4, 5);
		
	// set starting count value after attaching
	// clear the encoder's raw count and set the tracked count to zero
  encoder.clearCount();
  encoder.setCount(0);
	Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));
	// set the lastToggle
}

void loop(){
	// Loop and read the count
	// Serial.println(encoder.getCount());
  // delay(101);
  float currentTime=millis();
  if(currentTime-lastReadTime>=250)
  {
    count=encoder.getCount()/cpr;
    currentRPM=(count)/(currentTime-lastReadTime)*1000*60;
    currentPosition+=count*360;
    // Serial.println(" Encoder rpm = " + String(currentRPM));
    encoder.clearCount();
    PID(goal); 
  }
}


void PID(int goal)
{  

  motor_pwm>=0?motor_dir=0:motor_dir=1;
  dt=currentTime-lastReadTime;
  Serial.print("CurrentSpeed:");
  Serial.print(currentPosition);
  Serial.print(",");
  Serial.print("GOAL:");
  Serial.println(goal);
  // if(fabs(error)<=0.3)
  // {
  //   digitalWrite(DIR,0);
  //   analogWrite(PWM,0);
  //   return;
  // }

  digitalWrite(DIR,motor_dir);
  analogWrite(PWM,abs(motor_pwm));

  error=goal-currentPosition;
  integral+=error;
  differential=(error-lastError)/dt;

 //rpm range is 0-100, pwm values are 0-255
  motor_pwm=constrain(kp*error+kd*differential+ki*integral,-255,255);

  lastError=error;
  lastReadTime=millis();

}
