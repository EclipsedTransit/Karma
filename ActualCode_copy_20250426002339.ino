#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// === RC Controller Pin Setup ===
const int throttlePin = 2; // Throttle channel (up/down)
const int turnPin = 3;     // Turn channel (left/right)

// === Brushless Motor (ESC) Control Pins ===
const int leftMotorPin = 9;  // ESC signal for left motor
const int rightMotorPin = 10; // ESC signal for right motor

// === NeoPixel LED Pin Setup ===
#define LED_PIN_TOP 6
#define LED_PIN_BOTTOM 5
#define NUM_LEDS 40

Adafruit_NeoPixel topRing(NUM_LEDS, LED_PIN_TOP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bottomRing(NUM_LEDS, LED_PIN_BOTTOM, NEO_GRB + NEO_KHZ800);

// === RC Input Setup ===
Servo throttleInput;
Servo turnInput;

// === Variables for Controlling Speed and Direction ===
int throttleValue = 0;
int turnValue = 0;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

// === LED Lighting Setup ===
enum Direction { LEFT, STRAIGHT, RIGHT };
Direction currentDirection = STRAIGHT;

// === Flip Detection ===
bool isFlipped = false; // Assume not flipped initially

void setup() {
  Serial.begin(9600);

  // Initialize ESCs and Motors (via Servo signals)
  throttleInput.attach(throttlePin); // Attach throttle control channel
  turnInput.attach(turnPin);         // Attach turning control channel

  // Initialize NeoPixel LED rings
  topRing.begin();
  bottomRing.begin();
  topRing.show();  // Initialize LEDs to OFF
  bottomRing.show();  // Initialize LEDs to OFF

  // Set initial motor speed and LED direction
  setDefaultLEDPattern();
}

void loop() {
  // === Read RC Values ===
  throttleValue = throttleInput.read();  // Throttle value (1000 to 2000)
  turnValue = turnInput.read();          // Turn value (1000 to 2000)

  // === Flip Detection: Check if robot is flipped ===
  if (throttleValue < 1100 || throttleValue > 1900) {
    isFlipped = true;  // If throttle is at extreme (flipped scenario)
  } else {
    isFlipped = false; // Otherwise, assume not flipped
  }

  // === Map Throttle Value to Motor Speed ===
  int baseSpeed = map(throttleValue, 1000, 2000, 0, 255);  // Map throttle to 0-255 range
  int turnAdjustment = map(turnValue, 1000, 2000, -50, 50); // Map turn input for left/right adjustment

  // Calculate motor speeds based on throttle and turning
  leftMotorSpeed = constrain(baseSpeed - turnAdjustment, 0, 255);
  rightMotorSpeed = constrain(baseSpeed + turnAdjustment, 0, 255);

  // === Reverse Motor Direction if Flipped ===
  if (isFlipped) {
    leftMotorSpeed = 255 - leftMotorSpeed;  // Reverse the motor speed
    rightMotorSpeed = 255 - rightMotorSpeed; // Reverse the motor speed
  }

  // === Control Brushless Motors (ESCs) ===
  analogWrite(leftMotorPin, leftMotorSpeed);   // Send PWM signal to left motor ESC
  analogWrite(rightMotorPin, rightMotorSpeed); // Send PWM signal to right motor ESC

  // === LED Direction Control ===
  if (turnValue < 1400) {
    currentDirection = LEFT;  // Turn left
    blinkSection(0, 13, topRing, bottomRing, topRing.Color(255, 0, 0), bottomRing.Color(255, 0, 0)); // Red LED for left turn
  } else if (turnValue > 1600) {
    currentDirection = RIGHT; // Turn right
    blinkSection(13, 26, topRing, bottomRing, topRing.Color(0, 0, 255), bottomRing.Color(0, 0, 255)); // Blue LED for right turn
  } else {
    currentDirection = STRAIGHT; // Move straight
    setDefaultLEDPattern();  // Green LED for straight
  }

  delay(20);  // Short delay for control stability
}

// === Set Default LED Pattern (Stable Light) ===
void setDefaultLEDPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < 13) {
      topRing.setPixelColor(i, topRing.Color(0, 255, 0));  // Green LEDs on top ring (moving straight)
      bottomRing.setPixelColor(i, bottomRing.Color(0, 255, 0));  // Green LEDs on bottom ring (moving straight)
    } else if (i < 26) {
      topRing.setPixelColor(i, topRing.Color(255, 0, 0));  // Red LEDs on top ring (moving left)
      bottomRing.setPixelColor(i, bottomRing.Color(255, 0, 0));  // Red LEDs on bottom ring (moving left)
    } else {
      topRing.setPixelColor(i, topRing.Color(0, 0, 255));  // Blue LEDs on top ring (moving right)
      bottomRing.setPixelColor(i, bottomRing.Color(0, 0, 255));  // Blue LEDs on bottom ring (moving right)
    }
  }
  topRing.show();  // Update top LED ring
  bottomRing.show();  // Update bottom LED ring
}

// === Blink a Section of LEDs for Direction Indication ===
void blinkSection(int startIdx, int endIdx, Adafruit_NeoPixel &topRing, Adafruit_NeoPixel &bottomRing, uint32_t topColor, uint32_t bottomColor) {
  for (int i = startIdx; i < endIdx; i++) {
    topRing.setPixelColor(i, topColor); // Set the LED color for the section
    bottomRing.setPixelColor(i, bottomColor); // Set the LED color for the section
  }
  topRing.show(); // Update the top LED ring
  bottomRing.show(); // Update the bottom LED ring
}

