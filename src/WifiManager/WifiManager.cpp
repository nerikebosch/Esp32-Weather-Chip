#include "WifiManager.h"
#include "Config.h"

bool WifiManager::begin() {
    WiFi.persistent(false);
    WiFi.setSleep(false);
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Try to connect for ~10 seconds before giving up
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("\nFailed to connect to Wi-Fi.");
        return false;
    }
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}