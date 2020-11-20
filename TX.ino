#include <math.h>
const int x_out = A1; //Accelerometer output for x-axis
const int y_out = A2; //Accelerometer output for y-axis
const int z_out = A3; //Accelerometer output for z-axis

#include <Wire.h>
#include <SPI.h>  
#include "RF24.h"

#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP3XX bmp; // Using I2C connection

RF24 myRadio (9, 10);
byte addresses[][6] = {"0"};
const int led_pin = 13;

float x;
float y;
float z;

struct package
{
  int Alarm ;
  int Height ;
};
typedef struct package Package;
Package data;

void setup()
{
  
    Serial.begin(9600);
    pinMode(led_pin, OUTPUT);
    myRadio.begin();  
    myRadio.setChannel(115); 
    myRadio.setPALevel(RF24_PA_MIN);
    myRadio.setDataRate( RF24_250KBPS ) ; 
    myRadio.openWritingPipe( addresses[0]);
    delay(1000);
    if (!bmp.begin_I2C()) 
    {
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      while (1);
    }
      // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

}
void loop()
{
  data.Alarm = 0; //the alarm will always be 0
  digitalWrite(led_pin, HIGH);
  readSensor();
  readtilt();
  Serial.println(data.Alarm);
  Serial.println(data.Height);

  myRadio.write(&data, sizeof(data)); 
  digitalWrite(led_pin, LOW);
  delay(200);
}


void readSensor()
{
  if (! bmp.performReading()) 
  {
    Serial.println("Failed to perform reading :(");
    return;
  }

  data.Height = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.println(data.Height);

    if (data.Height < 20) 
  {
     data.Alarm = 1;
  }
}



void readtilt()
{
int x_adc_value, y_adc_value, z_adc_value; 
double x_g_value, y_g_value, z_g_value, roll, pitch;
  
x_adc_value = analogRead(x_out); //Digital value of voltage on x_out pin
y_adc_value = analogRead(y_out); // Digital value of voltage on y_out pin
z_adc_value = analogRead(z_out); // Digital value of voltage on z_out pin

  
    x_g_value = ( ( ( (double)(x_adc_value * 5)/1024) - 1.65 ) / 0.330 ); // Acceleration in x-direction in g units. you can subtract 2 from the whole eqn to compensate for the error
    Serial.print("x = ");
    Serial.print(x_g_value);
    Serial.print("g");
    Serial.print("\n\n"); 
  
    y_g_value = ( ( ( (double)(y_adc_value * 5)/1024) - 1.65 ) / 0.330 ); // Acceleration in y-direction in g units. you can subtract 2 from the whole eqn to compensate for the error
    Serial.print("y = ");
    Serial.print(y_g_value);
    Serial.print("g");
    Serial.print("\n\n");
      
    z_g_value = ( ( ( (double)(z_adc_value * 5)/1024) - 1.80 ) / 0.330 ); // Acceleration in z-direction in g units. you can subtract 2 from the whole eqn to compensate for the error
    Serial.print("z = ");
    Serial.print(z_g_value);
    Serial.print("g");
    Serial.print("\n\n");

    roll = ( ( (atan2(y_g_value,z_g_value) * 180) / 3.14 ) + 180 ); // Formula for roll
    Serial.print("Roll = ");
    Serial.print(roll);
    Serial.print("\n\n");

    pitch = ( ( (atan2(z_g_value,x_g_value) * 180) / 3.14 ) + 180 ); // Formula for pitch
    Serial.print("Pitch = ");
    Serial.print(pitch);
    Serial.print("\n\n");

    if (roll < 115 || roll > 224)
    {
      data.Alarm = 1; 
    }
    if (pitch < 219 || pitch > 350) 
    {
      data.Alarm = 1;
    }

}
