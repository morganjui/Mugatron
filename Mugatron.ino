#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

#define MOSFET 3
#define LED_80 12
#define LED_60 10
#define LED_40 9
#define LED_POWER 13
#define THERM_PIN A5 //for Theristor
#define ONE_WIRE_BUS 5 //for DS18B20 Temperature Sensor

OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);
float waterTemp;
boolean isHeated;

enum {
  T_KELVIN=0,
  T_CELSIUS,
  T_FAHRENHEIT
};


void setup() /*----( SETUP: RUNS ONCE )----*/
{
  pinMode(MOSFET, OUTPUT);
  pinMode(LED_80, OUTPUT);
  pinMode(LED_60, OUTPUT);
  pinMode(LED_40, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
//  pinMode(THERM_PIN, INPUT);

  // initialize serial communications at 9600 bps
  Serial.begin(9600);
  delay(1000);
  
  digitalWrite(LED_POWER, HIGH);
  digitalWrite(MOSFET, HIGH);
  Serial.println("power on");
  isHeated = false; 

  digitalWrite(LED_40, LOW);
  digitalWrite(LED_60, LOW);
  digitalWrite(LED_80, LOW);
    
  sensors.begin();
  delay(1000);

}


void loop() // LOOP: RUNS CONSTANTLY
{
  
  Serial.print("Temperature from Thermister: ");
  Serial.print(getThermisterTemp(THERM_PIN, T_CELSIUS,3892.0f,298.15f,3000.0f,3300.0f));
  Serial.print(" degree C (ADC ");
  Serial.print(analogRead(THERM_PIN));
  Serial.println(")");
  

  Serial.print("Temperature from DS18B20: ");
  waterTemp = getDS18B20Temp();
  Serial.print(waterTemp);
  Serial.println(" degree C");
  Serial.println("");

  // turn off when reaching 80 degree C
  
  if (waterTemp < 80 && !isHeated) {
    digitalWrite(LED_POWER, HIGH);
    digitalWrite(MOSFET, HIGH);
    Serial.println("power is on");
  } else {
    digitalWrite(LED_POWER, LOW);
    digitalWrite(MOSFET, LOW);
    isHeated = true;
    Serial.println("power is off");
  }


  if(waterTemp < 40){
    digitalWrite(LED_40, LOW);
    digitalWrite(LED_60, LOW);
    digitalWrite(LED_80, LOW);
  }else if(waterTemp <= 60){ // green lightw up when between 40 ~ 60 degree C (for making tea)
    digitalWrite(LED_40, HIGH);
    digitalWrite(LED_60, LOW);
    digitalWrite(LED_80, LOW);
  }else if(waterTemp <= 80){ //white lights up when between 60 ~ 80 degree C (for making coffee)
    digitalWrite(LED_40, LOW);
    digitalWrite(LED_60, HIGH);
    digitalWrite(LED_80, LOW);
  }else{ // red light up when over 80 degree C
    digitalWrite(LED_40, LOW);
    digitalWrite(LED_60, LOW);
    digitalWrite(LED_80, HIGH);
  }
  
  delay(1000);
}

float getThermisterTemp(int AnalogInputNumber,int OutputUnit,float B,float T0,float R0,float R_Balance)
{
  float R,T;

  R = 1024.0f * R_Balance / float(analogRead(AnalogInputNumber)) - R_Balance;
  T = 1.0f / (1.0f / T0 + (1.0f / B) * log( R / R0 ));

  switch(OutputUnit) {
    case T_CELSIUS :
      T -= 273.15f;
    break;
    case T_FAHRENHEIT :
      T = 9.0f * (T-273.15f) / 5.0f + 32.0f;
    break;
    default:
    break;
  };

  return T;
}

float getDS18B20Temp()
{
//  Serial.println();
//  Serial.print("DS18B20 Requesting temperature...");
  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("DONE");
  
  float tempC = sensors.getTempCByIndex(0);
  
  return tempC;
}
