

// #define SIM800L_IP5306_VERSION_20190610
#define SIM800L_AXP192_VERSION_20200327
// #define SIM800C_AXP192_VERSION_20200609
// #define SIM800L_IP5306_VERSION_20200811

// Define the serial console for debug prints, if needed
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG          SerialMon
#include<string.h>
#include "utilities.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1
// Set serial for UART data (to send as message)
#define SerialUT  Serial2
// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800          // Modem is SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set RX buffer to 1Kb

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

char* lower = "";
char* response = " ";
String res = "";
bool new_data = 0;
int led = 0;

#define SMS_TARGET  "+919344868743"
char* msg = "+cmt: \"+919344868743\"";
char* ring = "+clip: \"+919344868743\"";

char* value1 = "led on";
char* value0 = "led off";

#define led_pin 13


void setup()
{

  // Set console baud rate
  SerialMon.begin(115200);
  pinMode(led_pin,OUTPUT);
  delay(10);

  // Start power management
  if (setupPMU() == false) {
    Serial.println("Setting power error");
  }

  // Some start operations
  setupModem();

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  SerialUT.begin(115200, SERIAL_8N1, uart_rx, uart_tx);

  delay(6000);

  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);
  delay(1000);

  SerialAT.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  SerialAT.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  SerialAT.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    SerialAT.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (SerialAT.available())
  {
    //Serial.write(SerialAT.read());//Forward what Software Serial received to Serial Port

    char add = SerialAT.read();
    res = res + add;
    delay(1);
    new_data = 1;
  }

  if (new_data) {
    response = &res[0];
    //------------------------------------------------- Converting every character of the String in lower form
    const int length = strlen( response ); // get the length of the text
    lower = ( char* )malloc( length + 1 ); // allocate 'length' bytes + 1 (for null terminator) and cast to char*
    lower[ length ] = 0; // set the last byte to a null terminator
    //------------------------------------------------- copy all character bytes to the new buffer using tolower
    for ( int i = 0; i < length; i++ )
    {
      lower[ i ] = tolower( response[ i ] );
    }
    Serial.println(lower);// printin the String in lower character form
    Serial.println("\n");
    led_control( lower);
    response = "";
    res = "";
    lower = "";
    new_data = 0;

  }
}

void led_control(char* lower)
{

  if (strstr(lower, msg))
  {
    String source = (String)lower;
    

    char* desti = &source[0];
    int i = 0;
    while (desti[i+2] != '\n' )
      i++;
    String destin = (String)desti;
    source = source.substring(51);
    
    Serial.print("Your message is = ");Serial.print(source);
    char* led1 = &source[0];
    if(strstr(led1, value1))
    {
      digitalWrite(led_pin, HIGH);
      res = modem.sendSMS(SMS_TARGET, String("led is turned on"));
     
      DBG("SMS:", res ? "OK" : "fail");
      led = 1;
      return; 
      
    }
    if(strstr(led1, value0))
  {
    digitalWrite(led_pin, LOW);
    led = 0;
     // Read data from UART of another board
      String uartData = ""; // Stores the received UART data
      
      // Read data from UART until timeout or a specific delimiter (e.g., newline)
      unsigned long startMillis = millis();
      while (millis() - startMillis < 4000) { // Read for up to 4 seconds
        if (SerialUT.available()) {
         
          char incomingChar = SerialUT.read();
          if (incomingChar == '\n') { // End of data
            break;
          }
          uartData += incomingChar;
        }
      }

      // Send the received UART data to SMS_TARGET
      if (uartData!=NULL) {
        res = modem.sendSMS(SMS_TARGET, "Received UART Data: " + uartData);
        DBG("SMS:", res ? "OK" : "fail");
      } else {
        SerialMon.println("No UART data received.");
      }

      return;
  }
  }

}
//CREATED SERIALUT.......FROM serial 2,complete code