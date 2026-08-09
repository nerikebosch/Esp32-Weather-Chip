#include "TimeManager.h"
#include "Config.h"
#include "time.h"

bool TimeManager::begin() {
    Serial.println("Syncing time with NTP server...");
    
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

    struct tm timeinfo;
    // Wait up to 5 seconds to get the time
    int retry = 0;
    while (!getLocalTime(&timeinfo) && retry < 10) {
        Serial.print(".");
        delay(500);
        retry++;
    }

    if (retry >= 10) {
        Serial.println("\nFailed to obtain time.");
        return false;
    }

    Serial.println("\nTime synced successfully!");
    Serial.println(getDateString() + " " + getTimeString());
    return true;
}

unsigned long TimeManager::getEpochTime() {
    time_t now;
    time(&now);
    return now;
}

String TimeManager::getDateString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "1970-01-01";
    }
    
    char dateBuffer[11]; // YYYY-MM-DD + null terminator
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", &timeinfo);
    return String(dateBuffer);
}

String TimeManager::getTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "00:00:00";
    }
    
    char timeBuffer[9]; // HH:MM:SS + null terminator
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
    return String(timeBuffer);
}