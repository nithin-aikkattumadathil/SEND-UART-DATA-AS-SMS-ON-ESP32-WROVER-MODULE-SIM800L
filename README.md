# SEND-UART-DATA-AS-SMS-ON-ESP32-WROVER-MODULE-SIM800L
* This project is for sending available uart data as message to a mobile number
# Components used are :
* --- Arduino UNO board for transfering uart data "test" --- ESP32 Wrover Module with SIM800L ,for receiving message and for sending message
* I've mounted my VI SIM on wrover
* Connect TX pin of UNO to RX pin of wrover , and ground to ground
# CODE FOR UNO
* 
const int ledPin = 13;
void setup() {
  Serial.begin(115200); // Initialize Serial communication at 9600 baud
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Transmit a test message
  if(Serial.println("test"))
{
    digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
  delay(50);
}
  delay(1000); // Wait for a second
}
* If i send "led on" from the specified number , on board blue LED on wrover will turn on and replies with a message "led is turned on" from the wrover module
* If i send "led off" from the specified number , on board blue LED will turn off and if UART data is available on RX pin, that data will send to the specified number as reply
   
