/*
UWE RACING
Daniel Nicklin
Formula Student Steering Wheel Display

Reads data from rfactor using a python plugi
Dsplays important variables on LCD, 7 segment and range of leds
Created 26/03/2013
Last Modified 26/03/2013

      v = Serial.read();
      newGear = v >> 4;
      newRpmValue = (v << 4 & 0xFF) >> 4;

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Pin Definitions
//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 8;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 10;
//Pin connected to Data in (DS) of 74HC595
const int dataPin = 9;
//Pin connected to wheel button
const int buttonPin = 11;
//Pin connected to wheel warning light
const int ledPin = 12;

// Warning light variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
const long interval = 100;           // interval at which to blink (milliseconds)

//Array for rpm led range
const int rpmArray[9] = {0,1,3, 7, 15, 31, 63, 127, 255}; 
//Array for gear indicator
const byte gearArray[10] = {B11010101,B10011111,B00100101,B00001101,B10011001,B01001001,B01000001,B00011111,B00000011}; 

//Variable definition
//Water Temperature
unsigned int waterTemp = 0;
//Oil Temperature
unsigned int oilTemp = 0;
//Oil Pressure
unsigned int oilPressure = 0;
//RPM Value
unsigned int rpmValue = 0;
// max rpm
int rpmMaxValue = 0;
//Vehicle Speed
int speedValue = 0;
//Gear Indicator
int gear = 0;
//Neutral Indicator
boolean neutral = 0;
//Button status
boolean button = 0;

//rFactor
int newGear = 0;
int newRpmValue = 0;
int rpmLedLevel = 0;
int rpmLedStart = 0;

//Serial buffer test
int pos = 0;
char buffer[8];


  void setup() {
    delay(1);
      // reserve 200 bytes for the inputString:
    
  // Setup - run once
  //Pin mode definitions
  //Shift Register pins
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);  
    pinMode(clockPin, OUTPUT);
  //Perpherial pins
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
  
  // set up the LCD's number of columns and rows: 
    lcd.begin(20, 2);
    lcd.clear();
    lcd.setCursor(5,0);
  // Print welcome message to the LCD.
    lcd.print("UWE RACING");
    lcd.setCursor(5,1);
  lcd.print("#2012-2013");
  
  //Serial
  Serial.begin(9600);
  
  //Test warning light
  digitalWrite(ledPin, HIGH);
    
  //Start up test of all leds on the steering wheel
  //Ascending
  for (int i = 1; i<9;i++){
        digitalWrite(latchPin, LOW);             //Pull latch LOW to start sending data
        shiftOut(dataPin, clockPin, LSBFIRST,gearArray[i]);          //Send the data
        shiftOut(dataPin, clockPin, MSBFIRST,rpmArray[i]);          //Send the data
        digitalWrite(latchPin, HIGH);            //Pull latch HIGH to stop sending data
        delay(100);
  }
  delay(200);
  //Decending
  for (int i = 7; i>-1;i--){
        digitalWrite(latchPin, LOW);             //Pull latch LOW to start sending data
        shiftOut(dataPin, clockPin, LSBFIRST,gearArray[i]);          //Send the data
        shiftOut(dataPin, clockPin, MSBFIRST,rpmArray[i]);          //Send the data
        digitalWrite(latchPin, HIGH);            //Pull latch HIGH to stop sending data
        delay(100);
  }
  //turn off warning led
  digitalWrite(ledPin, LOW);
  //Clear all data on screen
  lcd.clear();
  //Declare buffer and fill with blank data
  for (int i = 0; i < sizeof(buffer); i++)
    buffer[i] = 0;
  }
  
  void warningBlink(){
    unsigned long currentMillis = millis();
    
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
  }
  
  void lcdPrint()  {
  //Return cursor to start
  lcd.setCursor(0,0);
  lcd.print("GEAR:");
  //Switch function to display characters
   switch (newGear){
    case 0:
    lcd.print("N");
    break;
    case -1:
    lcd.print("R");
    break;
    default:
    lcd.print(newGear); 
 }
  lcd.setCursor(10,0);
  lcd.print("OILT:");
  lcd.print(oilTemp);
  // Oil temperature limit warning LCD + warning light
  if (oilTemp >= 130)
  {
  lcd.print("!");
   warningBlink();
  }
  lcd.print(" "); //Clear any old characters
  lcd.setCursor(0,1);
  lcd.print("RPM:");
  lcd.print(newRpmValue);
  lcd.print("  "); //Clear any old characters
  lcd.setCursor(10,1);
  lcd.print("WATER:");
  lcd.print(waterTemp);
  // Water temperature limit warning LCD +warning light
  if (waterTemp >= 130)
  {
   lcd.print("!");
   warningBlink();
  }
    lcd.print("  "); //Clear any old characters
}

  void loop() {
      lcdPrint(); // Update lcd
    while (Serial.available()) {
   char c = (char)Serial.read();
    //Check for end of carriage   
    if (c == '\n') {
  //asign values to variables   
   rpmMaxValue =(buffer[0]*1000);
   newGear = constrain(buffer[1], -1, 7); // gear value between -1 (R) and 7
   oilTemp = byte(buffer[2]);
   waterTemp = byte(buffer[3]);

    //Convert bytes in buffer to long integer for currrent rpm value  
    union u_tag {
    byte b[4];
    unsigned long ulval;
    } u;
    u.b[0] = buffer[4];
    u.b[1] = buffer[5];
    u.b[2] = buffer[6];
    u.b[3] = buffer[7];
    newRpmValue = u.ulval;
   
   //rpmLedLevel = (newRpmValue/(rpmMaxValue/7));
   
   // Sets rpm value range for which leds are lit
   rpmLedStart = rpmMaxValue*0.7; //Sets minimum
   
  // rpmMaxValue = rpmMaxValue * 1.05; //Game max value is set too low
   
   rpmLedLevel = map(constrain(newRpmValue,rpmLedStart,rpmMaxValue), rpmLedStart, rpmMaxValue, 0, 8);
      
      //Update shift registers if current gear or rpm has changed
   if (newGear != gear || newRpmValue != rpmValue){
       digitalWrite(latchPin, LOW);             //Pull latch LOW to start sending data
       shiftOut(dataPin, clockPin, LSBFIRST,gearArray[newGear]);          //Send the data
      shiftOut(dataPin, clockPin, MSBFIRST,rpmArray[rpmLedLevel]);          //Send the data
      digitalWrite(latchPin, HIGH);            //Pull latch HIGH to stop sending data
      gear = newGear;
      rpmValue = newRpmValue; 
   }     
      pos = 0; //Reset position in buffer to start
    }
    else {
      //Add serial read data to buffer and increment position
      buffer[pos] = c;
      pos++;
       //check buffer size has not been exceeded
       if (pos >= sizeof(buffer))
         pos = 0;
   }
  }
  }


