#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <FirebaseClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h> 
#include "SensorManager/SensorManager.h"
#include "TimeManager/TimeManager.h"

// Define the AsyncClient alias required by the Mobizt library
using AsyncClient = AsyncClientClass;

class FirebaseManager {
private:
    TimeManager* timeManager; 

    // Required Mobizt FirebaseClient objects for No Auth
    WiFiClientSecure ssl_client;
    DefaultNetwork network;
    
    // THIS is the fix: It must be AsyncClient, not FirebaseClient
    AsyncClient aClient;  
    
    NoAuth noAuth;
    FirebaseApp app;
    RealtimeDatabase Database;
    AsyncResult result;

public:
    FirebaseManager(TimeManager* tm);
    
    void begin();
    void loop();
    void updateLiveWeather(WeatherData data);
    void pushHourlyHistory(AggregatedData data);
};

#endif