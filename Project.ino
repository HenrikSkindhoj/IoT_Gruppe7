#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//RFID
#include <SPI.h>
#include <MFRC522.h>
#include "secrets.h"
#define SS_PIN D8
#define RST_PIN D0
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int micValue = A0; 
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int StartRfid = 0;
int barks = 0;
int buzzPin = 4
int greenLED = 2
int redLED = 0

void setup() {
  Serial.begin(9600);  // Initialize serial
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  rfid.PCD_DumpVersionToSerial(); 
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.println(SECRET_SSID);
    digitalWrite(0, HIGH);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      StartRfid = 0;
        
      delay(4800);

      // green led blinking
      digitalWrite(greenLED, HIGH); 
      delay(200);
      digitalWrite(greenLED, LOW);     
    } 
    Serial.println("\nConnected.");
    digitalWrite(greenLED, LOW);
  }

  
  if (rfid.PICC_IsNewCardPresent()){ //looking for RFID card
    barks = 0; 
     
    if (StartRfid == 0){
       StartRfid = 1;
     
       delay(1000);
    }    
     else{
       StartRfid = 0;
       delay(1000);
       digitalWrite(greenLED, LOW);
       digitalWrite(redLED, HIGH);
    }
  }
  
  if (StartRfid == 1){
  float sensorValue = analogRead(micValue);
  // print out the value you read:
  Serial.println(sensorValue);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  
  
  
    if (sensorValue > 865){
      Serial.println("hund siger vuf"); //prints if the dog barks
      barks++; //counts number of barks
      digitalWrite(buzzPin, HIGH);   // turn the Buzzer on (HIGH is the voltage level) 
      delay(1000);// wait for a second
      digitalWrite(buzzPin, LOW);    // turn the Buzzer off by making the voltage LOW 
      int x = ThingSpeak.writeField(myChannelNumber, 1, number, myWriteAPIKey); //send data to thingspeak
      delay(19000); // wait 19 seconds

       
    }
  }
}
