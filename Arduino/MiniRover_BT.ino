/*
   Gamepad module provides three different mode namely Digital, JoyStick and Accerleometer.

   You can reduce the size of library compiled by enabling only those modules that you want to
   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/game-pad-module/
*/
//neopixel headers and settings
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 38 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1 // Popular NeoPixel ring size
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

enum mode {differential, rotate, ackermann, crabwalk};

mode operation = differential;

//dabble headers
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

int lastTriangleState = 0;
int lastCircleState = 0;
int lastCrossState = 0;
int lastSquareState = 0;

#define Drive_speed 12
#define L_drive_dir 14
#define R_drive_dir 15

#define LF_steer_dir 16
#define RF_steer_dir 17
#define LB_steer_dir 18
#define RB_steer_dir 21

#define LF_steer_speed 47
#define RF_steer_speed 48
#define LB_steer_speed 3
#define RB_steer_speed 35
float y;
float x;
int triangleState;
int circleState;
int crossState;
int squareState;
const uint8_t pins[] = {12,13,47,48,  // row of speed pins
                        14,15,16,17,18,21}; // corresponding dir pins

volatile bool app_is_connected = false;
void setup() {
  // put your setup code here, to run once:
  //neopixel setup
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(115200);      // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");       //set bluetooth name of your device

  for(uint8_t p : pins) {
    pinMode(p,OUTPUT);
  }

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();
  strip.setBrightness(50);
}

void loop() {
  strip.clear();
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.

  if(Dabble.isAppConnected()) {
    app_is_connected = true;
  }

  if(app_is_connected){
    strip.setPixelColor(0, strip.Color(0,255,0));

  }
  else{
    strip.setPixelColor(0, strip.Color(255,0,0));
  }
  
  strip.show();
  x = GamePad.getXaxisData();
  // Serial.print("x_axis: ");
  // Serial.print(x);mode
  // Serial.print('\t');

  y = GamePad.getYaxisData();
  // Serial.print("y_axis: ");
  // Serial.print(y);
  // Serial.print('\t');

  int triangleState = GamePad.isTrianglePressed();
  // Serial.print("Triangle: ");
  // Serial.print(triangleState);
  // Serial.print('\t');

  int squareState = GamePad.isSquarePressed();
  // Serial.print("Square: ");
  // Serial.print(squareState);
  // Serial.print('\t');

  int crossState = GamePad.isCrossPressed();
  // Serial.print("Cross: ");
  // Serial.print(crossState);
  // Serial.print('\t');

  int circleState = GamePad.isCirclePressed();
  // Serial.print("Circle: ");
  // Serial.print(circleState);
  // Serial.print('\t');

  if (triangleState == 1 && operation!=differential) {operation = differential; /*lastTriangleState = triangleState;*/}
  else if (squareState == 1 && operation!=ackermann) {operation = ackermann; /*lastSquareState = squareState;*/}
  else if (crossState == 1 && operation!=crabwalk) {operation = crabwalk; /*lastCrossState = crossState;*/}
  else if (circleState == 1 && operation!=rotate) {operation = rotate; /*lastCircleState = circleState;*/}

  // Serial.print(operation);
  // Serial.println("");

  if(abs(x)<2) x = 0;
  if(abs(y)<2) y = 0;

  switch(operation){
    case differential:
      if(y==0)
      {
        digitalWrite(L_drive_dir, x>0?HIGH:LOW);
        digitalWrite(R_drive_dir, x>0?LOW:HIGH);
        print_debug();
        analogWrite(Drive_speed,(abs(x)/7)*255);
      }
      else
      {
        digitalWrite(L_drive_dir, y>0?HIGH:LOW);
        digitalWrite(R_drive_dir, y>0?HIGH:LOW);
        analogWrite(Drive_speed, (abs(y)/7)*255);
      }

        break;

    case crabwalk:

      if(x==0&&y==0)
      {
        analogWrite(LB_steer_speed,0);
        analogWrite(LF_steer_speed,0);
        analogWrite(RF_steer_speed,0);
        analogWrite(RB_steer_speed,0);
        analogWrite(Drive_speed,0);
        print_debug();
        break;

      }

      if(fabs(x)>fabs(y))
      {
        digitalWrite(LF_steer_dir,x>0?HIGH:LOW);
        digitalWrite(RF_steer_dir,x>0?HIGH:LOW);
        digitalWrite(RB_steer_dir,x>0?HIGH:LOW);
        digitalWrite(LB_steer_dir,x>0?HIGH:LOW);

        analogWrite(LB_steer_speed,75);
        analogWrite(LF_steer_speed,75);
        analogWrite(RF_steer_speed,75);
        analogWrite(RB_steer_speed,75);
        analogWrite(Drive_speed,0);
      }
      else
      {
        analogWrite(LB_steer_speed,0);
        analogWrite(LF_steer_speed,0);
        analogWrite(RF_steer_speed,0);
        analogWrite(RB_steer_speed,0);

        digitalWrite(L_drive_dir,y>0?HIGH:LOW);
        digitalWrite(R_drive_dir,y>0?HIGH:LOW);
        analogWrite(Drive_speed,(fabs(y)/7)*255);
      }
      break;
    default:
      analogWrite(Drive_speed, 0);
      analogWrite(LF_steer_speed,0);
      analogWrite(RF_steer_speed,0);
      break;
     
  }


}


void print_debug()
{
  
  Serial.print("x_axis: ");
  Serial.print(x);
  Serial.print('\t');

  Serial.print("y_axis: ");
  Serial.print(y);
  Serial.print('\t');

  Serial.print("Triangle: ");
  Serial.print(triangleState);
  Serial.print('\t');

  Serial.print("Square: ");
  Serial.print(squareState);
  Serial.print('\t');

  Serial.print("Cross: ");
  Serial.print(crossState);
  Serial.print('\t');

  Serial.print("Circle: ");
  Serial.print(circleState);
  Serial.print('\t');
  Serial.print("millis: ");
  Serial.println(millis());


}