#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

float battery_read()
{
  float voltage = analogRead(4) * 3.3f / 4095.0f * 2.02;
  //return voltage;
  return 123.0f - 123.0f / pow(1.0f + pow(voltage / 3.7f, 80), 0.165f);
}

void setup(void) {
  tft.init();
  tft.setRotation(2);
}

void loop() {
  float batVol = battery_read();
    
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextFont(0);
  tft.println("Battery:");
  tft.println(batVol);

  delay(1000);
}
