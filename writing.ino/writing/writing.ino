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
 Serial.println(F("Enter data, ending with #"));
 Serial.println("");
}
void loop(){
 for(byte i = 0; i < 6; i++){
 key.keyByte[i] = 0xFF;
 }
 if(!mfrc522.PICC_IsNewCardPresent()){
 return;
 }

 if (!mfrc522.PICC_ReadCardSerial()){
 Serial.println("[Bring PIIC closer to PCD]");
 return;
 }

 byte buffr[16];
 byte block = 8;
 byte len;

 Serial.setTimeout(1000L);
 /*
 * Read data from serial
 */
 len = Serial.readBytesUntil('#', (char *) buffr, 16) ;
 Serial.println("[PIIC ready!]");
 if(len>0){
 for(byte i = len; i < 16; i++){
 buffr[i] = ' '; //We have to pad array items with spaces.
 }
 String mString;
 mString = String((char*)buffr);

 Serial.println(" ");
 writeBytesToBlock(block, buffr);
 Serial.println(" ");
 mfrc522.PICC_HaltA();
 mfrc522.PCD_StopCrypto1();
 Serial.print("Saved on block ");
 Serial.print(block);
 Serial.print(":");
 Serial.println(mString);
 Serial.println("Note: To rewrite to this PICC, take it away from the PCD, and bring it closer again.");
 }
 delay(500);
}
void writeBytesToBlock(byte block, byte buff[]){
 card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));

 if(card_status != MFRC522::STATUS_OK) {
 Serial.print(F("PCD_Authenticate() failed: "));
 Serial.println(mfrc522.GetStatusCodeName(card_status));
 return;
 }

 else{
 Serial.println(F("PCD_Authenticate() success: "));
 }
 // Write block
 card_status = mfrc522.MIFARE_Write(block, buff, 16);

 if (card_status != MFRC522::STATUS_OK) {
 Serial.print(F("MIFARE_Write() failed: "));
 Serial.println(mfrc522.GetStatusCodeName(card_status));
 return;
 }
 else{
 Serial.println(F("Data saved."));
 }
} 