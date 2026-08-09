#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// Wi-Fi Credentials
// ==========================================
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ==========================================
// Firebase Credentials
// ==========================================
#define FIREBASE_API_KEY "YOUR_FIREBASE_WEB_API_KEY"
#define FIREBASE_DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
#define FIREBASE_USER_EMAIL "YOUR_FIREBASE_USER_EMAIL"
#define FIREBASE_USER_PASSWORD "YOUR_FIREBASE_USER_PASSWORD"

// ==========================================
// Time & NTP Settings (South Africa Standard Time)
// ==========================================
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 7200      // UTC+2 (2 hours * 3600 seconds)
#define DAYLIGHT_OFFSET_SEC 0    // No Daylight Saving Time in SA

#endif