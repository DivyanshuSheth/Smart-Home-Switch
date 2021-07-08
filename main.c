#include "dht.h"
#include <TimeLib.h>
#include <LiquidCrystal.h> 
#include "stdarg.h"


#define LDRInputPin A0                  // LDR sensor connected here
#define DHTInputPin A1                  // DHT sensor connected here


// SET ALL PARAMETERS HERE
const int aheadMinutes = 10;            // prior starting time for fan/AC (for Machine Learning based switching)
const int fanTemp = 30;                 // temperature at which fan turns on
const int fanHumidity = 75;             // humidity at which fan turns on
const int lightOnHourLDR = 17;          // time of day (hour) at which light switching is controlled by the LDR Input
const int lightOnMinuteLDR = 30;        // time of day (minutes) at which light switching is controlled by LDR Input
const int lightOnHour = 19;             // time of day at which light turns on automatically
const int LDRBoundary = 60;             // bounday LDR input value - for LDR input less than this, lights will turn on
const int hournow = 18;                  // set the current time (hour) while starting the system for the first time
const int minutenow = 55;                // set the current time (minute) while starting the system for the first time
const int secondnow = 0;                 // set the current time (second) while starting the system for the first time
const int daytoday = 11;                 // set the current date (day) while starting the system for the first time
const int monthtoday = 12;               // set the current date (month) while starting the system for the first time
const int yeartoday = 2021;              // set the current date (year) while starting the system for the first time


const int PIRInputPin = 13;             // PIR Input comes here
const int LightLEDPin = 7;              // Light connected here // IN1 on the relay
const int FanLEDPin = 8;                // Fan connected here // IN2 on the relay
const int LCDContrast = 125;            // Contrast on the LCD Display


int PIRState = LOW;  
int LDRState = 0;
int PIRval = 0;                         
int DHTHumidity = 0;
int DHTTemperature = 0;


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  
dht DHT;

// Machine Learning - Linear Regression for predicting time of arrival
#pragma once
//namespace Eloquent {
//    namespace ML {
//        namespace Port {
            class LinearRegression {
                public:
                    /**
                    * Predict class for features vector
                    */
                    float predict(float *x) {
                        return dot(x, -7.490451107479, -1.069948671851) + 2027.519167910283;
                    }

                protected:
                    /**
                    * Compute dot product
                    */
                    float dot(float *x, ...) {
                        va_list w;
                        va_start(w, 21);
                        float dot = 0.0;

                        for (uint16_t i = 0; i < 21; i++) {
                            const float wi = va_arg(w, double);
                            dot += x[i] * wi;
                        }

                        return dot;
                    }
                };
//            }
//        }
//    }


void setup() {
  
  Serial.begin(9600);
  delay(100);     // Delay to let system boot
  
  // LCD
  analogWrite(6, LCDContrast);
  lcd.begin(16, 2);

  // Time
  int hr = hournow, mn = minutenow, sc = secondnow, dd = daytoday, mm = monthtoday, yyyy = yeartoday;
  setTime(hr, mn, sc, dd, mm, yyyy);

  // Inputs/Outputs
  pinMode(LightLEDPin, OUTPUT);
  pinMode(FanLEDPin, OUTPUT);
  
  pinMode(DHTInputPin, INPUT);
  pinMode(LDRInputPin, INPUT);
  pinMode(PIRInputPin, INPUT);
  
  Serial.println("Sensors:\n1. DHT11 Humidity & Temperature Sensor\n2. LDR Light Sensor\n3. PIR Presence Sensor\n\n");

}

void loop() {
    time_t t = now();
    Serial.print("The time is: ");
    Serial.print(hour(t));
    Serial.print("hours and ");
    Serial.print(minute(t));
    Serial.print("minutes\n\n");
    
    // PIR
    Serial.print("PIR:\n");
    PIRval = digitalRead(PIRInputPin);  // read input value
    
    if (PIRval == HIGH) {            // check if the input is HIGH
    //      digitalWrite(LEDPin, HIGH);  // turn LED ON
      if (PIRState == LOW) {
        // we have just turned on
        Serial.println("Entered the room!\n");
        // We only want to print on the output change, not state
        delay(5000);
        PIRState = HIGH;
      }
      else {
        Serial.print("Left the room\n\n");
        delay(5000);
        PIRState = LOW;
      }
    }

    if (PIRState == LOW) {
      digitalWrite(LightLEDPin, LOW);
      
      // ML to predict time of arrival, and set FanLEDPin accordingly
      
      LinearRegression lr;
      float result;
      float input[2];
      input[0] = hour(t);
      input[1] = minute(t);
      result = lr.predict(input);

      if (result - (hour(t) * 60 + minute(t)) <= aheadMinutes) {
        digitalWrite(FanLEDPin, LOW);
      }
      else {
        digitalWrite(FanLEDPin, LOW);  
      }
      delay(2000);
      
    }
    
    else {
      
      // DHT 
      DHT.read11(DHTInputPin);
      Serial.print("DHT:\n");
      Serial.print("Current humidity = ");
      DHTHumidity = DHT.humidity;
      Serial.print(DHTHumidity);
      Serial.print("%  ");
      Serial.print("Temperature = ");
      DHTTemperature = DHT.temperature;
      Serial.print(DHTTemperature); 
      Serial.print("degrees C  \n\n");
      delay(200);


      // Print temperature and humidity on the LCD Display
      lcd.setCursor(0, 0);
      lcd.print("Temperature: ");
      lcd.print(DHTTemperature);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Humidity: ");
      lcd.print(DHTHumidity);
      lcd.print("%");
      

      if (DHTTemperature >= fanTemp) {
        digitalWrite(FanLEDPin, HIGH);
      }
      
      else if (DHTHumidity > fanHumidity) {
        digitalWrite(FanLEDPin, HIGH);
      }
      
      else {
        digitalWrite(FanLEDPin, LOW);
      }
      
      
      if (hour(t) < lightOnHourLDR || hour(t) == lightOnHourLDR && minute(t) <= lightOnMinuteLDR) {
        digitalWrite(LightLEDPin, LOW);
      }
      
      else if (hour(t) < lightOnHourLDR) {
        // LDR
        Serial.print("LDR:\n");
        LDRState = analogRead(LDRInputPin);
        Serial.print("Input: \n");
        Serial.print(LDRState);
        Serial.print("\n\n");
        delay(200);

        if (LDRState < LDRBoundary) {
          digitalWrite(LightLEDPin, HIGH);
        }
        else {
          digitalWrite(LightLEDPin, LOW);
        }
      }
      else {
        digitalWrite(LightLEDPin, HIGH);
      }
      
      
      delay(2000);                // Wait 2 seconds before accessing the sensors again
    }

}
