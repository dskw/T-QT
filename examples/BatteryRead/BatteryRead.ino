#include <Arduino.h>
#include <OneButton.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include "driver/adc.h"
#include <esp_bt.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define PIN_BAT_VOLT                    4
#define PIN_LCD_BL                      10
#define PIN_BTN_L                       0
#define PIN_BTN_R                       47

OneButton btnL = OneButton(PIN_BTN_L, true, true);

float battery_read()
{
  adc_power_on();
  float voltage = analogRead(PIN_BAT_VOLT) * 2.0f * 3.3f / 4096.0f;
  adc_power_off();
  return voltage;
}

float battery_percent(float voltage)
{
  return 123.0f - 123.0f / pow(1.0f + pow(voltage / 3.7f, 80), 0.165f);
}

void powerSafe()
{
  adc_power_off();
  btStop(); // disable Bluetooth
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off
  setCpuFrequencyMhz(80);
}

bool lastState = LOW;

static void handleClick() {
    lastState = !lastState;
    digitalWrite(PIN_LCD_BL, lastState);
}

void setup(void) {
  tft.init();
  tft.setRotation(2);
  powerSafe();

  pinMode(PIN_BAT_VOLT, ANALOG);
  analogReadResolution(12);
  
  pinMode(PIN_LCD_BL, OUTPUT);
  btnL.attachClick(handleClick);
}

unsigned long timer = 0;

void loop() {
  btnL.tick();

  if (timer+1000 < millis())
  {
    timer = millis();
    float batVol = battery_read();

    if (batVol < 3.3f)
    {
      powerSafe();
      pinMode(PIN_LCD_BL, OUTPUT);
      digitalWrite(PIN_LCD_BL, HIGH);
      esp_deep_sleep_start();
      return;
    }
    
    float batPercent = battery_percent(batVol);
      
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.setTextFont(0);
    tft.println("Battery:");
    tft.print(batPercent);
    tft.println(" %");
    tft.print(batVol);
    tft.println(" V");
  }
  delay(10);
}
