#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

class TimeManager {
public:
    // Syncs the internal RTC with the NTP server
    bool begin();

    // Gets the Unix Epoch timestamp (seconds since 1970)
    unsigned long getEpochTime();

    // Returns a formatted string like "2026-08-09" (Good for Firebase paths)
    String getDateString();

    // Returns a formatted string like "12:21:00"
    String getTimeString();
};

#endif