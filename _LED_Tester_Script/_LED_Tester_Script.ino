#include <FastLED.h>

#define DATA_PIN 7
#define NUM_LEDS 10

CRGB leds[NUM_LEDS];

void setup() {
FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
// Set all LEDs to red
fill_solid(leds, NUM_LEDS, CRGB::Red);
FastLED.show();
delay(1000);

// Set all LEDs to green
fill_solid(leds, NUM_LEDS, CRGB::Green);
FastLED.show();
delay(1000);

// Set all LEDs to blue
fill_solid(leds, NUM_LEDS, CRGB::Blue);
FastLED.show();
delay(1000);
}