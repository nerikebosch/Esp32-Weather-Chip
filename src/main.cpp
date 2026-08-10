#include <Arduino.h>
#include "esp_system.h"
#include "esp_sleep.h"
#include <WiFi.h>                     // Added for Wi-Fi override
#include "soc/soc.h"                  // Added for Brownout override
#include "soc/rtc_cntl_reg.h"         // Added for Brownout override
#include "WifiManager/WifiManager.h"
#include "TimeManager/TimeManager.h"
#include "SensorManager/SensorManager.h"
#include "FirebaseManager/FirebaseManager.h"

#define ONBOARD_LED 2

WifiManager wifi;
TimeManager timeMgr;
SensorManager sensors;
FirebaseManager firebase(&timeMgr); 

unsigned long previousMillis = 0;
const long ONE_MINUTE_MS = 60000;
int minuteCounter = 0;

void setup() {
    // --- 1. THE BROWNOUT OVERRIDE ---
    CLEAR_PERI_REG_MASK(RTC_CNTL_BROWN_OUT_REG, RTC_CNTL_BROWN_OUT_ENA_M);
    
    Serial.begin(115200);
    
    pinMode(ONBOARD_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, HIGH); 
    
    // --- 2. THE POWER STABILIZER ---
    delay(2000); 
    
    Serial.println("\n--- Starting ESP32 Weather Station ---");
    Serial.printf("Reset reason: %d\n", esp_reset_reason());

    // --- 3. THE WI-FI FLUSH ---
    WiFi.disconnect(true, true);
    delay(500);
    WiFi.mode(WIFI_STA);
    delay(500);

    // --- THE BULLETPROOF WI-FI LOOP (now bounded, with forced restart) ---
    int connectAttempts = 0;
    const int MAX_ATTEMPTS_BEFORE_RESTART = 10;

    while (!wifi.isConnected()) {
        Serial.println("Attempting to connect to Wi-Fi...");
        wifi.begin();

        for (int i = 0; i < 15; i++) {
            if (wifi.isConnected()) break;
            digitalWrite(ONBOARD_LED, LOW);
            delay(250);
            digitalWrite(ONBOARD_LED, HIGH);
            delay(250);
        }

        connectAttempts++;

        if (!wifi.isConnected() && connectAttempts >= MAX_ATTEMPTS_BEFORE_RESTART) {
            Serial.println("WiFi stuck after repeated attempts. Deep sleeping briefly to fully reset RF hardware...");
            delay(200); // let the serial print flush
            esp_sleep_enable_timer_wakeup(1000000); // wake after 1 second
            esp_deep_sleep_start();
        }
    }
    
    Serial.println("Wi-Fi Connected successfully!");
    digitalWrite(ONBOARD_LED, HIGH);

    timeMgr.begin();

    if (!sensors.begin()) {
        Serial.println("Sensor failure. Check wiring!");
    }

    firebase.begin();
    
    Serial.println("Setup Complete. Entering Main Loop.");
    digitalWrite(ONBOARD_LED, LOW); 
}

void loop() {
    firebase.loop();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= ONE_MINUTE_MS) {
        previousMillis = currentMillis;

        if (wifi.isConnected()) {
            
            WeatherData currentData = sensors.getLiveReadings();
            
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

            digitalWrite(ONBOARD_LED, HIGH);
            delay(1000);
            digitalWrite(ONBOARD_LED, LOW);

        } else {
            Serial.println("Wi-Fi disconnected. Attempting reconnect...");
            digitalWrite(ONBOARD_LED, HIGH);
            wifi.begin();
            digitalWrite(ONBOARD_LED, LOW);
        }
    }
}