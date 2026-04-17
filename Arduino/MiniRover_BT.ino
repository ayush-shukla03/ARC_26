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

//dabble headers
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

const uint8_t pins[] = {4,6,10,12,42,40,  // row of speed pins
                        5,7,11,13,41,39}; // corresponding dir pins
// 0 -> left 1 speed
// 1 -> right 1 speed
// 2 -> left 2 speed
// 3 -> right 2 speed
// 4 -> left 3 speed
// 5 -> right 3 speed
// 0+6 -> left 1 dir
// 1+6 -> right 1 dir
// 2+6 -> left 2 dir
// 3+6 -> right 2 dir
// 4+6 -> left 3 dir
// 5+6 -> right 3 dir
inline uint16_t joy2pwm(float axis) {
  return (uint16_t)(255.0*(abs(axis)))/14.0;
}
inline bool joy2dir(float axis) {
  return axis > 0 ? true : false;
}
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
  float c = GamePad.getXaxisData();
  Serial.print("x_axis: ");
  Serial.print(c);
  Serial.print('\t');
  float d = GamePad.getYaxisData();
  Serial.print("y_axis: ");
  Serial.print(d);
  Serial.print('\t');

  int triangle = GamePad.isTrianglePressed();
  Serial.print("Triangle: ");
  Serial.print(triangle);
  Serial.print('\t');

  int square = GamePad.isSquarePressed();
  Serial.print("Square: ");
  Serial.print(square);
  Serial.print('\t');

  int cross = GamePad.isCrossPressed();
  Serial.print("Cross: ");
  Serial.print(cross);
  Serial.print('\t');

  int circle = GamePad.isCirclePressed();
  Serial.print("Circle: ");
  Serial.print(circle);
  Serial.println("");

}