#include <Servo.h>

const int throttlePin = 2;  // RC receiver signal pin
const int motorPWM = 9;     // PWM pin for motor control
const int motorDir = 8;     // Motor direction pin

Servo throttleInput;

void setup() {
    pinMode(motorPWM, OUTPUT);
    pinMode(motorDir, OUTPUT);
    digitalWrite(motorDir, HIGH);  // Set direction (adjust if needed)

    throttleInput.attach(throttlePin);  // Attach RC throttle channel
}

void loop() {
    int throttleValue = throttleInput.read();  // Read RC throttle signal (1000-2000µs)
    
    // Map throttle input (1000-2000µs) to PWM range (0-255)
    int motorSpeed = map(throttleValue, 1000, 2000, 0, 255);
    
    // Apply speed to motor
    analogWrite(motorPWM, motorSpeed);

// If you want throttle at the highest value
 // if (throttleValue > 1500) {   
  //      analogWrite(motorPWM, 255); 
  //   } else {
    //    analogWrite(motorPWM, 0); 
  //  }
}
