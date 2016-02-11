/*************************************************** 
  This sketch has the Arduino send a basic Get request to a web page
  
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
//2,4,6,7,8,9
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
  #define WEBPAGE      "/api/getDat"        // API Get page


//header
#include <LiquidCrystal.h>
#include <TimeAlarms.h>
#include <Time.h>
#include <TimeLib.h>

LiquidCrystal lcd(9,8,7,4,2,6);
//long randNum;
//int buttonState = 0;

String currentLine = "";
String getVar = "";
long checkupTime = 5000;
long lastTime = 0;
long cycles = 1;   

Tinysine_CC3000_Client www;

/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 
  Serial.print("Free RAM: ");
  Serial.println(getFreeRam(), DEC);
  
  //Setting output pins, to be controlled via Wifi
  /*
   * Basic Milestone 1
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  */

  /*
   * Basic Milestone 2, 3
  pinMode(A0, INPUT);
  */

  setTime(16,57,0,2,10,2016);
  
  lcd.begin(16, 2); 
  lcd.print("Alarm Clock"); 
  lcd.setCursor(0, 1); 
  lcd.print("");
  
 
 /* Initialise the module */
  connectToSite();
  
/* Set initial values */
  valueSet();
 
}


//--------------------------------------------------------
void loop(void)
{
  //tone(A0, 392);
  //noTone(A0);
  //ResetTimer();

  digitalClockDisplay();  
  delay(1000);
  
  /*
   * Basic Milestone 2
  buttonState = digitalRead(A0);
  if(buttonState == HIGH){
            //when button is pressed
            //generate a random number
            randNum = random(100);
            Serial.println(randNum);
            lcd.clear();
            lcd.begin(16, 2); 
            lcd.print("Random Number:"); 
            lcd.setCursor(0, 1); 
            lcd.print(randNum);
            delay(250);
          }
          else{
            //when button is not pressed
            
          }
          */


          
 if( millis()-lastTime > checkupTime)
 {
    Serial.println("Main Loop:");
    valueSet();
 }
 
}

/* connectToSite() is the function that initializes and connects the Wifi module to the wifi network and the Internet.  
* DO NOT MODIFY THIS FUNCTION
* You do not need to understand this function to understand the rest of the sketch
*/
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

/* valueSet() is the function in which things actually happen
* This function connects to the web page proper accesses the Get command on the API
* The webpage's API has a response for the Get command, which this function interprets
*/

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
   
    Serial.println("While Loop:");
    while(true){
            if (www.connected()) {
            Serial.println("Entering Commands");
            www.fastrprint(F("GET "));
            www.fastrprint(WEBPAGE);
            www.fastrprint(F(" HTTP/1.0\r\n"));
            www.fastrprint(F("Host: "));
            www.fastrprint(WEBSITE);
            www.fastrprint(F("\n"));
            www.fastrprint(F("Connection: close\n"));
           // www.fastrprint(F("Data Incoming:\n"));
            www.println();
          
        
          Serial.println(F("-------------------------------------\n"));
        
 //As long as there is output available, the Arduino will read output from the API

 



          while (www.connected()) {
            while (www.available()) {
              char c = www.read();  //Every line of data is downloaded one character at a time, into c
              currentLine += c; // Each character is added to the end of the currentLine variable
              Serial.print(c);
              if(c == '\n'){  
                currentLine = "";  //After each line, we want to clear the currentLine variable
              }
            }
          }
        
         } else {
           // Serial.println(F("Connection failed"));    
            return;
          }
          Serial.println(F("\n----------------------+---------------"));
          


         
         
          Serial.println("Final Reading:");
          Serial.println(currentLine);
          //getVar = currentLine.toInt(); // The last line of output will be the data we want to download.
          getVar = currentLine;
          
          Serial.print(cycles); //Number of times the loop has run
          Serial.println(" Cycles");
          
          delay(100);
          www.close();
          delay(200);
          cycles ++;

          lastTime = millis();



          //Now we decide what to do with our data

          //Checks for alarm time
          Serial.print("Alarm time:");
          Serial.println(getVar);

          String alarm_yyyy = getVar.substring(1,5);
          String alarm_mm = getVar.substring(6,8);
          String alarm_dd = getVar.substring(9,11);

          //checks for am/pm
          String alarm_h = getVar.substring(12,14);
          String alarm_ampm = "";

          //check for single digits
          //month() and day() are revered!
          String clock_dd = String(month()); 
          String clock_mm = String(day());
          if(int(month())<10){
            clock_dd = String(0)+month();
          }
          if(int(day())<10){
            clock_mm = String(0)+day();
          }

          if(alarm_h == "12"){
            alarm_h = "12";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "13"){
            alarm_h = "01";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "14"){
            alarm_h = "02";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "15"){
            alarm_h = "03";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "16"){
            alarm_h = "04";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "17"){
            alarm_h = "05";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "18"){
            alarm_h = "06";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "19"){
            alarm_h = "07";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "20"){
            alarm_h = "08";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "21"){
            alarm_h = "09";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "22"){
            alarm_h = "10";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "23"){
            alarm_h = "11";
            alarm_ampm = "PM";
          }
          else if(alarm_h == "24"){
            alarm_h = "12";
            alarm_ampm = "AM";
          }
          else{
            alarm_ampm = "AM";
          }
          
          String alarm_m = getVar.substring(15,17);
          String alarm_s = "0";

          Serial.println(alarm_yyyy);
          Serial.println(alarm_mm);
          Serial.println(alarm_dd);
          Serial.println(alarm_h);
          Serial.println(alarm_m);
          Serial.println(alarm_s);

          Serial.println("String(year()):");
          Serial.println(String(year()));

          String clock_hour = String(hour());
          if(hour() > 12){
            clock_hour = hour() - 12;
          }
          if(hour() < 10 || hour() > 12){
            clock_hour = String(0)+clock_hour;
          }
          
          Serial.println("time comparison:");
          Serial.println(alarm_h);
          Serial.println(clock_hour);
          Serial.println(alarm_m);
          Serial.println(String(minute()));
          Serial.println(alarm_s);
          Serial.println(String(second()));
          Serial.println(alarm_ampm);
          Serial.println(isAM());
          //compares alarm time with current time
          if(getVar == "0"){
            //alarm stopped
            noTone(A0);
            Serial.println("Alarm stopped");
            
          }
          else if (alarm_h.equals(clock_hour) && alarm_m.equals(String(minute())) && alarm_s.equals(String(second()))){
            Serial.println("Time match");
          }
          else if(alarm_yyyy.equals(String(year())) && alarm_mm.equals(clock_mm) && alarm_dd.equals(clock_dd) && alarm_h.equals(clock_hour) && alarm_m.equals(String(minute()))){
            
            if(isAM() == 1 && alarm_ampm == "AM"){
              //ring alarm
              Serial.println("Alarm match AM");
              tone(A0, 392);
            }
            else if(isPM() == 1 && alarm_ampm == "PM"){
              //ring 
              Serial.println("Alarm match PM");
              tone(A0, 392);
            }
          }
          
          //Checks if button is pressed

          /*
          if(digitalRead(5) == HIGH){
            //when button is pressed
            //generate a random number
            randNum = random(100);
            Serial.println(randNum);
            lcd.clear();
            lcd.begin(16, 2); 
            lcd.print("Random Number:"); 
            lcd.setCursor(0, 1); 
            lcd.print(randNum);
            delay(1000);
          }
          else{
            //when button is not pressed
          }
          */
          
          /*
           * Basic Milestone 1
           if(getVar==3)
                   { 
                     digitalWrite(7,HIGH);
                     digitalWrite(6,LOW);
                     digitalWrite(5,LOW);
                   }
                   else if(getVar==2)
                   {
                     digitalWrite(7,LOW);
                     digitalWrite(6,HIGH);
                     digitalWrite(5,LOW);
                   }
                   else if(getVar==1)
                   {
                     digitalWrite(7,LOW);
                     digitalWrite(6,LOW);
                     digitalWrite(5,HIGH);
                   }
                   else
                   {
                     digitalWrite(7,LOW);
                     digitalWrite(6,LOW);
                     digitalWrite(5,LOW);
                   }
          */
                   
                   
     /* The arduReset function is a last resort.
      * When your sketch simply will not loop correctly with the wifi connection, use this to automatically
      * reset the Arduino after each pull.
      * The drawback with arduReset is that since the Arduino resets, 
      * any peripherals it uses will be initialized when it resets,
      * so adjust the delay in the arduReset function to your liking, so that it at least powers the peripheral
      * for a short time before resetting.
      */
     
     // Serial.println("Soft Reset!");
     // arduReset();

  
    }
  
  
}





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

void digitalClockDisplay(){
  // digital clock display of the time
  int serial_hour = hour();
  if(hour() > 12){
    serial_hour = hour() - 12;
  }
  printDigits(serial_hour);
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  Serial.print(" ");
  //check for am/pm
  if(isAM()){
    Serial.print("AM");
  }
  else if(isPM()){
    Serial.print("PM");
  }
  
  Serial.print(" ");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year());
  Serial.println();

  //print to LCD
  lcd.begin(16, 2); 
  //date - dd/mm/yyyy
  lcd.print(day());
  lcd.print("/");
  lcd.print(month());
  lcd.print("/");
  lcd.print(year());
  
  lcd.setCursor(0, 1); 
  //time - hh:mm:ss AM/PM
  int clock_hour = hour();
  if(hour() > 12){
    clock_hour = hour() - 12;
  }
  printDigitsLCD(clock_hour);
  lcd.print(":");
  printDigitsLCD(minute());
  lcd.print(":");
  printDigitsLCD(second());
  lcd.print(" ");
  if(isAM()){
    lcd.print("AM");
  }
  else if(isPM()){
    lcd.print("PM");
  }
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printDigitsLCD(int digits){
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}
