#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

#define SS_PIN 10
#define RST_PIN 9
#define GREEN_LED 6
#define RED_LED 7
#define BUZZER 5 

MFRC522 mfrc522(SS_PIN, RST_PIN);


byte authorizedUID[] = {0xAA, 0x98, 0x85, 0xB1};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println("RFID Authenticator Ready");
}

void loop() {
  if(!mfrc522.PICC_IsNewCardPresent()) return;
  if(!mfrc522.PICC_ReadCardSerial()) return;
  
  Serial.print("Card UID: ");
  for(byte i = 0; i < mfrc522.uid.size; i++){
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if(checkUID(mfrc522.uid.uidByte)) {
    successAuth();
  } else {
    failAuth();
  }

  mfrc522.PICC_HaltA();// stop reading after a read 
  delay(1500); // preventing double reads 
}

bool checkUID(byte *uid){
  for(byte i = 0; i < 4; i++){
    if(uid[i] != authorizedUID[i]) return false;
  }
  return true;
}

void successAuth() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  tone(BUZZER, 1000, 200); // beeps
  delay(200);
  tone(BUZZER, 1500, 200);
  delay(1000);
  digitalWrite(GREEN_LED, LOW);
}

void failAuth() {
  for(int i = 0; i < 3; i++){
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 500, 200);
    delay(100); // short delay cuz missing 220 Ohm resistor 
    digitalWrite(RED_LED, LOW); 
    // delay(1000);
    // digitalWrite(RED_LED, LOW);
    delay(500);
  }
}


