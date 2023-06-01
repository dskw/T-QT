#include <Arduino.h>
#include <OneButton.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include "driver/adc.h"
#include <esp_bt.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


OneButton btnL = OneButton(0, true, true);

float battery_read()
{
  adc_power_on();
  float voltage = analogRead(4) * 3.3f / 4095.0f * 2.02;
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
    digitalWrite(10, lastState);
}

void setup(void) {
  tft.init();
  tft.setRotation(2);
  powerSafe();

  pinMode(10, OUTPUT);
  btnL.attachClick(handleClick);
}

unsigned long timer = 0;

void loop() {
  btnL.tick();

  if (timer+1000 < millis())
  {
    timer = millis();
    float batVol = battery_read();
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
