import serial

# Open the serial connection with Arduino
ser = serial.Serial('COM21', 9600) # Put your port here.

# Open the file to save the transactions
file = open('./data-store/transactions.txt', 'a')

while True:
    # Wait for the Arduino to send transaction information or prompt
    data = ser.readline().decode().strip()

    if data.startswith("Transaction completed successfully."):
        # Save the transaction in the file
        transaction = data
        file.write(transaction + '\n')
        file.flush()  # Ensure data is written immediately to the file

        # Print the transaction
        print('Transaction:', transaction)

    elif data.startswith("Enter"):
        # Prompt the user to enter a response
        response = input(data)

        # Send the response to Arduino
        ser.write(response.encode())
# Close the file and serial connection
file.close()
ser.close()
