#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <TextAnimation.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 5 

ArduinoLEDMatrix matrix; // instance of LED matrix 
MFRC522 mfrc522(SS_PIN, RST_PIN);

byte authorizedUID[][4] = {
  {0xAA, 0x98, 0x85, 0xB1},
  {0xAA, 0xAA, 0xAA, 0xAA},
  {0xAA, 0xAA, 0xAA, 0xAA}
};

uint8_t good[8][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
};

uint8_t bad[8][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

uint8_t empty[8][12] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

const int numAuthorizedCards = sizeof(authorizedUID) / sizeof(authorizedUID[0]);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  matrix.begin();
  matrix.renderBitmap(empty, 8, 12);
  mfrc522.PCD_Init();
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

bool checkUID(byte *uid) {
  bool match;
  
  for(int i = 0; i < numAuthorizedCards; i++) {
    match = true;
    
    // Check each byte of the UID
    for(byte j = 0; j < 4; j++) {
      if(uid[j] != authorizedUID[i][j]) {
        match = false;
        break;  
      }
    }
    if(match) return true; 
  }
  return false;  
}

void successAuth() {
  matrix.renderBitmap(good, 8, 12);
  tone(BUZZER, 1000, 200); // beeps
  delay(200);
  tone(BUZZER, 1500, 200);
  delay(500);
  matrix.renderBitmap(empty, 8, 12);
}

void failAuth() {
  for(int i = 0; i < 3; i++){
    matrix.renderBitmap(bad, 8, 12);
    tone(BUZZER, 500, 200);
    delay(500);
    matrix.renderBitmap(empty, 8, 12);
  }
}


