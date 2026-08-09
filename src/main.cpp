#include <Arduino.h>
#include "WifiManager/WifiManager.h"
#include "TimeManager/TimeManager.h"
#include "SensorManager/SensorManager.h"
#include "FirebaseManager/FirebaseManager.h"

// --- Global Objects ---
WifiManager wifi;
TimeManager timeMgr;
SensorManager sensors;
FirebaseManager firebase(&timeMgr); // Pass timeMgr so Firebase can get timestamps

// --- Timing Variables ---
unsigned long previousMillis = 0;
const long ONE_MINUTE_MS = 60000;
int minuteCounter = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- Starting ESP32 Weather Station ---");

    // 1. Initialize Wi-Fi
    if (!wifi.begin()) {
        Serial.println("Failed to connect to Wi-Fi. Restarting...");
        delay(5000);
        ESP.restart();
    }

    // 2. Initialize Time
    timeMgr.begin();

    // 3. Initialize Sensors
    if (!sensors.begin()) {
        Serial.println("Sensor failure. Check wiring!");
        // We do not restart here, allow to troubleshoot via serial
    }

    // 4. Initialize Firebase
    firebase.begin();
    
    Serial.println("Setup Complete. Entering Main Loop.");
}

void loop() {
    // Keep Firebase internal tasks running
    firebase.loop();

    unsigned long currentMillis = millis();

    // Check if 1 minute has passed
    if (currentMillis - previousMillis >= ONE_MINUTE_MS) {
        previousMillis = currentMillis;

        // Ensure we are still connected to Wi-Fi
        if (wifi.isConnected()) {
            
            // --- 1. MINUTE TASKS (Live Data) ---
            WeatherData currentData = sensors.getLiveReadings();
            
            // Print to Serial for debugging
            Serial.printf("[%s] Temp: %.2fC | Hum: %.2f%% | Pres: %.2fhPa | Lux: %.2f\n", 
                           timeMgr.getTimeString().c_str(), 
                           currentData.temperature, currentData.humidity, 
                           currentData.pressure, currentData.lux);
                           
            // Send to Firebase and accumulate for the average
            firebase.updateLiveWeather(currentData);
            sensors.accumulateData(currentData);

            // Increment our hourly counter
            minuteCounter++;

            // --- 2. HOURLY TASKS (Historical Data) ---
            if (minuteCounter >= 60) {
                AggregatedData hourlyData = sensors.getHourlyAverageAndReset();
                firebase.pushHourlyHistory(hourlyData);
                
                minuteCounter = 0; // Reset counter for the next hour
            }
        } else {
            Serial.println("Wi-Fi disconnected. Attempting reconnect...");
            wifi.begin();
        }
    }
}