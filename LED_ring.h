#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define LED_ring_PIN  15  //D8 1-wire LED_ring
#define NUMPIXELS   12  //How many NeoPixels are attached to the Arduino
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_ring_PIN, NEO_GRB + NEO_KHZ800);

void LED_bar_breath(int delayval, int led_loop, int led_position[], int led1[], int led2[], int led3[], int led4[]) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  int i = 0;
  for (int j = 0; j < led_loop; j++) {
    for (int k = 1; k < 21; k++) {
      pixels.setPixelColor((i + led_position[0]) % NUMPIXELS, pixels.Color((led1[0] > 0) ? exp (k / (24 / log(led1[0]))) : 0, (led1[1] > 0) ? exp (k / (24 / log(led1[1]))) : 0, (led1[2] > 0) ? exp (k / (24 / log(led1[2]))) : 0));
      pixels.setPixelColor((i + led_position[1]) % NUMPIXELS, pixels.Color((led2[0] > 0) ? exp (k / (24 / log(led3[0]))) : 0, (led2[1] > 0) ? exp (k / (24 / log(led2[1]))) : 0, (led2[2] > 0) ? exp (k / (24 / log(led2[2]))) : 0));
      pixels.setPixelColor((i + led_position[2]) % NUMPIXELS, pixels.Color((led2[0] > 0) ? exp (k / (24 / log(led3[0]))) : 0, (led3[1] > 0) ? exp (k / (24 / log(led3[1]))) : 0, (led3[2] > 0) ? exp (k / (24 / log(led3[2]))) : 0));
      pixels.setPixelColor((i + led_position[3]) % NUMPIXELS, pixels.Color((led3[0] > 0) ? exp (k / (24 / log(led4[0]))) : 0, (led4[1] > 0) ? exp (k / (24 / log(led4[1]))) : 0, (led4[2] > 0) ? exp (k / (24 / log(led4[2]))) : 0));
      pixels.show();
      delay(delayval);
    }
    for (int k = 20; k > 0; k--) {
      pixels.setPixelColor((i + led_position[0]) % NUMPIXELS, pixels.Color((led1[0] > 0) ? exp (k / (24 / log(led1[0]))) : 0, (led1[1] > 0) ? exp (k / (24 / log(led1[1]))) : 0, (led1[2] > 0) ? exp (k / (24 / log(led1[2]))) : 0));
      pixels.setPixelColor((i + led_position[1]) % NUMPIXELS, pixels.Color((led2[0] > 0) ? exp (k / (24 / log(led3[0]))) : 0, (led2[1] > 0) ? exp (k / (24 / log(led2[1]))) : 0, (led2[2] > 0) ? exp (k / (24 / log(led2[2]))) : 0));
      pixels.setPixelColor((i + led_position[2]) % NUMPIXELS, pixels.Color((led2[0] > 0) ? exp (k / (24 / log(led3[0]))) : 0, (led3[1] > 0) ? exp (k / (24 / log(led3[1]))) : 0, (led3[2] > 0) ? exp (k / (24 / log(led3[2]))) : 0));
      pixels.setPixelColor((i + led_position[3]) % NUMPIXELS, pixels.Color((led3[0] > 0) ? exp (k / (24 / log(led4[0]))) : 0, (led4[1] > 0) ? exp (k / (24 / log(led4[1]))) : 0, (led4[2] > 0) ? exp (k / (24 / log(led4[2]))) : 0));
      pixels.show();
      delay(delayval);
    }
  }
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void LED_bar_loading(int led_position[], int led1[], int led2[], int led3[], int led4[], int led_time, int led_loop, bool turnoff) {

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();

  for (int i = 0; i < led_loop; i++) {
    pixels.setPixelColor((i + led_position[0] - 1) % NUMPIXELS, pixels.Color(led1[0], led1[1], led1[2])); // Moderately bright green color.
    pixels.setPixelColor((i + led_position[1] - 1) % NUMPIXELS, pixels.Color(led2[0], led2[1], led2[2])); // Moderately bright green color.
    pixels.setPixelColor((i + led_position[2] - 1) % NUMPIXELS, pixels.Color(led3[0], led3[1], led3[2])); // Moderately bright green color.
    pixels.setPixelColor((i + led_position[3] - 1) % NUMPIXELS, pixels.Color(led4[0], led4[1], led4[2])); // Moderately bright green color.

    pixels.show();
    delay(led_time);
    if (turnoff) {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      pixels.show();
    }
  }

}

void LED_wifi_connecting(int i, int brightness,int bmax) {
  int b = map(brightness, 1, bmax, 1, 15);
  int led1[3] = {0, 10 * b, 0};
  int led2[3] = {10 * b, 0, 0};
  int led3[3] = {7 * b, b, 0};
  int led4[3] = {0, 0, 10 * b};

  int led_position[4] = {i, i + 3, i + 6, i + 9};
  LED_bar_loading(led_position, led1, led2, led3, led4, 50, 1, false);

}

void LED_wifi_AP(int i,int brightness){
  
  int led1[3] = {0, 10 * brightness, 0};
  int led2[3] = {10 * brightness, 0, 0};
  int led3[3] = {7 * brightness, brightness, 0};
  int led4[3] = {0, 0, 10 * brightness};
  
  int led_position[4] = {i, i + 3, i + 6, i + 9};
  LED_bar_breath(5, 1, led_position, led1, led2, led3, led4);
  }

  
void led_ring_init() {
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}
