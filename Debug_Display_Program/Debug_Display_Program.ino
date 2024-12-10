/* -------------------------------------------------- */
/* NAME:    CARD-READ SCRIPT                          */
/* AUTHOR:  John Willis                               */
/*                                                    */
/*                                                    */
/* PURPOSE: To read MiFare Ultralight Cards           */
/* and display the information on the ST7735          */
/* LED display from Adafruit.                         */
/* -------------------------------------------------- */

/* Library Includes */
#include <stdint.h>           // Library for data types.
#include <SPI.h>              // Arduino library for SPI controls.
#include <MFRC522.h>          // Library for RC522 scanning module.
#include <Adafruit_GFX.h>     // Core graphics library for Adafruit displays.
#include <Adafruit_ST7735.h>  // Hardware-specific library for TFT ST7735 displays from Adafruit.

/* Definitions for Pins */
#define RC522_SS_PIN      5   // SS/SDA Pin Location for RC522
#define RC522_RST_PIN     0   // RST Pin Location for RC522
#define ST7735_SS_PIN     21  // SS/SDA Pin Location for TFT ST7735 (TST_CS)
#define ST7735_DC_PIN     22  // DC Pin Location for TFT ST7735 (D/C)
#define ST7735_RST_PIN    2   // RST Pin Location for TFT ST7735

Adafruit_ST7735 led = Adafruit_ST7735(ST7735_SS_PIN, ST7735_DC_PIN, ST7735_RST_PIN);  // Creating class for TFT ST7735 Display
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);                                            // Creating class for RC522 Module

void setup()
{
  Serial.begin(9600);                               // Initialize the serial communication with the computer.
  SPI.begin();                                      // Initialization for the SPI bus for slave devices.
  rfid.PCD_Init();                                  // Initialization for the RC522 module.
  Serial.println();                                 // Clear the line for beginning of serial output.
  Serial.println(F("<RC522 Successfully Setup>"));  // Display initialization success for RC522 to serial window.

  led.initR(INITR_BLACKTAB);                                        // Initializion for the TFT ST7735 display.
  led.fillScreen(ST77XX_BLACK);                                     // Set the TFT ST7735 display to full black.
  drawTextAt(0, 0, "Press the button to scan.", ST77XX_WHITE);  // Display starting prompt to TFT ST7735 display.
  Serial.println(F("<TFT ST7735 Successfully Setup>"));             // Display initialization success for TFT ST7735 to serial window.

  /* Confirmations for data, display in terminal. */
  Serial.println(F("<Program Successful Setup>"));                                  // State success of program setting up.
  Serial.println(F("This program is intended to scan Mifare Ultralight Cards."));   // Specify acceptable card types in serial window.
  Serial.println(F("Scan PICC to see data as follows: UID and PICC Type"));         // Give directions in Serial Window for what to scan.
}

void loop()
{
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


}

/* Functions for displaying text and/or shapes to the TFT ST7735 Display. */
/* ------------------------------ */
/* drawTextAt - print text to tft */
/* display screen at specific     */
/* cursor with textwrap and color */
/* decided by parameters.         */
/* ------------------------------ */
void drawTextAt(uint8_t x, uint8_t y, char *text, uint16_t color)
{
  led.setCursor(x, y);
  led.setTextColor(color);
  led.setTextWrap(true);
  led.print(text);
}

/* ------------------------------ */
/* resetAll - reset cursor to     */
/* origin (0, 0) and set entire   */
/* screen to white, delay for 500 */
/* milliseconds and then set to   */
/* black and print to Serial that */
/* the screen has been reset.     */
/* ------------------------------ */
void resetAll()
{
  led.setCursor(0, 0);
  led.setTextColor(ST77XX_WHITE);
  led.fillScreen(ST77XX_WHITE);
  delay(500);
  led.fillScreen(ST77XX_BLACK);
  Serial.println(F("TFT ST7735 has been reset."));
}

/* Functions for dumping information from byte arrays, specifically for RC522 module. */
/* ------------------------------ */
/* printHex - print to serial     */
/* window the bytes from a buffer */
/* with spaces for each byte in   */
/* HEXadecimal format.            */
/* ------------------------------ */
void printHex(byte *buffer, byte bufferSize)            // Print information in Hex.
{
  for (uint8_t i = 0; i < bufferSize; i++) 
    if (buffer[i] != NULL)
    {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
}

/* ------------------------------ */
/* printDec - print to serial     */
/* window the bytes from a buffer */
/* with spaces for each byte in   */
/* DECimal format.                */
/* ------------------------------ */
void printDec(byte *buffer, byte bufferSize)            // Print information in Dec.
{
  for (uint8_t i = 0; i < bufferSize; i++)
    if (buffer[i] != NULL)
    {
      Serial.print(' ');
      Serial.print(buffer[i], DEC);
    }
}