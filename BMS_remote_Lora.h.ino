#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define VBATPIN A9
#define PIEZO_PIN 5 // Piezo speaker/buzzer pin

void setup() {
  Serial.begin(9600);
  LoRa.setPins(8, 4, 7);      //CS, RST, INT

  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default
  display.clearDisplay();
  display.display();
  display.setRotation(1);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setTextColor(1, 0);
  display.setCursor(0, 0);

  if (!LoRa.begin(915E6)) {
    display.setCursor(0, 0);
    display.print("LoRa failed!");
    display.display();
    tone(5, 4000, 5000);      //Pin, Freq, Duration
    while (1);
  }
}

void loop() {

  const int batMin = 3.2;    //Remote battery % conversion
  const int batMax = 4.3;
  const int span = batMax - batMin;
  float measuredVbat = analogRead(VBATPIN);
  measuredVbat *= 2;    // we divided by 2, so multiply back
  measuredVbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredVbat /= 1024; // convert to voltage
  int batPercent = ((measuredVbat - batMin) / (span) * 100);

  Serial.println(measuredVbat);

  if (batPercent > 100) {
    display.setCursor(0, 48);     //(Row, Column)
    display.print("Charging?");
    display.display();

  } else {
    display.setCursor(0, 48);     //(Row, Column)
    display.print("Batt: ");
    display.print(batPercent);
    display.print("%          ");
    display.display();
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {     // received a packet?

    Serial.println("Received packet: ");
    display.setCursor(0, 0);

    while (LoRa.available()) {
      display.print((char)LoRa.read());
      display.print("   ");
      display.print(" V");
      display.display();
     }
      display.print("      ");
      display.display();
  }
}
