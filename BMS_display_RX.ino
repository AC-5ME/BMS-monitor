
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <RH_RF95.h>

#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3

int smokeVal = 0;      // 0-1035 smoke values


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  delay(250); // wait for the LED to power up

  lcd.init();
  lcd.backlight();
  lcd.setCursor (0, 0);      // Set the cursor on the X column and Y row
  lcd.print ("Pack: ");
  lcd.setCursor (0, 1);
  lcd.print ("Mean cell: ");
  lcd.setCursor (0, 2);
  lcd.print ("Cell std dev: ");
  lcd.setCursor (0, 3);
  lcd.print ("Alerts: ---");

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    lcd.setCursor (8, 3);
    lcd.print("                       ");
    lcd.setCursor (8, 3);
    lcd.print("LoRa failed!");
    while (1);
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    lcd.setCursor (8, 3);
    lcd.print("                        ");
    lcd.setCursor (8, 3);
    lcd.print("setFrequency fail!");
    while (1);
  }

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(10, false);

  int16_t packetnum = 0;  // packet counter, we increment per xmission

  //Serial.setTimeout(2000);      //Timeout for Serial.find (1000ms default)
}

void Print_Voltage() {      //Reset screen and print pack voltage

  if (Serial.find(": ")) {
    long packVolts =  Serial.parseInt(SKIP_NONE, '.');
    float printVolts = (packVolts / 100.0);

    lcd.clear();
    lcd.print ("Pack: ");
    lcd.setCursor (0, 1);
    lcd.print ("Mean cell: ");
    lcd.setCursor (0, 2);
    lcd.print ("Cell std dev: ");
    lcd.setCursor (0, 3);
    lcd.print ("BMS: ---");

    lcd.setCursor (6, 0);
    lcd.print (printVolts);
    lcd.setCursor (13, 0);
    lcd.print ("V");
  }
}

void Send_Pack(float printVolts) {

  char voltPacket[7] = "------";
  dtostrf(printVolts, 5, 2, voltPacket);
  rf95.send((uint8_t *)voltPacket, 7);
  rf95.waitPacketSent();
}

void Print_Mean() {     //Print cell volt mean

  if (Serial.find(": ")) {
    long packMean =  Serial.parseInt(SKIP_NONE, '.');
    float printMean = (packMean / 100.0);

    lcd.setCursor (11, 1);
    lcd.print (printMean);
    lcd.setCursor (16, 1);
    lcd.print ("V");

    char meanPacket[5] = "-----";
    dtostrf(printMean, 4, 2, meanPacket);
    //rf95.send((uint8_t *)meanPacket, 5);
    //rf95.waitPacketSent();
  }
}

void Print_Dev() {      //Print cell volt standard deviation

  if (Serial.find(": ")) {
    long packDev =  Serial.parseInt(SKIP_NONE, '.');
    float printDev = (packDev / 1000.0);

    lcd.setCursor (14, 2);
    lcd.print (printDev);
    lcd.setCursor (19, 2);
    lcd.print ("V");

    char devPacket[5] = "-----";
    dtostrf(printDev, 4, 2, devPacket);
    //rf95.send((uint8_t *)devPacket, 5);
    //rf95.waitPacketSent();
  }
}

void Print_Alerts() {     //Print Alerts
  lcd.setCursor (5, 3);
  for (int i = 1; i <= 11; i++) {
    char c = Serial.read ();
    lcd.print(c);
  }
  delay (5000);
}

void Print_Uptime() {     //Print charge time elapsed
  int chargeHours;
  int chargeMins;
  int chargeSecs;

  if (Serial.find(": ")) {
    chargeHours =  Serial.parseInt();
  }

  if (Serial.find(", ")) {
    chargeMins =  Serial.parseInt();
  }

  if (Serial.find(", ")) {
    chargeSecs =  Serial.parseInt();
  }

  lcd.clear();
  lcd.print ("     -Charging-");
  lcd.setCursor (0, 1);
  lcd.print ("Hours: ");
  lcd.setCursor (7, 1);
  lcd.print (chargeHours);
  lcd.setCursor (0, 2);
  lcd.print ("Minutes: ");
  lcd.setCursor (9, 2);
  lcd.print (chargeMins);
  lcd.setCursor (0, 30);
  lcd.print ("smokeVal: ");
  lcd.setCursor (11, 3);
  lcd.print (smokeVal);
  delay (2000);
}

void PrintTH_1_2() {      //Print temps #1/2
  int TH1;
  int TH2;

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

void PrintTH_3_4() {      //Print temps #3/4
  int TH3;
  int TH4;

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
  delay (2000);
}

void smokeAlarm() {
  int analogPin = A0;     //MQ-135 smoke sensor
  int val = 0;      // 0-1035 smoke values
  smokeVal = analogRead(analogPin);  // read the input pin
}

void loop() {
  
 if (rf95.available()) {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  
    if (rf95.recv(buf, &len))
    {
      buf[len] = 0;
      lcd.setCursor (8, 3);
      lcd.print ((char*)buf);
      //Send_Pack (c);
    }
  }
}
  /*
  Serial.println ("show");

  if (Serial.available() > 0) {

    if (Serial.find("voltage"))
      Print_Voltage();

    if (Serial.find("mean   "))
      Print_Mean();

    if (Serial.find("std dev"))
      Print_Dev();

    if (Serial.find("alerts   : "))
      Print_Alerts();

    if (Serial.find("uptime   "))
      Print_Uptime();
  }

  Serial.println ("sh th");

  if (Serial.available() > 0) {

    if (Serial.find("2 | "))
      PrintTH_1_2();

    if (Serial.find("4 | "))
      PrintTH_3_4();
  }

  smokeAlarm();
}*/
