
#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// 32u4, M0, M4, nrf52840, esp32-s2 and 328p
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

enum FSM_STATES {     //Recevied loop states
  FSM_PRINT_PACK,
  FSM_PRINT_MEAN,
  FSM_PRINT_DEV,
  FSM_PRINT_ALERTS,
};

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(5, OUTPUT);      //Alarm buzzer

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
  display.display();

  digitalWrite(RFM95_RST, LOW);     //Manual radio reset
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    display.setCursor(48, 30);
    display.print("Radio fail!");
    display.display();
    while (1);
  }

  display.setCursor(48, 30);
  display.print("Radio enabled");
  display.display();

  if (!rf95.setFrequency(RF95_FREQ)) {
    display.setCursor(72, 30);
    display.print("setFrequency failed!");
    display.display();
    while (1);
  }

  rf95.setTxPower(10, false);     //5 to 23 dBm

}

void loop() {

  char packRequest[6] = "Pack?";
  rf95.send((uint8_t *)packRequest, 6);
  rf95.waitPacketSent();
}
  /*uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.available())
  {
    if (rf95.recv(buf, &len))
    {
      buf[len] = 0;
      display.setCursor(36, 0);
      display.print((char*)buf);
      display.setCursor(75, 0);
      display.print("V");
      display.display();

      tone(5, 200, 1000);
      //Print_Voltage();
    }
  }
}*/
