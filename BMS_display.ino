#include <LoRa.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int analogPin = A0;     //MQ-135 smoke sensor

void Print_Voltage(long& packVolts) {      //Reset screen and print pack voltage
  if (Serial.find(": ")) {
    packVolts =  Serial.parseInt(SKIP_NONE, '.');

    lcd.clear();
    lcd.print ("Pack: ");
    lcd.setCursor (0, 1);
    lcd.print ("Mean cell: ");
    lcd.setCursor (0, 2);
    lcd.print ("Cell std dev: ");
    lcd.setCursor (0, 3);
    lcd.print ("Msg: ---");

    lcd.setCursor (6, 0);
    lcd.print ((packVolts / 100.0));
    lcd.setCursor (12, 0);
    lcd.print ("V");
  }
}

void Print_Mean(long& packMean) {     //Print cell volt mean
  if (Serial.find(": ")) {
    packMean =  Serial.parseInt(SKIP_NONE, '.');

    lcd.setCursor (11, 1);
    lcd.print ((packMean / 100.0));
    lcd.setCursor (15, 1);
    lcd.print ("V");
  }
}

void Print_Dev(long& packDev) {      //Print cell volt standard deviation

  if (Serial.find(": ")) {
    packDev =  Serial.parseInt(SKIP_NONE, '.');

    lcd.setCursor (14, 2);
    lcd.print ((packDev / 1000.0));
    lcd.setCursor (18, 2);
    lcd.print ("V");
  }
}

void Print_Alerts() {     //Print Alerts
  lcd.setCursor (5, 3);
  for (int i = 1; i <= 11; i++) {
    char c = Serial.read ();
    lcd.print(c);
  }
}

void Print_Uptime(int& chargeHours, int& chargeMins, int& chargeSecs) {     //Print charge time elapsed
  if (Serial.find(": ")) {
    chargeHours =  Serial.parseInt();
  }
  if (Serial.find(", ")) {
    chargeMins =  Serial.parseInt();
  }
  if (Serial.find(", ")) {
    chargeSecs =  Serial.parseInt();
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
  lcd.clear();
  lcd.print ("  -Pack Temps (C)-");
  lcd.setCursor (0, 1);
  lcd.print ("TH 1: ");
  lcd.setCursor (0, 2);
  lcd.print ("TH 2: ");
  TH1 = Serial.parseInt();
  TH2 = Serial.parseInt();
  lcd.setCursor (6, 1);
  lcd.print (TH1);
  lcd.setCursor (6, 2);
  lcd.print (TH2);
}

void PrintTH_3_4(int& TH3, int& TH4) {      //Print temps #3/4
  lcd.setCursor (10, 1);
  lcd.print ("TH 3: ");
  lcd.setCursor (10, 2);
  lcd.print ("TH 4: ");
  TH3 = Serial.parseInt();
  TH4 = Serial.parseInt();
  lcd.setCursor (17, 1);
  lcd.print (TH3);
  lcd.setCursor (17, 2);
  lcd.print (TH4);
}

void SendValues(long packVolts, long packMean, long packDev) {
  LoRa.beginPacket();
  LoRa.print ("P");
  LoRa.print ("Pack: ");
  LoRa.print ((packVolts / 100.0));
  LoRa.println ("V");

  LoRa.print ("Mean cell: ");
  LoRa.print ((packMean / 100.0));
  LoRa.println ("V");

  LoRa.print ("Cell std dev: ");
  LoRa.print ((packDev / 1000.0));
  LoRa.print ("V");
  LoRa.endPacket();
}

void SendTH(int TH1, int TH2, int TH3, int TH4) {
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

void SendTime(int chargeHours, int chargeMins, int chargeSecs) {
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

void SmokeAlarm() {
  int smokeVal = analogRead(analogPin);     // 0-1035 smoke values

  while (smokeVal > 100) {
    LoRa.beginPacket();
    LoRa.print ("!");
    LoRa.endPacket();
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

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
  lcd.print ("Msg: ---");

  if (!LoRa.begin(915E6)) {
    lcd.setCursor (5, 3);
    lcd.print ("LoRa failed!");
  }
}

void loop() {
  long packVolts = 0;
  long packMean = 0;
  long packDev = 0;
  int TH1 = 0;
  int TH2 = 0;
  int TH3 = 0;
  int TH4 = 0;
  int chargeHours = 0;
  int chargeMins = 0;
  int chargeSecs = 0;

  Serial.println ("show");

  if (Serial.available() > 0) {
    if (Serial.find("voltage"))
      Print_Voltage(packVolts);
    if (Serial.find("mean   "))
      Print_Mean(packMean);
    if (Serial.find("std dev"))
      Print_Dev(packDev);
    if (Serial.find("alerts   : "))
      Print_Alerts();
    if (Serial.find("uptime   "))
      Print_Uptime(chargeHours, chargeMins, chargeSecs);
  }

  SendValues(packVolts, packMean, packDev);
  delay (2000);

  SendTime(chargeHours, chargeMins, chargeSecs);
  delay (2000);

  Serial.println ("sh th");

  if (Serial.available() > 0) {
    if (Serial.find("2 | "))
      PrintTH_1_2(TH1, TH2);
    if (Serial.find("4 | "))
      PrintTH_3_4(TH3, TH4);
  }

  SendTH(TH1, TH2, TH3, TH4);
  delay (2000);

  SmokeAlarm();
  delay (2000);
}
