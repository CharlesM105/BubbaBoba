#include <FastLED.h>

#define LED_PIN     6    // Pin connected to the LED strip data input
#define NUM_LEDS    30   // Number of LEDs in the strip
#define BRIGHTNESS  150  // Brightness (0-255)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];  // Array to hold LED colors

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
    rainbowCycle(10);  // Cycle through rainbow colors
}

// Function to cycle through rainbow colors
void rainbowCycle(int wait) {
    for (int j = 0; j < 255; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV((i * 256 / NUM_LEDS + j) & 255, 255, BRIGHTNESS);
        }
        FastLED.show();
        delay(wait);
    }
}