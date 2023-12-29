#include<EEPROM.h>//EEPROM header file
#include "Adafruit_Fingerprint.h" //fingerprint library header file
#include<LiquidCrystal_I2C.h> //lcd header file
LiquidCrystal_I2C lcd(0x27,16,2);
#include <SoftwareSerial.h>
#include<Keypad.h>
SoftwareSerial fingerPrint(2, 3); //for tx/rx communication between arduino & r305 fingerprint sensor
SoftwareSerial node_mcu(4,5);// tx/rx communication between arduino and nodeMCU


#include "RTClib.h" //library file for DS3231 RTC Module
RTC_DS3231 rtc;

uint8_t id,i;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

#define register_back 14
#define delete_ok 15

#define reverse 17
#define buzzer 16

#define records 100

char c;
String msg;
String d1,t1;
String dataSend;



DateTime now;

#define ROWS 4//four rows
#define COLS 4 //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



void setup(){
lcd.begin();
lcd.backlight();
Serial.begin(9600);
node_mcu.begin(9600);
pinMode(register_back, INPUT_PULLUP);

pinMode(reverse, INPUT_PULLUP);
pinMode(delete_ok, INPUT_PULLUP);
pinMode(buzzer, OUTPUT);
pinMode(13,INPUT);
digitalWrite(buzzer, LOW);
Serial.println("POWERED ON");
    //Serial.println("COnnecting...");
   
while(!node_mcu.available()){
 
   Serial.println("connecting...");
   lcd.print("Connecting..");
   delay(1000);
  }

lcd.clear();
lcd.print("Connected");
delay(1000);
lcd.clear();
lcd.print("  Fingerprint ");
lcd.setCursor(0,1);
lcd.print("Attendance System");
delay(1000);
lcd.clear();

digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);

finger.begin(57600);
lcd.clear();
lcd.print("Finding Module..");
lcd.setCursor(0,1);
delay(1000);
if (finger.verifyPassword())
{
Serial.println("Found fingerprint sensor!");
lcd.clear();
lcd.print(" Module Found");
delay(1000);
}
else
{
Serial.println("Did not find fingerprint sensor :(");
lcd.clear();
lcd.print("Module Not Found");
lcd.setCursor(0,1);
lcd.print("Check Connections");
delay(1000);
while (1);
}

if (! rtc.begin())
Serial.println("Couldn't find RTC");

rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

if (rtc.lostPower())
{
Serial.println("RTC is NOT running!");
rtc.adjust(DateTime(2023, 4, 17, 15, 29, 0));
lcd.clear();
}
}

void loop()
{
//StaticJsonBuffer<1000> jsonBuffer;
//JsonObject& root=jsonBuffer.createObject();
now = rtc.now();
lcd.setCursor(0,0);
lcd.print("Time: ");
lcd.print(now.hour(), DEC);
lcd.print(':');
lcd.print(now.minute(), DEC);
lcd.print(':');
lcd.print(now.second(), DEC);
lcd.print(" ");
lcd.setCursor(0,1);
lcd.print("Date: ");
lcd.print(now.day(), DEC);
lcd.print('/');
lcd.print(now.month(), DEC);
lcd.print('/');
lcd.print(now.year(), DEC);
lcd.print(" ");
delay(500);
//StaticJsonBuffer<1000> jsonBuffer;
//JsonObject& root=jsonBuffer.createObject();
int result=getFingerprintIDez();
if(result>0)
{

digitalWrite(buzzer, HIGH);
delay(100);
digitalWrite(buzzer, LOW);
node_mcu.print(result);
node_mcu.print("A");

node_mcu.print(now.hour());
node_mcu.print(":");
node_mcu.print(now.minute());
node_mcu.print(":");
node_mcu.print(now.second());
node_mcu.print("B");

node_mcu.print(now.day());
node_mcu.print("/");
node_mcu.print(now.month());
node_mcu.print("/");
node_mcu.print(now.year());
node_mcu.print("C");
node_mcu.print("\n");
delay(1000);
lcd.clear();
lcd.print("ID:");
lcd.print(result);
lcd.setCursor(0,1);
lcd.print("Please Wait....");
delay(500);
lcd.clear();
lcd.print("Attendance ");
lcd.setCursor(0,1);
lcd.print("Registered");
delay(1000);

return;
}
checkKeys();
delay(1000);
}

// dmyyhms - 7 bytes



void checkKeys()
{
if(digitalRead(register_back) == 0)
{
lcd.clear();
lcd.print("Please Wait");
delay(500);
Enroll();
}

else if(digitalRead(delete_ok) == 0)
{
lcd.clear();
lcd.print("Please Wait");
delay(2000);
delet();
}
}

void Enroll()
{
  String inputString_pin="";
  long int pin=0;
  String inputString="";
  long int count=0;
  lcd.clear();
  lcd.print("Enter PIN:");
  lcd.setCursor(0,1);
  while(1){
 
char key = customKeypad.getKey();

Serial.println(key);
if(key)
{
if(key >='0' && key <= '9') { // only act on numeric keys
      inputString_pin += key;      
      delay(100);// append new character to input string for pin
      pin=inputString_pin.toInt();
      Serial.println(inputString_pin);
      lcd.clear();
      lcd.print("Enter PIN:");
      lcd.setCursor(0,1);
      lcd.print(pin);
 }
}

if(digitalRead(delete_ok)==0){
  if(pin != 1234){
    Serial.println("Incorrect PIN");
    lcd.clear();
    delay(100);
    lcd.print("Incorrect PIN");
    delay(500);
    return;   
  }
  lcd.clear();
  lcd.print("Enter Finger ID:");
  lcd.setCursor(0,1);
  while(1){
  key = customKeypad.getKey();
  Serial.println(key);
  if(key)
    {
      if(key >='0' && key <= '9') { // only act on numeric keys
      inputString += key;      
      delay(100);// append new character to input string
      count=inputString.toInt();
      Serial.println(inputString);
      lcd.clear();
      lcd.print("Enter Finger ID: ");
      lcd.setCursor(0,1);
      lcd.print(count);
      }
    }

  if(digitalRead(delete_ok)==0){
    if(count>records || count<0){
    Serial.println("Error");
    lcd.clear();
    delay(100);
    lcd.print("Error");
    delay(500);
    return;   
    }
  i=EEPROM.read(EEPROM.length()-1);         //reading the index of the last stored ID from the EEPROM of arduino  
  for(int j=1;j<=i;j++){
    if(count == EEPROM.read(j)){
      Serial.println("ID already exist");
      lcd.clear();
      delay(100);
      lcd.print("ID already");
      lcd.setCursor(0,1);
      lcd.print("exist");
      delay(500);
      return;
    }
  }

  id=count;
  i++;
  EEPROM.update(i,id);                      //adding new ID to the EEPROM 
  EEPROM.update(EEPROM.length()-1, i);      //updating the index value with the index of new ID stored
  getFingerprintEnroll();
  inputString="";
  Serial.print("FINGERPRINT ID: ");Serial.println(id);
  delay(500);
  return;   
  }
if(digitalRead(reverse) == 0)
{
  int len=inputString.length();
  Serial.println(len);
  inputString.remove(len-1);                // remove last character of input string
  Serial.println(inputString);  
  count=inputString.toInt();
  lcd.clear();
  lcd.print("Enter Finger ID: ");
  lcd.setCursor(0,1);
  lcd.print(count);
  delay(100);
}

if(digitalRead(register_back) == 0)
{
return;
}
}
}
}
}

void delet()
{
String inputString_pin="";
  long int pin=0;
  String inputString="";
  long int count=0;
  lcd.clear();
  lcd.print("Enter PIN:");
  lcd.setCursor(0,1);
  while(1){
 
char key = customKeypad.getKey();

Serial.println(key);
if(key)
{
if(key >='0' && key <= '9') { // only act on numeric keys
      inputString_pin += key;      
      delay(100);// append new character to input string for pin
      pin=inputString_pin.toInt();
      Serial.println(inputString_pin);
      lcd.clear();
      lcd.print("Enter PIN:");
      lcd.setCursor(0,1);
      lcd.print(pin);
 }
}

if(digitalRead(delete_ok)==0){
  if(pin != 1234){
    Serial.println("Incorrect PIN");
    lcd.clear();
    delay(100);
    lcd.print("Incorrect PIN");
    delay(500);
    return;   
  }
  lcd.print("Enter Finger ID:");
  lcd.setCursor(0,1);
  while(1){
 
    char key = customKeypad.getKey();
Serial.println(key);
if(key)
{
if(key >='0' && key <= '9') {              // only act on numeric keys
      inputString += key;                  // append new character to input string      
      delay(100);
      count=inputString.toInt();
      Serial.println(inputString);
      lcd.clear();
      lcd.print("Enter Finger ID: ");
      lcd.setCursor(0,1);
      lcd.print(count);
}
  }

if(digitalRead(delete_ok)==0){
  int flag = 0;                            //initially assuming ID is not present in eeprom
  if(count>records || count<0){
    Serial.println("Error");
    lcd.clear();
    delay(100);
    lcd.print("Error");
    delay(500);
    return;   
  }
  for(int j=1;j<=i;j++){
  if(count==EEPROM.read(j)){
   flag=1;                    //ID found
  }
  if(flag==0){                //If ID is not present
    Serial.println("ID not found");
    lcd.clear();
    delay(100);
    lcd.print("ID NOT");
    lcd.setCursor(0,1);
    lcd.print("  FOUND  ");
    delay(500);
    return;  
  }
  }


  id=count;
  deleteFingerprint(id);
  inputString="";
  Serial.print("FINGERPRINT ID: ");Serial.println(id);
  delay(500);
  return;
   
  }
if(digitalRead(reverse) == 0)
{
  int len=inputString.length();
  Serial.println(len);
  inputString.remove(len-1);       // removing last character from input string
  Serial.println(inputString);
  count=inputString.toInt();
  lcd.clear();
  lcd.print("Enter Finger ID: ");
  lcd.setCursor(0,1);
  lcd.print(count);
  delay(100);
}

if(digitalRead(register_back) == 0)
{
return;
}
}
}
  }
}

uint8_t getFingerprintEnroll()
{
int p = -1;
lcd.clear();
lcd.print("finger ID:");
lcd.print(id);
lcd.setCursor(0,1);
lcd.print("Place Finger");
delay(2000);
while (p != FINGERPRINT_OK)
{
p = finger.getImage();
switch (p)
{
case FINGERPRINT_OK:
Serial.println("Image taken");
lcd.clear();
lcd.print("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.println("No Finger");
lcd.clear();
lcd.print("No Finger Found");
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
lcd.clear();
lcd.print("Comm Error");
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
lcd.clear();
lcd.print("Imaging Error");
break;
default:
Serial.println("Unknown error");
lcd.clear();
lcd.print("Unknown Error");
break;
}
}

// OK success!

p = finger.image2Tz(1);
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
lcd.clear();
lcd.print("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
lcd.clear();
lcd.print("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
lcd.clear();
lcd.print("Comm Error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
lcd.clear();
lcd.print("Feature Not Found");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
lcd.clear();
lcd.print("Feature Not Found");
return p;
default:
Serial.println("Unknown error");
lcd.clear();
lcd.print("Unknown Error");
return p;
}

Serial.println("Remove finger");
lcd.clear();
lcd.print("Remove Finger");
delay(2000);
p = 0;
while (p != FINGERPRINT_NOFINGER) {
p = finger.getImage();
}
Serial.print("ID "); Serial.println(id);
p = -1;
Serial.println("Place same finger again");
lcd.clear();
lcd.print("Place Finger");
lcd.setCursor(0,1);
lcd.print(" Again");
while (p != FINGERPRINT_OK) {
p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.print(".");
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
break;
default:
Serial.println("Unknown error");
return;
}
}

// OK success!

p = finger.image2Tz(2);
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
return p;
default:
Serial.println("Unknown error");
return p;
}

// OK converted!
Serial.print("Creating model for #"); Serial.println(id);

p = finger.createModel();
if (p == FINGERPRINT_OK) {
Serial.println("Prints matched!");
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
return p;
} else if (p == FINGERPRINT_ENROLLMISMATCH) {
Serial.println("Fingerprints did not match");
return p;
} else {
Serial.println("Unknown error");
return p;
}

Serial.print("ID "); Serial.println(id);
p = finger.storeModel(id);
if (p == FINGERPRINT_OK) {
Serial.println("Stored!");
lcd.clear();
lcd.print(" Finger Stored!");
delay(2000);
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
return p;
} else if (p == FINGERPRINT_BADLOCATION) {
Serial.println("Could not store in that location");
return p;
} else if (p == FINGERPRINT_FLASHERR) {
Serial.println("Error writing to flash");
return p;
}
else {
Serial.println("Unknown error");
return p;
}
}

int getFingerprintIDez()
{
uint8_t p = finger.getImage();

if (p != FINGERPRINT_OK)
return -1;

p = finger.image2Tz();
if (p != FINGERPRINT_OK)
return -1;

p = finger.fingerFastSearch();
if (p != FINGERPRINT_OK)
{
lcd.clear();
lcd.print("Finger Not Found");
lcd.setCursor(0,1);
lcd.print("Try Later");
delay(2000);
lcd.clear();
return -1;
}
// found a match!
Serial.print("Found ID #");
Serial.println(finger.fingerID);
return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id)
{
uint8_t p = -1;
lcd.clear();
lcd.print("Please wait");
p = finger.deleteModel(id);
if (p == FINGERPRINT_OK)
{
Serial.println("Deleted!");
lcd.clear();
lcd.print("Finger Deleted");
lcd.setCursor(0,1);
lcd.print("Successfully");
delay(1000);
}

else
{
Serial.print("Something Wrong");
lcd.clear();
lcd.print("Something Wrong");
lcd.setCursor(0,1);
lcd.print("Try Again Later");
delay(2000);
return p;
}
}
