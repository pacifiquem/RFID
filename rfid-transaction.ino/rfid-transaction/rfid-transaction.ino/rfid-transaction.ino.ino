/*
* Project Name: RFID Transaction Management System
* Author: Murangwa Pacifique
* Co-Authors: Ighor - Tresor
*/


#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode card_status;

int moneyAmount = 0;
int pointsAmount = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println(F("\n ==== Place the card near the reader. ==== \n"));

}

void loop() {



  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println(mfrc522.PICC_ReadCardSerial());
    Serial.println(F("Error reading card."));
    return;
  }

  byte buffer[18]; // Increased buffer size to account for null character
  byte moneyBlock = 4;
  byte pointsBlock = 8;
  byte len;

  Serial.println(F("Card detected."));

  // Authenticate moneyBlock to read money amount
  card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, moneyBlock, &key, &(mfrc522.uid));
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication error for moneyBlock."));
    return;
  }

  // Read money amount from moneyBlock
  card_status = mfrc522.MIFARE_Read(moneyBlock, buffer, &len);
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Error reading moneyBlock."));
    return;
  }

  buffer[len] = '\0'; // Null-terminate the buffer
  moneyAmount = atoi((char*)buffer);

  // Authenticate block 8 to read pointsBlock amount
  card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, pointsBlock, &key, &(mfrc522.uid));
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication error for pointsBlock."));
    return;
  }

  // Read points amount from pointsBlock
  card_status = mfrc522.MIFARE_Read(pointsBlock, buffer, &len);
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Error reading pointsBlock."));
    return;
  }

  buffer[len] = '\0'; // Null-terminate the buffer
  pointsAmount = atoi((char*)buffer);

  Serial.print(F("Available Money: $"));
  Serial.println(moneyAmount);
  Serial.print(F("Available Points: "));
  Serial.println(pointsAmount);

  Serial.println(F("Enter 'm' to use money, 'p' to use points:"));

  while (Serial.available() <= 0) {
    //Wait for input
  }

  char input = Serial.read();
  int amount = 0;

  if (input == 'm') {
    Serial.println(F("Enter the amount to deduct from money: "));

    while (Serial.available() <= 1) {
      // Wait for input
    }


    amount = Serial.parseInt();

    if (amount > moneyAmount) {
      Serial.println(F("Insufficient funds."));
      return;
    }

    moneyAmount -= amount;
    pointsAmount += 10; // Increased points by 10 because he/she used money

    Serial.println(F("Transaction completed successfully."));
  } else if (input == 'p') {
    Serial.println(F("Enter the amount to deduct from points: "));

    while (Serial.available() <= 1) {
      // Wait for input
    }

    amount = Serial.parseInt();

    if (amount > pointsAmount) {
      Serial.println(F("Insufficient funds."));
      return;
    }

    pointsAmount -= amount;
    Serial.println(F("Transaction completed successfully."));
  } else {
    Serial.println("Invalid Input.");
    return;
  }

  sprintf((char*)buffer, "%d", moneyAmount);
  card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, moneyBlock, &key, &(mfrc522.uid));
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication error for moneyBlock."));
    return;
  }
  card_status = mfrc522.MIFARE_Write(moneyBlock, buffer, 16);
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Error writing to moneyBlock."));
    return;
  }

  sprintf((char*)buffer, "%d", pointsAmount);
  card_status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, pointsBlock, &key, &(mfrc522.uid));
  
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication error for block 8."));
    return;
  }

  card_status = mfrc522.MIFARE_Write(pointsBlock, buffer, 16);
  
  if (card_status != MFRC522::STATUS_OK) {
    Serial.println(F("Error writing to pointsBlock."));
    return;
  }

  Serial.print(F("Remaining Money: $"));
  Serial.println(moneyAmount);
  Serial.print(F("Remaining Points: "));
  Serial.println(pointsAmount);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  Serial.println("Wait 10 seconds before making another transaction");

  delay(10000); // Delay to allow card removal before the next transaction
}
