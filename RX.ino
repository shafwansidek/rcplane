#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "RF24.h"        //https://github.com/TMRh20/RF24
#define buzzer 5 //Buzzer
#define LEDR 4 // LED REd
#define LEDG 3 // LED Green

RF24 myRadio (9, 10);
byte addresses[][6] = {"0"}; 

int AlarmX = 0;
int HeightX = 0;

#define I2C_ADDR    0x27 // assigning the LCD address to communicate
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define _MAX_TEXT_BUFFER_ 40 //assigning max buffer size

#define buzzer 5
#define LEDR 4
#define LEDG 3
int n = 1;
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);



struct package
{
  int Alarm ;
  int Height ;
};


typedef struct package Package;
Package data;

void setup() 
{
  lcd.begin (16,2); //My LCD size is 16x2
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE); //assigning the pin for backlight which is pin 3
  lcd.setBacklight(HIGH); 
  lcd.home (); // go home

  pinMode(LEDG, OUTPUT); //Green
  pinMode(LEDR, OUTPUT); //RED
  pinMode(buzzer, OUTPUT); //Buzzer
  
  lcd.setCursor (0,0);
  lcd.print("Plane Position");
  lcd.setCursor (0,1);
  lcd.print("Height:");

  Serial.begin(9600);
  delay(100);
  digitalWrite(buzzer, LOW); 

  startWirelessCommunication();
}


void loop() 
{
   AlarmX = 0;
   HeightX = 0;
   lcd.setCursor (8,1);//this is to set the position of the LCD
   lcd.print("     ");
   checkForWirelessData();
   
   
   if (AlarmX == 1)
   {
    digitalWrite(buzzer, HIGH); 
    delay(500);
    digitalWrite(buzzer, LOW);
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
   }
   else
   {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
   }

}


void startWirelessCommunication()
{
  myRadio.begin();
  myRadio.setChannel(115);//assigning a channel for the transmission, can be any channel but must be the same both TX and RX
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);//send to the first address which is address pointing to array 0
  myRadio.startListening();//startListening means that this device will only receive data, while the other hardware will transmit signal
  delay(100);
}

void checkForWirelessData()
{
  if ( myRadio.available()) //when data is receive from the NRF24
  {
    while (myRadio.available())
    {
      myRadio.read( &data, sizeof(data) );
      AlarmX = data.Alarm;
      HeightX = data.Height;
    }
    Serial.print("\nPackage:");
    Serial.print("\n");
    Serial.println(data.Alarm);
    Serial.println(data.Height);
    lcd.setCursor (8,1);
    lcd.print(HeightX);
    lcd.setCursor (11,1);
    lcd.print("m");
    delay(500);

  } 
}
 
