/* -------------------------------------------------- */
/* NAME:    CARD-READ SCRIPT                          */
/* AUTHOR:  John Willis                               */
/*                                                    */
/*                                                    */
/* PURPOSE: To read MiFare Ultralight Cards           */
/* -------------------------------------------------- */

/* Library Includes */
#include <SPI.h>        // Arduino library
#include <MFRC522.h>    // Library for RC522
#include <stdint.h>     // Library for data types.

/* Definitions for Pins */
#define SS_SDA_PIN 5    // SS/SDA Pin Location
#define RST_PIN 0       // RST Pin Location


MFRC522 rfid(SS_SDA_PIN, RST_PIN);  // Creating class for the reader.

void setup()
{
  Serial.begin(9600);   // Initialize the serial communication with the main device.
  SPI.begin();          // Initialization for the SPI bus.
  rfid.PCD_Init();      // Initialization for the MFRC522 components.

  /* Confirmations for data, display in terminal. */
  Serial.println();                                                                     // Clear the line.
  Serial.println(F("<Program Successful Setup>"));                                      // State success.
  Serial.println(F("This program is intended to scan Mifare Ultralight Cards."));       // Specify acceptable card types.
  Serial.println(F("Scan PICC to see data as follows: UID, type, and data blocks."));   // Give directions in Serial Window.
}

void loop()
{
  /* Setup a question function for reading or writing data. */
  char answer[2];           // Define character for the answer selection.
  bool isAnswered = false;  // Define boolean for exiting while loop.
  uint8_t returnedSize;    // Define a return int for readBytesUntil.

  while (!isAnswered)
  {
    for (uint8_t i = 0; i < 2; i++)     // Clear answer out.
      answer[i] = NULL;
    
    Serial.setTimeout(20000L);          // Wait 20 seconds as a timeout.
    Serial.println();
    Serial.println(F("Would you like to write or read data? (Type W/R followed by # sign.)"));
    returnedSize = Serial.readBytesUntil('#', answer, 2);

    if (answer[0] == 'R' || answer[0] == 'r' || answer[0] == 'W' || answer[0] == 'w')
      isAnswered = true;
  }

  /* Test if card is still there. */
  if (!rfid.PICC_IsNewCardPresent())     // Check if the card is present. If this fails, return to start of loop.
  {
    Serial.println(F("Take card away from sensor and replace."));
    return;
  }
  
  if (!rfid.PICC_ReadCardSerial())       // Verify a successful read. If this fails, return to start of loop.
  {
    Serial.println(F("Card Read Failure, please try again."));
    return;
  }

  /* Read Card UID and print. */
  Serial.print(F("Card UID (Hex):"));
  printHex(rfid.uid.uidByte, rfid.uid.size);  // Print in HEX
  Serial.println();
  Serial.print(F("Card UID (Dec):"));
  printDec(rfid.uid.uidByte, rfid.uid.size);  // Print in DEC
  Serial.println();

  /* Read Card PICC Type and print. */
  Serial.print(F("PICC Type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  if (answer[0] == 'w' || answer[0] == 'W')
  {
    /* Define constant variables and buffers. */
    uint8_t const blockAddress = 5;   // Block address for writing function.
    uint8_t const numBytes = 32;      // Number of characters to write.
    uint8_t length = numBytes;        // Modifiable number of bytes.
    byte buffer[numBytes];            // Buffer for serial input.

    /* Write prompting and get input. */
    Serial.println();   // Make newline before prompting terminal.
    Serial.println(F("Input the desired data string below, ending with a # sign. (Max 30 characters)"));   // Prompt user.
    length = Serial.readBytesUntil('#', buffer, numBytes);    // Read information into buffer.

    /* Print received data. */
    Serial.println(F("Data that you are writing: "));
    for (uint8_t i = 0; i < numBytes; i++)
      Serial.write(buffer[i]);
    Serial.println();

    /* Write data and confirm status of write. */
    MFRC522::StatusCode status = rfid.MIFARE_Write(blockAddress, buffer, length);  // Write to card.

    if (status == MFRC522::StatusCode::STATUS_OK)
      Serial.println(F("Writing successful. Read card to check data."));
    else
      Serial.println(F("Writing failure. Check card placement and try again."));

    Serial.println();   // Print newline at end of function.    
  }
  else if (answer[0] == 'r' || answer[0] == 'R')
  {
    /* Create constant variables and buffers and read data (32 bytes). */
    uint8_t const blockAddress = 5;     // Data block address we are reading from.
    uint8_t const numBytes = 32;        // Number of bytes to read.
    uint8_t length = numBytes;          // Non-constant size.
    byte data[numBytes];                // Data array as character.
    MFRC522::StatusCode status = rfid.MIFARE_Read(blockAddress, data, &length);   // Read values into data array.

    /* Confirm status of read, if successful, print information. */
    if (status == MFRC522::StatusCode::STATUS_OK)
    {
      Serial.print("Reading Data: ");

      for (uint8_t i; i < numBytes; i++)    // For the entire length of the character array.
        if (data[i] != NULL)                // If the data is valid, then write the data.
          Serial.write(data[i]);
      Serial.println();

      /* For Debug Purposes */
      Serial.print(F("Data Retrieved (Hex): "));
      printHex(data, numBytes);
      Serial.println();
      Serial.print(F("Data Retrieved (Dec): "));
      printDec(data, numBytes);
      Serial.println();
    }
    else
      Serial.println(F("Error reading card, check placement and try again."));

    Serial.println();   // Newline for reads.
  }
  else
  {
    Serial.println(F("ERROR IN READ/WRITE QUESTION!"));
    Serial.println(F("Did you write a valid value?"));
    Serial.println();
  }
}

/* Functions for dumping information. */
void printHex(byte *buffer, byte bufferSize)            // Print information in Hex.
{
  for (uint8_t i = 0; i < bufferSize; i++) 
    if (buffer[i] != NULL)
    {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
}

void printDec(byte *buffer, byte bufferSize)            // Print information in Dec.
{
  for (uint8_t i = 0; i < bufferSize; i++)
    if (buffer[i] != NULL)
    {
      Serial.print(' ');
      Serial.print(buffer[i], DEC);
    }
}