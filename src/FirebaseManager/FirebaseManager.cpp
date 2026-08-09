#include "FirebaseManager.h"
#include "Config.h"

// THIS is the fix: We pass ssl_client directly, and use the getNetwork() helper
FirebaseManager::FirebaseManager(TimeManager* tm) : aClient(ssl_client, getNetwork(network)) {
    timeManager = tm;
}

void FirebaseManager::begin() {
    Serial.println("Initializing Firebase (No Auth)...");
    
    // Firebase strictly requires SSL. We bypass certificate verification for simplicity.
    ssl_client.setInsecure(); 

    // 1. Initialize the Firebase App with NoAuth
    initializeApp(aClient, app, getAuth(noAuth));
    
    // 2. Bind the Realtime Database to the App
    app.getApp<RealtimeDatabase>(Database);
    
    // 3. Set your database URL from Config.h
    Database.url(FIREBASE_DATABASE_URL);
    
    Serial.println("Firebase Initialization complete.");
}

void FirebaseManager::loop() {
    // Keep internal Firebase tasks running
    app.loop();
    Database.loop();
}

void FirebaseManager::updateLiveWeather(WeatherData data) {
    String jsonStr = "{\"temperature\":" + String(data.temperature, 2) + 
                     ",\"humidity\":" + String(data.humidity, 2) + 
                     ",\"pressure\":" + String(data.pressure, 2) + 
                     ",\"lux\":" + String(data.lux, 2) + 
                     ",\"timestamp\":" + String(timeManager->getEpochTime()) + "}";

    // We wrap jsonStr in object_t() so Firebase correctly stores it as a JSON object
    Database.set<object_t>(aClient, "/weather/current", object_t(jsonStr), result);
    Serial.println("Live weather updated on Firebase.");
}

void FirebaseManager::pushHourlyHistory(AggregatedData data) {
    String jsonStr = "{\"avgTemperature\":" + String(data.avgTemperature, 2) + 
                     ",\"avgHumidity\":" + String(data.avgHumidity, 2) + 
                     ",\"avgPressure\":" + String(data.avgPressure, 2) + 
                     ",\"avgLux\":" + String(data.avgLux, 2) + 
                     ",\"maxTemperature\":" + String(data.maxTemperature, 2) + 
                     ",\"minTemperature\":" + String(data.minTemperature, 2) + 
                     ",\"dataPointsCount\":" + String(data.dataPointsCount) + 
                     ",\"timeString\":\"" + timeManager->getTimeString() + "\"}";

    // Create the path: /weather/history/hourly/2026-08-09
    String path = "/weather/history/hourly/" + timeManager->getDateString();

    // Wrap in object_t() and push
    Database.push<object_t>(aClient, path, object_t(jsonStr), result);
    Serial.println("Hourly history pushed to Firebase at: " + path);
}