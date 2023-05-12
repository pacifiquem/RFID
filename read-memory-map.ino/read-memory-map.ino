#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {
 Serial.begin(9600);
 while (!Serial);
 SPI.begin();
 //Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
 mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07<<4));
 mfrc522.PCD_Init();
 delay(4);
 mfrc522.PCD_DumpVersionToSerial();
 Serial.println(F("DISPLAYING UID, SAK, TYPE, AND DATA BLOCKS:"));
}
void loop(){
 if(!mfrc522.PICC_IsNewCardPresent()){
 return;
 }
 if(!mfrc522.PICC_ReadCardSerial()){
 return;
 }
 /*
 Dump debug info about the card. Worry not. PICC_HaltA() will be automatically called at the end.
 */
 mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}