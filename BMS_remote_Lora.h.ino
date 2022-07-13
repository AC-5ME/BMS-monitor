#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

// 32u4, M0, M4, nrf52840, esp32-s2 and 328p
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define VBATPIN A7

enum FSM_STATES {     //Recevied loop states
  FSM_PRINT_PACK,
  FSM_PRINT_MEAN,
  FSM_PRINT_DEV,
  FSM_PRINT_ALERTS,
};

void setup() {
  pinMode(5, OUTPUT);      //Alarm buzzer
  LoRa.setPins(8, 4, 7);      //CS, RST, INT

  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setTextColor(1, 0);
  display.setCursor(0, 0);
  display.print("Pack: ");
  display.setCursor(0, 10);
  display.print("Mean Cell: ");
  display.setCursor(0, 20);
  display.print("Cell std dev: ");
  display.setCursor(0, 30);
  display.print("Alerts: ---");
  display.setCursor(0, 30);
  display.display();

  if (!LoRa.begin(915E6)) {
    display.setCursor(75, 30);
    display.println("LoRa failed!");
    display.display();
    tone(5, 200, 1000);
    while (1);
  }
}
void loop() {
  
  int packetSize = LoRa.parsePacket();
  if (packetSize) {     // received a packet
    
    Serial.print("Received packet: ");
    display.setCursor(0, 0);
  
    while (LoRa.available()) {
      display.print((char)LoRa.read());
      display.display();
    }
  }
}
