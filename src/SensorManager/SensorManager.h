#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>

// Struct to hold instantaneous readings
struct WeatherData {
    float temperature;
    float tempOut;     // <--- NEW: Outside Temp
    float humidity;
    float pressure;
    float lux;
};

// Struct to hold the hourly aggregated data
struct AggregatedData {
    float avgTemperature;
    float avgTempOut;  // <--- NEW
    float avgHumidity;
    float avgPressure;
    float avgLux;
    
    float maxTemperature;
    float minTemperature;
    float maxTempOut;  // <--- NEW
    float minTempOut;  // <--- NEW
    
    int dataPointsCount; // How many minutes of data were averaged
};

class SensorManager {
private:
    Adafruit_BME280 bme;
    Adafruit_VEML7700 veml;

    // Running totals for averages
    float sumTemp;
    float sumTempOut;  // <--- NEW
    float sumHum;
    float sumPres;
    float sumLux;

    // Daily/Hourly extremes
    float maxTemp;
    float minTemp;
    float maxTempOut;  // <--- NEW
    float minTempOut;  // <--- NEW

    // Counter for how many readings we have taken
    int readingCount;

    // Helper to reset variables
    void resetAccumulators();

public:
    SensorManager();
    
    bool begin();
    WeatherData getLiveReadings();
    
    // Pass the live readings here every minute to build the average
    void accumulateData(WeatherData currentData);
    
    // Call this every hour to get the math results and reset the counters
    AggregatedData getHourlyAverageAndReset();
};

#endif