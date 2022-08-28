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

enum SERIAL_REQUEST {     //Sent msg states
  P,      //Pack voltage
  S,      //Smoke alarm
};

void setup() {
  LoRa.setPins(8, 4, 7);      //CS, RST, INT
  //LoRa.setSyncWord(0xBB);     //Not working

  display.begin(0x3C, true); // Address 0x3C default
  display.clearDisplay();
  display.display();
  display.setRotation(1);
  display.setTextSize(1);
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

void BatSignal_Display() {
  int rssi = LoRa.packetRssi();     //Signal strength
  float snr = LoRa.packetSnr();     //Signal/noise ratio

  const int batMin = 3.2;    //Remote battery % conversion
  const int batMax = 4.2;
  const int span = batMax - batMin;
  float measuredVbat = analogRead(VBATPIN);
  measuredVbat *= 2;    // we divided by 2, so multiply back
  measuredVbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredVbat /= 1024; // convert to voltage
  int batPercent = ((measuredVbat - batMin) / (span) * 100);

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("dBm/snr: ");
  display.print(rssi);
  display.print("/");
  display.print(snr);
  //display.print(" dBm ");
  display.display();

  /* if (batPercent > 100) {
     display.setCursor(0, 48);     //(Row, Column)
     display.print("Remote charging?       ");
     display.display();

    } else { */

  display.setCursor(0, 48);
  display.print("Remote: ");
  display.print(batPercent);
  display.print("%  ");
  display.print(measuredVbat);
  display.print("V ");
  display.display();
}

void Print_Pack() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  Serial.print("got it!");
  int packetSize = LoRa.parsePacket();
  if (packetSize) {     // received a packet?

    while (LoRa.available()) {

      display.print((char)LoRa.read());
      display.display();
    }
  }
}

void Print_Smoke() {
  for (int i = 0; i < 5; i++) {
    display.clearDisplay();
    display.display();
    display.setTextSize(3);
    display.setCursor(15, 10);
    display.print("Smoke!");
    display.display();
    //tone(5, 4000, 500);      //Pin, Freq, Duration
    delay(500);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  display.setTextSize(1);

  BatSignal_Display();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {     // received a packet?

    while (LoRa.available()) {
      switch ((char)LoRa.read()) {
        case 'P': {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case 'M': {
            display.setCursor(0, 10);

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case 'C': {
            display.setCursor(0, 20);

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case 'T': {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case 'U': {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case '!': {
            Print_Smoke();
          } break;
      }
    }
  }
}
