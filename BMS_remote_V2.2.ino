#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

//#define BUTTON_A  9
//#define BUTTON_B  6
//#define BUTTON_C  5
#define VBATPIN A9
//int alarmPin = 5; // Piezo buzzer pin (inop)

void setup() {
  LoRa.setPins(8, 4, 7);      //CS, RST, INT
  //LoRa.setSyncWord(0xBB);     //Not working

  //pinMode(alarmPin, OUTPUT);      //Not working, drains battery

  display.begin(0x3C, true); // Address 0x3C default
  display.clearDisplay();
  display.display();
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setTextColor(1, 0);

  if (!LoRa.begin(915E6)) {
    display.setCursor(0, 0);
    display.print("LoRa failed!");
    display.display();
    while (1);
  }
}

void BatSignal_Display() {
  //int rssi = LoRa.packetRssi();     //Signal strength
  //float snr = LoRa.packetSnr();     //Signal/noise ratio

  const float batMin = 3.0;    //Remote battery % conversion
  const float batMax = 4.2;
  const float span = batMax - batMin;
  float measuredVbat = analogRead(VBATPIN);
  measuredVbat *= 2;    // Adafruit conversions
  measuredVbat *= 3.3;
  measuredVbat /= 1024;
  static float filterVbat;
  const float alpha = 0.08;     //Current smoothing factor - 1.0 does little, lower is smoother

  filterVbat = alpha * (measuredVbat) + (1 - alpha) * filterVbat;
  int batPercent = ((filterVbat - batMin) / (span) * 100);

  /*
    display.setCursor(0, 35);
    display.print("RSSI: ");
    display.print(rssi);
    display.print(" dBm");

    display.setCursor(0, 45);
    display.print("S/N: ");
    display.print(snr);
    display.print(" dBm");
  */

  display.setCursor(0, 55);
  display.print("Remote: ");
  display.print(filterVbat);
  display.print("V ");
  display.print(batPercent);
  display.print("%  ");
  display.display();
}

void Print_Alarm() {
  while (1) {
    display.clearDisplay();
    display.display();
    display.setTextSize(3);
    display.setCursor(15, 20);
    display.print("Alarm!");
    display.display();
    //tone(alarmPin, 4000, 500);      //Pin, Freq, Duration (inop)
    delay(500);
  }
}

void loop() {
  BatSignal_Display();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {     // received a packet?

    if (LoRa.available()) {
      switch ((char)LoRa.read()) {
        case '@': {     //Pack values
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case '#': {     //Alerts
            display.setCursor(0, 25);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case '$': {     //Pack temps
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case '^': {     //Charger uptime
            display.clearDisplay();
            display.setCursor(0, 0);
            display.display();

            while (LoRa.available()) {
              display.print((char)LoRa.read());
              display.display();
            }
          } break;

        case '!': {     //Charger alarm
            Print_Alarm();
          } break;
      }
    }
  }
}
