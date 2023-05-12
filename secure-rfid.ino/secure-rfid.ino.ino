#include <SPI.h>

#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
char ACCESS_GRANTED[] = "granted";

const int SUCCESS_PIN = 7;
const int ERROR_PIN = 8;
const int BUZZER_PIN = 6;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    pinMode(SUCCESS_PIN, OUTPUT);
    pinMode(ERROR_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    Serial.println(F("READING THE CARD UNIQUE ID:"));
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}
void loop() {
    if (!rfid.PICC_IsNewCardPresent()) {
        return;
    }
    if (!rfid.PICC_ReadCardSerial()) {
        return;
    }
    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3]) {
        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }
        Serial.print(F("[check-access]"));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println(F("\n"));
        Serial.setTimeout(500);
        String res = Serial.readString();
        res.trim();
        Serial.print("|");
        Serial.print(res);
        Serial.println("|");
        if(res.equals(ACCESS_GRANTED)){
          Serial.println("ACCESS GRANTED");
          access_granted();
        }else {
          Serial.println("ACCESS DENIED");
          access_denied();
        }
    } else {
        Serial.println(F("This car was lastly detected ."));
        buzz("once");
    }
    /*
     * Halt PICC
     * Stop encryption on PCD
     */
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
void printHex(byte * buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void access_granted() {
  digitalWrite(SUCCESS_PIN, HIGH);
  digitalWrite(ERROR_PIN, LOW);
  buzz("twice");
  delay(1000);
  digitalWrite(SUCCESS_PIN, LOW);
}

void access_denied() {
  digitalWrite(ERROR_PIN, HIGH);
  digitalWrite(SUCCESS_PIN, LOW);
  buzz("long");
  digitalWrite(ERROR_PIN, LOW);
}

void buzz(String type){
  if(type.equals("once")) {
    analogWrite(BUZZER_PIN, 60);
    delay(60);
    analogWrite(BUZZER_PIN, 0);
  }else if(type.equals("twice")){
    analogWrite(BUZZER_PIN, 60);
    delay(60);
    analogWrite(BUZZER_PIN, 0);
    delay(60);
    analogWrite(BUZZER_PIN, 60);
    delay(60);
    analogWrite(BUZZER_PIN, 0);
  }else if(type.equals("long")){
    analogWrite(BUZZER_PIN, 60);
    delay(1000);
    analogWrite(BUZZER_PIN, 0);
  } else {
    analogWrite(BUZZER_PIN, 60);
    delay(60);
    analogWrite(BUZZER_PIN, 0);
  }
}