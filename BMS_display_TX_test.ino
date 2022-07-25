#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
int counter = 0;
int smokeVal = 0;      // 0-1035 smoke values

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(4, 2, 3);      //CS, RST, INT


  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

void smokeAlarm() {
  int analogPin = A0;     //MQ-135 smoke sensor
  int val = 0;      // 0-1035 smoke values
  smokeVal = analogRead(analogPin);  // read the input pin

  LoRa.beginPacket();
  LoRa.print(" smokeVal ");
  LoRa.print(smokeVal);
  LoRa.endPacket();
  delay(3000);
}

void loop() {
  float sendValue1 = 110.00;
  float sendValue2 = 3.00;
  float sendValue3 = 0.01;

  // lcd.setCursor (0, 0);
  // lcd.print ("Sending...");

  LoRa.beginPacket();
  LoRa.print("   Pack   ");
  LoRa.print(sendValue1);
  LoRa.endPacket();
  delay(3000);

  LoRa.beginPacket();
  LoRa.print("   Mean  ");
  LoRa.print(sendValue2);
  LoRa.endPacket();
  delay(3000);

  LoRa.beginPacket();
  LoRa.print("  Std Dev  ");
  LoRa.endPacket();
  Serial.println("sent 3");
  delay(3000);

  smokeAlarm();
}
