#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WifiManager {
public:
    // Attempts to connect to Wi-Fi. Returns true if successful.
    bool begin();
    
    // Checks if the ESP32 is currently connected
    bool isConnected();
};

#endif