#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

#define RST_PIN 9
#define SS_PIN 10
#define SD_CHIP_SELECT 4

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode card_status;

int moneyAmount = 0;
int pointsAmount = 0;

File myFile;

void setup() {
  Serial.begin(9600); // initialize Serial
  SPI.begin(); // initialize SPI
  mfrc522.PCD_Init(); // initialize RFID Read (PCD)

  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("SD card initialization failed.");
    while (1);
  }

  Serial.println("SD card initialized.\n");

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

  // Authenticate pointsBlock to read points amount
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

  while (!Serial.available()) {
    // Wait for input
  }

  char input = Serial.read();
  int amount = 0;

  if (input == 'm' || input == 'M') {
    Serial.println(F("Enter the amount to deduct from money: "));

    while (!Serial.available()) {
      // Wait for input
    }

    amount = Serial.parseInt();

    if (amount > moneyAmount) {
      Serial.println(F("Insufficient funds."));
      return;
    }

    moneyAmount -= amount;
    pointsAmount += 10; // Increased points by 10 because he/she used money

    bool saved_data_to_file = writeToSDCard(amount, "Money", moneyAmount); // save transaction to file

    // Send transaction information to Python code
    Serial.print(F("Transaction: Money, Deducted: $"));
    Serial.println(amount);

  } else if (input == 'p' || input == 'P') {
    Serial.println(F("Enter the amount to deduct from points: "));

    while (!Serial.available()) {
      // Wait for input
    }

    amount = Serial.parseInt();

    if (amount > pointsAmount) {
      Serial.println(F("Insufficient points."));
      return;
    }

    pointsAmount -= amount;

    bool saved_data_to_file = writeToSDCard(amount, "Points", pointsAmount); // save transaction to file

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
    Serial.println(F("Authentication error for pointsBlock."));
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

  Serial.println("\n \n");
  delay(5000);
}

bool writeToSDCard(int initial_balance, String mode, int new_balance) {
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("");
    myFile.print("Initial Balance: ");
    myFile.println(initial_balance);
    myFile.print("Mode Used: ");
    myFile.println(mode);
    myFile.print("New Balance: ");
    myFile.println(new_balance);
    myFile.println("\n");
    myFile.close();
    return true;
  } else {
    Serial.println("error opening data.txt");
    return false;
  }
}

