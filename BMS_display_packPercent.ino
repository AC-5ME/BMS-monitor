#include <hd44780.h>
#include <LoRa.h>
#include <Wire.h>
#include <SPI.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

int smokePin = A0;     //MQ-135 smoke sensor
int alarmPin = 8;
int testPin = 5;

char Alerts [] = "No Messages";     //array for BMS alert msg

void Print_Voltage(long& packVolts, int& packPercent) {      //Reset screen and print pack voltage
  const int packMin = 84.0;    //Pack voltage % conversion (3V to 4V1 per cell)
  const int packMax = 114.8;
  const int span = packMax - packMin;

  if (Serial.find(": ")) {
    packVolts =  Serial.parseInt(SKIP_NONE, '.');

    packPercent = (((packVolts / 100.0) - packMin) / span) * 100;

    lcd.clear();
    lcd.print ("Pack: ");
    lcd.setCursor (0, 1);
    lcd.print ("Mean cell: ");
    lcd.setCursor (0, 2);
    lcd.print ("Cell std dev: ");
    lcd.setCursor (0, 3);
    lcd.print ("Alerts: ---");

    lcd.setCursor (6, 0);
    lcd.print (packVolts / 100.0), ("V ");
    lcd.setCursor (12, 0);
    lcd.print ("V");

    lcd.setCursor (16, 0);
    lcd.print (packPercent);
    lcd.setCursor (18, 0);
    lcd.print ("%");

  }
}

void Print_Mean(long& packMean) {     //Print cell volt mean
  if (Serial.find(": ")) {
    packMean =  Serial.parseInt(SKIP_NONE, '.');

    lcd.setCursor (11, 1);
    lcd.print (packMean / 100.0);
    lcd.setCursor (15, 1);
    lcd.print ("V");
  }
}

void Print_Dev(long& packDev) {      //Print cell volt standard deviation
  if (Serial.find(": ")) {
    packDev =  Serial.parseInt(SKIP_NONE, '.');

    lcd.setCursor (14, 2);
    lcd.print (packDev / 1000.0);
    lcd.setCursor (18, 2);
    lcd.print ("V");
  }
}

void Print_Alerts() {     //Print Alerts
  lcd.setCursor (8, 3);

  for (int i = 0; i <= 11; i++) {
    Alerts [i] = Serial.read ();
    delay (10);
  }

  for (int i = 1; i <= 11; i++) {
    lcd.print (Alerts [i]);
  }
}


void Print_Uptime(int& chargeHours, int& chargeMins, int& chargeSecs) {     //Print charge time elapsed
  if (Serial.available()) {

    //if (Serial.find(": ")) {
      chargeHours =  Serial.parseInt();
   // }

   // if (Serial.find(", ")) {
      chargeMins =  Serial.parseInt();
   // }

   // if (Serial.find(", ")) {
      chargeSecs =  Serial.parseInt();
   // }
  }

  delay (2000);

  lcd.clear();
  lcd.print ("      -Uptime-");
  lcd.setCursor (0, 1);
  lcd.print ("Hours: ");
  lcd.setCursor (7, 1);
  lcd.print (chargeHours);
  lcd.setCursor (0, 2);
  lcd.print ("Minutes: ");
  lcd.setCursor (9, 2);
  lcd.print (chargeMins);
  lcd.setCursor (0, 30);
  lcd.print ("Seconds: ");
  lcd.setCursor (9, 3);
  lcd.print (chargeSecs);
}

void PrintTH_1_2(int& TH1, int& TH2) {      //Print temps #1/2
  TH1 = Serial.parseInt();
  TH2 = Serial.parseInt();

  lcd.clear();
  lcd.print ("  -Pack Temps (C)-");
  lcd.setCursor (0, 1);
  lcd.print ("TH 1: ");
  lcd.setCursor (0, 2);
  lcd.print ("TH 2: ");
  lcd.setCursor (6, 1);
  lcd.print (TH1);
  lcd.setCursor (6, 2);
  lcd.print (TH2);
}

void PrintTH_3_4(int& TH3, int& TH4) {      //Print temps #3/4
  TH3 = Serial.parseInt();
  TH4 = Serial.parseInt();

  lcd.setCursor (10, 1);
  lcd.print ("TH 3: ");
  lcd.setCursor (10, 2);
  lcd.print ("TH 4: ");
  lcd.setCursor (17, 1);
  lcd.print (TH3);
  lcd.setCursor (17, 2);
  lcd.print (TH4);
}

void Send_Values(long& packVolts, int& packPercent, long& packMean, long& packDev) {
  LoRa.beginPacket();
  LoRa.print ("P");
  LoRa.print ("Pack: ");
  LoRa.print ((packVolts / 100.0));
  LoRa.print ("V  ");

  LoRa.print (packPercent);
  LoRa.println ("%");

  LoRa.print ("Mean cell: ");
  LoRa.print ((packMean / 100.0));
  LoRa.println ("V");

  LoRa.print ("Cell std dev: ");
  LoRa.print ((packDev / 1000.0));
  LoRa.print ("V");
  LoRa.endPacket();
}

void Send_Alerts () {
  LoRa.beginPacket();
  LoRa.print ("A");
  LoRa.print ("Alerts: ");
  LoRa.print (Alerts);
  LoRa.endPacket();
}

void Send_TH(int& TH1, int& TH2, int& TH3, int& TH4) {
  LoRa.beginPacket();
  LoRa.print ("T");
  LoRa.print ("TH1: ");
  LoRa.print (TH1);
  LoRa.println ("C");
  LoRa.print ("TH2: ");
  LoRa.print (TH2);
  LoRa.println ("C");
  LoRa.print ("TH3: ");
  LoRa.print (TH3);
  LoRa.println ("C");
  LoRa.print ("TH4: ");
  LoRa.print (TH4);
  LoRa.println ("C");
  LoRa.endPacket();
}

void Send_Time(int& chargeHours, int& chargeMins, int& chargeSecs) {
  LoRa.beginPacket();
  LoRa.print ("U");
  LoRa.println ("Uptime:");
  LoRa.print (chargeHours);
  LoRa.println (" H");
  LoRa.print (chargeMins);
  LoRa.println (" M");
  LoRa.print (chargeSecs);
  LoRa.println (" S");
  LoRa.endPacket();
}

void Send_Alarm(long& packVolts, long& packDev, int& TH1, int& TH2, int& TH3, int& TH4) {
  int smokeVal = analogRead(smokePin);     // 0-1035 smoke values

  /*lcd.setCursor (17, 3);      //Debug output
    lcd.print (smokeVal);
    Serial.print ("smokeVal: ");
    Serial.println (smokeVal);
    Serial.print ("pack: ");
    Serial.println ((packVolts / 100.0));
    Serial.print ("TH: ");
    Serial.println (TH1);
    Serial.println (TH2);
    Serial.println (TH3);
    Serial.println (TH4);
    Serial.print ("dev: ");
    Serial.println ((packDev / 1000.0));*/

  if (millis() > 60000) {     //Smoke sensor warm-up delay
    while (smokeVal > 300) {      //300 for light smoke
      LoRa.beginPacket();
      LoRa.print ("!");
      LoRa.endPacket();
      analogWrite(alarmPin, 255);
    }
  }

  while (((packVolts / 100.0) > 115.00)) {      //pack over voltage
    LoRa.beginPacket();
    LoRa.print ("!");
    LoRa.endPacket();
    analogWrite(alarmPin, 255);
  }

  while ((packDev / 1000.0) > .05) {      //Pack cell variance
    LoRa.beginPacket();
    LoRa.print ("!");
    LoRa.endPacket();
    analogWrite(alarmPin, 255);
  }

  while ((TH1 > 55) or (TH2 > 55) or (TH3 > 55) or (TH4 > 55)) {      //60C max cell temp from datasheet
    LoRa.beginPacket();
    LoRa.print ("!");
    LoRa.endPacket();
    analogWrite(alarmPin, 255);
  }
}

void Alarm_Test() {
  int testVal = digitalRead(testPin);     //Alarm test button

  if (testVal == LOW) {
    LoRa.beginPacket();
    LoRa.print ("!");
    LoRa.endPacket();
    lcd.setCursor (0, 3);
    lcd.print ("    -Alarm test-    ");
    analogWrite(alarmPin, 150);
    delay (150);
    analogWrite(alarmPin, 0);
    testVal = HIGH;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);


  pinMode(testPin, INPUT_PULLUP);     //Test button
  pinMode(alarmPin, OUTPUT);    //Alarm!

  LoRa.setPins(4, 2, 3);      //CS, RST, INT
  //LoRa.setTxPower(5);     //not working
  //LoRa.setSyncWord(0xBB);     //not working

  //Serial.setTimeout(2000);      //Timeout for Serial.find (1000ms default)

  lcd.setCursor (0, 0);      // Set the cursor on the X column and Y row
  lcd.print ("Pack: ");
  lcd.setCursor (0, 1);
  lcd.print ("Mean cell: ");
  lcd.setCursor (0, 2);
  lcd.print ("Cell std dev: ");
  lcd.setCursor (0, 3);
  lcd.print ("Alerts: ---");

  if (!LoRa.begin(915E6)) {
    lcd.setCursor (5, 3);
    lcd.print ("LoRa failed!");
  }
}

void loop() {
  long packVolts = 0;     //Sends 0's to remote to indicate failure
  int packPercent = 0;
  long packMean = 0;
  long packDev = 0;
  int TH1 = 0;
  int TH2 = 0;
  int TH3 = 0;
  int TH4 = 0;
  int chargeHours = 0;
  int chargeMins = 0;
  int chargeSecs = 0;
  int smokeVal = 0;

  Alarm_Test();

  Serial.flush();

  Serial.println ("show");

  if (Serial.available()) {
    if (Serial.find("voltage")) {
      Print_Voltage(packVolts, packPercent);
    }
    if (Serial.find("mean   ")) {
      Print_Mean(packMean);
    }
    if (Serial.find("std dev")) {
      Print_Dev(packDev);
    }
    if (Serial.find("alerts   : ")) {
      Print_Alerts();
    }
    if (Serial.find("uptime   ")) {
      Print_Uptime(chargeHours, chargeMins, chargeSecs);
    }
  }

  Alarm_Test();
  Send_Alarm(packVolts, packDev, TH1, TH2, TH3, TH4);

  Send_Values(packVolts, packPercent, packMean, packDev);
  delay (2000);
  Send_Alerts ();
  delay (2000);

  Alarm_Test();
  Send_Alarm(packVolts, packDev, TH1, TH2, TH3, TH4);

  Send_Time(chargeHours, chargeMins, chargeSecs);
  delay (2000);

  Alarm_Test();
  Send_Alarm(packVolts, packDev, TH1, TH2, TH3, TH4);

  Serial.println ("sh th");

  if (Serial.available()) {
    if (Serial.find("2 | "))  {
      PrintTH_1_2(TH1, TH2);
    }
    if (Serial.find("4 | ")) {
      PrintTH_3_4(TH3, TH4);
    }
  }

  Send_TH(TH1, TH2, TH3, TH4);
  delay (2000);

  Alarm_Test();
  Send_Alarm(packVolts, packDev, TH1, TH2, TH3, TH4);
}
