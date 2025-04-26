#include <Adafruit_NeoPixel.h>
#include <Wire.h>

// Define LED pins and number of LEDs for top and bottom LED strips
#define LED_PIN_TOP   6   
#define LED_PIN_BOTTOM 7  
#define LED_COUNT      40  

Adafruit_NeoPixel ledTop(LED_COUNT, LED_PIN_TOP, NEO_GRB + NEO_KHZ800);  
Adafruit_NeoPixel ledBottom(LED_COUNT, LED_PIN_BOTTOM, NEO_GRB + NEO_KHZ800);

// Motor control pins (example - you may need to adjust these pins based on your motor controller setup)
#define LEFT_MOTOR_PIN   9
#define RIGHT_MOTOR_PIN  10

#define ADDR_ACCEL 0x18  

// Accelerometer register addresses and configurations
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23

// Accelerometer initialization
void configAccelerometer() {
  Wire.beginTransmission(ADDR_ACCEL);
  Wire.write(CTRL_REG1);
  Wire.write(0x2E); 
  Wire.endTransmission();
  
  Wire.beginTransmission(ADDR_ACCEL);
  Wire.write(CTRL_REG4);
  Wire.write(0x30); 
  Wire.endTransmission();
}

// Reading accelerometer data
int16_t readAccelData(uint8_t regLow, uint8_t regHigh) {
  Wire.beginTransmission(ADDR_ACCEL);
  Wire.write(regLow);
  Wire.endTransmission();
  Wire.requestFrom(ADDR_ACCEL, 2);
  int16_t data = Wire.read();  
  data |= (Wire.read() << 8);  
  return data;
}

// Function to control motors based on robot's orientation (turning)
void controlMotors(int16_t xAccel, int16_t yAccel, int16_t zAccel) {
  int turnSpeed = map(zAccel, -32768, 32767, -255, 255); // Adjust turn speed based on Z-axis (rotation)
  int baseSpeed = 100;  // Base speed for both wheels

  // If robot is turning, adjust the wheel speeds accordingly
  int leftMotorSpeed = baseSpeed - turnSpeed;
  int rightMotorSpeed = baseSpeed + turnSpeed;

  // Ensure motor speed stays within bounds
  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

  // Apply motor control (use PWM or motor driver functions)
  analogWrite(LEFT_MOTOR_PIN, abs(leftMotorSpeed));  // Using absolute values for PWM
  analogWrite(RIGHT_MOTOR_PIN, abs(rightMotorSpeed));  // Using absolute values for PWM

  // Direction control (if motors are bi-directional)
  if (leftMotorSpeed < 0) {
    // Reverse left motor if speed is negative (reverse logic depends on motor driver)
    digitalWrite(LEFT_MOTOR_PIN, LOW);
  } else {
    digitalWrite(LEFT_MOTOR_PIN, HIGH);
  }
  
  if (rightMotorSpeed < 0) {
    // Reverse right motor if speed is negative
    digitalWrite(RIGHT_MOTOR_PIN, LOW);
  } else {
    digitalWrite(RIGHT_MOTOR_PIN, HIGH);
  }
}

// LED update function based on robot's angle
void updateLEDs(int accelAngle, int zAccel) {
  int ledColor = map(accelAngle, 0, 360, 0, 255); 

  bool isFlipped = (zAccel < 0);  // Flip detection based on accelerometer Z-axis value

  if (isFlipped) {
    // Robot is flipped, adjust LED colors accordingly
    for (int i = 0; i < LED_COUNT; i++) {
      ledBottom.setPixelColor(i, ledColor, 0, 255 - ledColor);  // Set flipped LED color
    }
    ledBottom.show();

    for (int i = 0; i < LED_COUNT; i++) {
      ledTop.setPixelColor(i, 255 - ledColor, 0, ledColor);  // Reverse top and bottom LEDs for flipped state
    }
    ledTop.show();
  } else {
    // Robot is upright, normal LED behavior
    for (int i = 0; i < LED_COUNT; i++) {
      ledTop.setPixelColor(i, ledColor, 0, 255 - ledColor);  // Set normal LED color
    }
    ledTop.show();

    for (int i = 0; i < LED_COUNT; i++) {
      ledBottom.setPixelColor(i, ledColor, 0, 255 - ledColor);  // Set bottom LEDs to match
    }
    ledBottom.show();
  }
}

void setup() {
  Serial.begin(115200);
  
  ledTop.begin();
  ledBottom.begin();
  ledTop.show(); 
  ledBottom.show();

  Wire.begin();
  configAccelerometer();

  // Initialize motor control pins (if using PWM motors)
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
}

void loop() {
  int16_t xAccel = readAccelData(OUT_X_L, OUT_X_H);
  int16_t yAccel = readAccelData(OUT_Y_L, OUT_Y_H);
  int16_t zAccel = readAccelData(OUT_Z_L, OUT_Z_H);

  int accelAngle = atan2(yAccel, xAccel) * 180 / PI;  // Calculate the tilt angle of the robot

  // Update LEDs based on accelerometer angle and Z-axis (flip detection)
  updateLEDs(accelAngle, zAccel);

  // Control motor speeds based on accelerometer readings (rotation)
  controlMotors(xAccel, yAccel, zAccel);

  delay(100);  // Delay for a short period to update regularly
}


