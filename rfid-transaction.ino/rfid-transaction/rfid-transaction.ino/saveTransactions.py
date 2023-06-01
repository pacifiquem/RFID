import serial

# Open the serial connection with Arduino
ser = serial.Serial('COM21', 9600) # Put your port here.

# Open the file to save the transactions
file = open('transactions.txt', 'a')

while True:
    # Wait for the Arduino to send transaction information
    transaction = ser.readline().decode().strip()

    # Check if the transaction starts with "Transaction completed successfully."
    if transaction.startswith("Transaction completed successfully."):
        # Save the transaction in the file
        file.write(transaction + '\n')
        file.flush()  # Ensure data is written immediately to the file

        # Print the transaction
        print('Transaction:', transaction)

# Close the file and serial connection
file.close()
ser.close()
