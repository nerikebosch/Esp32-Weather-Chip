#include <Arduino.h>
#include "WifiManager/WifiManager.h"
#include "TimeManager/TimeManager.h"
#include "SensorManager/SensorManager.h"
#include "FirebaseManager/FirebaseManager.h"

WifiManager wifi;
TimeManager timeMgr;
SensorManager sensors;
FirebaseManager firebase(&timeMgr); 

unsigned long previousMillis = 0;
const long ONE_MINUTE_MS = 60000;
int minuteCounter = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- Starting ESP32 Weather Station ---");

    if (!wifi.begin()) {
        Serial.println("Failed to connect to Wi-Fi. Restarting...");
        delay(5000);
        ESP.restart();
    }

    timeMgr.begin();

    if (!sensors.begin()) {
        Serial.println("Sensor failure. Check wiring!");
    }

    firebase.begin();
    
    Serial.println("Setup Complete. Entering Main Loop.");
}

void loop() {
    firebase.loop();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= ONE_MINUTE_MS) {
        previousMillis = currentMillis;

        if (wifi.isConnected()) {
            
            WeatherData currentData = sensors.getLiveReadings();
            
            // Updated print statement to show TempIn and TempOut side-by-side
            Serial.printf("[%s] TempIn: %.2fC | TempOut: %.2fC | Hum: %.2f%% | Pres: %.2fhPa | Lux: %.2f\n", 
                           timeMgr.getTimeString().c_str(), 
                           currentData.temperature, currentData.tempOut, currentData.humidity, 
                           currentData.pressure, currentData.lux);
                           
            firebase.updateLiveWeather(currentData);
            sensors.accumulateData(currentData);

            minuteCounter++;

            if (minuteCounter >= 60) {
                AggregatedData hourlyData = sensors.getHourlyAverageAndReset();
                firebase.pushHourlyHistory(hourlyData);
                
                minuteCounter = 0; 
            }
        } else {
            Serial.println("Wi-Fi disconnected. Attempting reconnect...");
            wifi.begin();
        }
    }
}