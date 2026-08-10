#include "FirebaseManager.h"
#include "Config.h"

FirebaseManager::FirebaseManager(TimeManager* tm) : aClient(ssl_client, getNetwork(network)) {
    timeManager = tm;
}

void FirebaseManager::begin() {
    Serial.println("Initializing Firebase (No Auth)...");
    ssl_client.setInsecure(); 
    initializeApp(aClient, app, getAuth(noAuth));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(FIREBASE_DATABASE_URL);
    Serial.println("Firebase Initialization complete.");
}

void FirebaseManager::loop() {
    app.loop();
    Database.loop();
}

void FirebaseManager::updateLiveWeather(WeatherData data) {
    // Appended tempOut to the JSON string
    String jsonStr = "{\"temperature\":" + String(data.temperature, 2) + 
                     ",\"tempOut\":" + String(data.tempOut, 2) + 
                     ",\"humidity\":" + String(data.humidity, 2) + 
                     ",\"pressure\":" + String(data.pressure, 2) + 
                     ",\"lux\":" + String(data.lux, 2) + 
                     ",\"timestamp\":" + String(timeManager->getEpochTime()) + "}";

    Database.set<object_t>(aClient, "/weather/current", object_t(jsonStr), result);
    Serial.println("Live weather updated on Firebase.");
}

void FirebaseManager::pushHourlyHistory(AggregatedData data) {
    Serial.println("\n==================================================");
    Serial.println("[DEBUG] FirebaseManager::pushHourlyHistory() Started");

    // Check if time manager returns valid values
    String timeStr = timeManager->getTimeString();
    String dateStr = timeManager->getDateString();
    unsigned long epochTime = timeManager->getEpochTime();

    Serial.printf("[DEBUG] Timestamp: %lu | Date: %s | Time: %s\n", epochTime, dateStr.c_str(), timeStr.c_str());

    // Build JSON String (includes timestamp!)
    String jsonStr = "{\"avgTemperature\":" + String(data.avgTemperature, 2) + 
                     ",\"avgTempOut\":" + String(data.avgTempOut, 2) + 
                     ",\"avgHumidity\":" + String(data.avgHumidity, 2) + 
                     ",\"avgPressure\":" + String(data.avgPressure, 2) + 
                     ",\"avgLux\":" + String(data.avgLux, 2) + 
                     ",\"maxTemperature\":" + String(data.maxTemperature, 2) + 
                     ",\"minTemperature\":" + String(data.minTemperature, 2) + 
                     ",\"maxTempOut\":" + String(data.maxTempOut, 2) + 
                     ",\"minTempOut\":" + String(data.minTempOut, 2) + 
                     ",\"dataPointsCount\":" + String(data.dataPointsCount) + 
                     ",\"timestamp\":" + String(epochTime) + 
                     ",\"timeString\":\"" + timeStr + "\"}";

    String path = "/weather/history/hourly/" + dateStr;

    Serial.println("[DEBUG] Target Path: " + path);
    Serial.println("[DEBUG] JSON Payload: " + jsonStr);
    Serial.println("[DEBUG] Sending request to Firebase...");

    // Execute Push
    bool status = Database.push<object_t>(aClient, path, object_t(jsonStr), result);

    if (status) {
        Serial.println("[SUCCESS] Hourly history pushed to Firebase successfully!");
    } else {
        Serial.println("[ERROR] Failed to push hourly history!");
        if (result.isError()) {
            Serial.printf("[ERROR Details] Code: %d | Msg: %s\n", 
                          result.appError().code(), 
                          result.appError().message().c_str());
        }
    }
    Serial.println("==================================================\n");
}