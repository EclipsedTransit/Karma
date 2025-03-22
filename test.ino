#include <Servo.h>

Servo right;
Servo left;

int ch3 = 1500;  // Initialize channel 3 value to center position
int SCALE_FACTOR = 2;

void setup() {
  Serial.begin(9600);
  right.attach(2);  // attaches the servo on pin 2 to the servo object
  left.attach(3);  // attaches the servo on pin 3 to the servo object

  right.writeMicroseconds(1800);
  left.writeMicroseconds(1800);

  delay(1000); 
  
  right.writeMicroseconds(1000);
  left.writeMicroseconds(1000);
  
  //delay(8000); 
}

void setMotors(int r, int l){
  right.writeMicroseconds(r + 1100);
  left.writeMicroseconds(l + 1100);
}

int map_speed(int input_value) {
  //range is 1200 to 1800
  int output_value = input_value - 500;
  output_value /= 2;
  Serial.print("Channel 3: ");
  Serial.println(input_value);
  Serial.print("MAPPED");
  Serial.println(output_value);
  return output_value;
}

void loop() {
  // Read input from receiver channel 3 (left analog stick)
  ch3 = pulseIn(7, HIGH, 25000); // PulseIn measures pulse duration in microseconds

  // Map ch3 value to motor speed range (1000-2000 microseconds)
  int motorSpeed = map(ch3, 1000, 2000, 0, 1000);

  // Invert motor speed (up on the stick should increase motor speed)
  motorSpeed = map_speed(motorSpeed);

  // Set motor speeds based on ch3 value
  setMotors(motorSpeed, motorSpeed);

  // Debugging output
  //Serial.print("Channel 3: ");
  //Serial.println(ch3);
  Serial.print("Motor Speed: ");
  Serial.println(motorSpeed);

  // Delay for stability
  delay(10);
  setMotors(motorSpeed, motorSpeed);
}
