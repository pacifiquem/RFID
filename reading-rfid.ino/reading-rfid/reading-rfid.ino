#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode card_status;
void setup(){
 Serial.begin(9600);
 SPI.begin();
 mfrc522.PCD_Init();
 Serial.println(F("PCD Ready!"));
}
void loop(){
 for (byte i = 0; i < 6; i++){
 key.keyByte[i] = 0xFF;
 }
 if(!mfrc522.PICC_IsNewCardPresent()){
 return;
 }
 if(!mfrc522.PICC_ReadCardSerial()){
 return;
 }

 Serial.println(F("\n*** Balance on the PICC ***\n"));
 String balance = readBytesFromBlock();
 Serial.println(balance);
 Serial.println(F("\n***************************\n"));
 delay(1000);

 mfrc522.PICC_HaltA();
 mfrc522.PCD_StopCrypto1();
}
String readBytesFromBlock(){
 byte blockNumber = 8;

 card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid));
 if(card_status != MFRC522::STATUS_OK){
 Serial.print(F("Authentication failed: "));
 Serial.println(mfrc522.GetStatusCodeName(card_status));
 return;
 }
 byte arrayAddress[18];
 byte buffersize = sizeof(arrayAddress);
 card_status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
 if(card_status != MFRC522::STATUS_OK){
 Serial.print(F("Reading failed: "));
 Serial.println(mfrc522.GetStatusCodeName(card_status));
 return;
 }

 String value = "";
 for (uint8_t i = 0; i < 16; i++){
 value += (char)arrayAddress[i];
 }
 value.trim();
 return value;
}