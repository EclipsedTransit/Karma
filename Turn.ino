#include <Servo.h>

// RC Receiver signal pins
const int throttlePin = 2;  // Throttle channel signal (e.g., full forward at ~2000µs)
const int turnPin     = 3;  // Turning channel signal (neutral ~1500µs)

// Motor control pins for the left motor
const int leftMotorPWM = 9;
const int leftMotorDir = 8;

// Motor control pins for the right motor
const int rightMotorPWM = 10;
const int rightMotorDir = 7;

// Create Servo objects to read PWM signals from the RC receiver
Servo throttleInput;
Servo turnInput;

void setup() {
    // Initialize motor pins
    pinMode(leftMotorPWM, OUTPUT);
    pinMode(leftMotorDir, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);
    pinMode(rightMotorDir, OUTPUT);
    
    // Set default motor directions (adjust HIGH/LOW if necessary)
    digitalWrite(leftMotorDir, HIGH);
    digitalWrite(rightMotorDir, HIGH);
    
    // Attach RC receiver channels
    throttleInput.attach(throttlePin);
    turnInput.attach(turnPin);
}

void loop() {
    // Read RC signals (typically in microseconds)
    int throttleValue = throttleInput.read(); // Expected range: ~1000 to 2000
    int turnValue     = turnInput.read();     // Expected range: ~1000 to 2000, with 1500 as neutral

    // Map the throttle signal (1000-2000 µs) to a base motor speed (0-255 PWM)
    int baseSpeed = map(throttleValue, 1000, 2000, 0, 255);

    // Map the turn signal to an adjustment value.
    // For example, we map 1000-2000 µs to an adjustment range of -50 to +50.
    int turnAdjustment = map(turnValue, 1000, 2000, -50, 50);

    // Calculate individual motor speeds:
    // - To turn right, reduce the left motor speed and increase the right motor speed.
    // - To turn left, do the opposite.
    int leftSpeed  = baseSpeed - turnAdjustment;
    int rightSpeed = baseSpeed + turnAdjustment;
    
    // Ensure the computed speeds are within the valid PWM range (0-255)
    leftSpeed  = constrain(leftSpeed,  0, 255);
    rightSpeed = constrain(rightSpeed, 0, 255);
    
    // Output the PWM signals to the motors
    analogWrite(leftMotorPWM, leftSpeed);
    analogWrite(rightMotorPWM, rightSpeed);

    // Optional: Add a short delay to stabilize the control loop
    delay(20);
}

