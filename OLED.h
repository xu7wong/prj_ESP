#include "eyes_happy.h"
#include "eyes_concentrate.h"
#include "eyes_focus.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306();
#define OLED_I2C_ADDRESS  0x3C
void oled_init(){
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.drawBitmap(0, 0,  eyes_focus , 128, 64, 1);
  //display.setCursor(0, 0);
  //display.println("**\n");
  display.display();
}
