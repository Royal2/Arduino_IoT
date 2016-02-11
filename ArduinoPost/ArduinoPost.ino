/*************************************************** 
  This sketch has the Arduino send a basic Post request to a web page
  
 ****************************************************/
 
// Import libraries
#include <Tinysine_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

/******************************************************************************

The following Digital Pins are used by the Wifi shield and should be considered unavailable for *any* other purpose:

3
5
10
11
12
13

******************************************************************************/

// These are the interrupt and control pins
#define Tinysine_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define Tinysine_CC3000_VBAT  5
#define Tinysine_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Tinysine_CC3000 cc3000 = Tinysine_CC3000(Tinysine_CC3000_CS, Tinysine_CC3000_IRQ, Tinysine_CC3000_VBAT,
                                         SPI_CLOCK_DIV2); // you can change this clock speed

//Wifi Network credentials
#define WLAN_SSID       "Arduino24"           // Network name, cannot be longer than 32 characters!
#define WLAN_PASS       "KlaatuBaradaNikto"        // Network password
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// What page to grab!

  #define WEBSITE      "royiot.meteor.com" //domain the Arduino will access
  #define WEBPAGE      "/api/postDat/"        // API Get page

//header
#include <LiquidCrystal.h>

LiquidCrystal lcd(9,8,7,4,2,6);
const int sensorPin = A0; 
//float temperature;
float sensorVal=0;

String currentLine = "";
int getVar = 1000;
long checkupTime = 1000;
long lastTime = 0;
long pulseStart=0;
long cycles = 1;
float postData=0;
String postStr = "";

Tinysine_CC3000_Client www;


uint32_t ip;

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 
  Serial.print("Free RAM: ");
  Serial.println(getFreeRam(), DEC);
  
  lcd.begin(16, 2); 
  lcd.print("Weather App"); 
  //lcd.setCursor(0, 1); 
  //lcd.print("Sensor is Ready");
  
 /* Initialise the module */
  connectToSite();
  
/* Set initial values */
  valueSet();
  
}


//--------------------------------------------------------
void loop(void)
{

  sensorVal = analogRead(sensorPin);
  Serial.print("Sensor Value A0: "); 
  Serial.print(sensorVal);

  lcd.begin(16, 2); 
  lcd.print(sensorVal);

  delay (750);

  /*
  //convert the ADC reading to voltage 
  float voltage = (sensorVal/1024.0)*5.0; 
  //print new value to serial monitor 
  Serial.print(", Volts: "); 
  Serial.print(voltage);
  //convert the voltage to temperature in degrees 
  temperature = (voltage - .5)*100; 
  Serial.print(", degrees C: "); 
  Serial.println(temperature);

  //display temp onto LCD
  lcd.clear();
  lcd.begin(16, 2); 
  lcd.print("Temperature (C):"); 
  lcd.setCursor(0, 1); 
  lcd.print(temperature);
  delay(250);
    */      
 if( millis()-lastTime > checkupTime)
 {
    Serial.println("Main Loop:");
    valueSet();
 }
 
}
//----------------------------------------------------------

void connectToSite()
{
    
    
  // Optional SSID scan
  // listSSIDResults();
  
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  
  Serial.println(F("Connected to Wifi!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");  
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */  

  /* Try connecting to the website */
  
  
  Serial.print(WEBSITE); Serial.println(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
     

    
}

//------------------------------------------------------------------------------------------------------

void valueSet()
{
  Serial.println("");
    Serial.println("New Value!");
  long t = millis();
  long t_elapsed = 0;

     
  // do loop not in Adafruit
   do {    www = cc3000.connectTCP(ip, 80);
          t_elapsed = millis()-t; 
         } while((!www.connected()) && (t_elapsed < 5000));
         
         
         
   /*  Define the data you will be sending to the web page. 
   *   The fastrprint method will only take a char array, not a string
   *   We must first turn our data into a string, then turn that string into a number;
   *   If you need more characters, you can change the 20 to something else
   */
  postData = sensorVal;
  postStr = String(postData);
  Serial.println("postStr");
  Serial.println(postStr);
  int numChar = 20;
  char postChar[ numChar ] ;
  postStr.toCharArray(postChar, numChar);
   
    Serial.println("While Loop:");
    while(true){
            if (www.connected()) {
            Serial.println("Entering Commands");
            www.fastrprint(F("POST "));
            www.fastrprint(WEBPAGE);
            www.fastrprint(postChar);
            www.fastrprint(F(" HTTP/1.0\r\n"));
            www.fastrprint(F("Host: "));
            www.fastrprint(WEBSITE);
            www.fastrprint(F("\n"));
            www.fastrprint(F("Connection: close\n"));
           // www.fastrprint(F("Data Incoming:\n"));
            www.println();
          
        
          Serial.println(F("-------------------------------------\n"));
        
          while (www.connected()) {
            while (www.available()) {
              char c = www.read();
              currentLine += c;
              Serial.print(c);
              if(c == '\n'){
                currentLine = "";
              }
            }
          }
        
         } else {
           // Serial.println(F("Connection failed"));    
            return;
          }
          Serial.println(F("\n----------------------+---------------"));
          

          delay(100);
          www.close();
          delay(200);
         
         
          Serial.println("Final Reading:");
          Serial.println(currentLine);
          getVar = currentLine.toInt();

          Serial.print(cycles);
          Serial.println(" Cycles");
          cycles ++;
          pulseStart=millis();

          lastTime = millis();
          //Switch based on getVar


                   
                   
                   /* The arduReset function is a last resort.
                    * When your Wifi signal simply will not loop correctly, use this to automatically
                    * reset the Arduino after each pull.
                    * The drawback with ardu Reset is that since the Arduino resets, 
                    * any peripherals it uses will be initialized when it resets,
                    * so adjust the delay in the arduReset function to your liking, so that it at least powers the peripheral
                    * for a short time before resetting.
                    */
                   
                   // Serial.println("Soft Reset!");
                   // arduReset();

  
    }
  
  
}



/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

void arduReset(){
  
                                  #ifndef _SOFT_RESTART_H
                                  #define _SOFT_RESTART_H
                                  
                                  #include <avr/wdt.h>
                                  
                                  #define soft_restart()        \
                                  do                          \
                                  {                           \
                                      wdt_enable(WDTO_15MS);  \
                                      for(;;)                 \
                                      {                       \
                                      }                       \
                                  } while(0)
                                  
                                  #endif
                                  
                                  delay(10000);
                                  soft_restart();
}

