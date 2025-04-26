#include <Adafruit_NeoPixel.h>

#define LED_PIN_TOP    9   // Pin for top LED ring
#define LED_PIN_BOTTOM 6   // Pin for bottom LED ring
#define NUM_LEDS_TOP   40  // Number of LEDs on the top ring
#define NUM_LEDS_BOTTOM 40 // Number of LEDs on the bottom ring
#define RC_PIN          2  // Signal pin from controller (e.g., CH1)

Adafruit_NeoPixel ringTop(NUM_LEDS_TOP, LED_PIN_TOP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ringBottom(NUM_LEDS_BOTTOM, LED_PIN_BOTTOM, NEO_GRB + NEO_KHZ800);

enum Direction { LEFT, STRAIGHT, RIGHT };
Direction currentDirection = STRAIGHT;

unsigned long lastBlinkTime = 0;
const int blinkInterval = 300;
bool ledsOn = true;

void setup() {
  pinMode(RC_PIN, INPUT);
  ringTop.begin();
  ringBottom.begin();
  ringTop.show();
  ringBottom.show();
  setDefaultPattern();
}

void loop() {
  int pwmValue = pulseIn(RC_PIN, HIGH, 25000);

  // Determine current direction
  if (pwmValue < 1400) {
    currentDirection = LEFT;
  } else if (pwmValue > 1600) {
    currentDirection = RIGHT;
  } else {
    currentDirection = STRAIGHT;
  }

  // Update lights based on direction
  if (currentDirection == STRAIGHT) {
    setDefaultPattern();
    ledsOn = true; // Reset blinking state
  } else if (currentDirection == LEFT) {
    blinkSection(13, 26, ringTop.Color(255, 0, 0));  // Red section (left) on top ring
    blinkSection(13, 26, ringBottom.Color(255, 0, 0));  // Red section (left) on bottom ring
  } else if (currentDirection == RIGHT) {
    blinkSection(26, 40, ringTop.Color(0, 0, 255));  // Blue section (right) on top ring
    blinkSection(26, 40, ringBottom.Color(0, 0, 255));  // Blue section (right) on bottom ring
  }
}

// Shows default direction pattern: Green Top, Red Left, Blue Right
void setDefaultPattern() {
  for (int i = 0; i < NUM_LEDS_TOP; i++) {
    if (i < 13) {
      ringTop.setPixelColor(i, ringTop.Color(0, 255, 0));  // Top third - Green
      ringBottom.setPixelColor(i, ringBottom.Color(0, 255, 0));  // Bottom third - Green
    } else if (i < 26) {
      ringTop.setPixelColor(i, ringTop.Color(255, 0, 0));  // Left third - Red
      ringBottom.setPixelColor(i, ringBottom.Color(255, 0, 0));  // Bottom third - Red
    } else {
      ringTop.setPixelColor(i, ringTop.Color(0, 0, 255));  // Right third - Blue
      ringBottom.setPixelColor(i, ringBottom.Color(0, 0, 255));  // Bottom third - Blue
    }
  }
  ringTop.show();
  ringBottom.show();
}

// Blink a section between start and end index with a given color
void blinkSection(int startIdx, int endIdx, uint32_t color) {
  if (millis() - lastBlinkTime >= blinkInterval) {
    ledsOn = !ledsOn;
    lastBlinkTime = millis();
  }

  // Always show the full pattern first
  setDefaultPattern();

  if (!ledsOn) {
    // Turn off just the blinking section on both rings
    for (int i = startIdx; i < endIdx; i++) {
      ringTop.setPixelColor(i, 0);
      ringBottom.setPixelColor(i, 0);
    }
    ringTop.show();
    ringBottom.show();
  }
}




