#include "SensorManager.h"

SensorManager::SensorManager() {
    resetAccumulators();
}

bool SensorManager::begin() {
    // Start I2C bus (ESP32 defaults: SDA = 21, SCL = 22)
    Wire.begin();

    // 1. Initialize BME280 (0x76 and 0x77 are standard addresses)
    if (!bme.begin(0x76)) {
        Serial.println("Error: BME280 sensor not found!");
        return false;
    }

    // 2. Initialize VEML7700
    if (!veml.begin()) {
        Serial.println("Error: VEML7700 sensor not found!");
        return false;
    }

    // VEML7700 settings tailored for outdoor ambient light
    veml.setGain(VEML7700_GAIN_1_8);
    veml.setIntegrationTime(VEML7700_IT_100MS);
    veml.interruptEnable(false);

    Serial.println("Sensors initialized successfully.");
    return true;
}

WeatherData SensorManager::getLiveReadings() {
    WeatherData data;
    
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F; // Convert Pa to hPa (millibars)
    data.lux = veml.readLux();

    return data;
}

void SensorManager::accumulateData(WeatherData currentData) {
    // Add to running totals
    sumTemp += currentData.temperature;
    sumHum += currentData.humidity;
    sumPres += currentData.pressure;
    sumLux += currentData.lux;
    readingCount++;

    // Track extremes
    if (currentData.temperature > maxTemp) {
        maxTemp = currentData.temperature;
    }
    if (currentData.temperature < minTemp) {
        minTemp = currentData.temperature;
    }
}

AggregatedData SensorManager::getHourlyAverageAndReset() {
    AggregatedData result;

    // Prevent division by zero if called before any data is collected
    if (readingCount == 0) {
        result = {0, 0, 0, 0, 0, 0, 0};
        return result;
    }

    // Calculate averages
    result.avgTemperature = sumTemp / readingCount;
    result.avgHumidity = sumHum / readingCount;
    result.avgPressure = sumPres / readingCount;
    result.avgLux = sumLux / readingCount;
    
    // Transfer extremes
    result.maxTemperature = maxTemp;
    result.minTemperature = minTemp;
    result.dataPointsCount = readingCount;

    // Reset everything for the next hour
    resetAccumulators();

    return result;
}

void SensorManager::resetAccumulators() {
    sumTemp = 0.0;
    sumHum = 0.0;
    sumPres = 0.0;
    sumLux = 0.0;
    readingCount = 0;
    
    // Reset extremes to artificial high/low thresholds so they trigger on the first reading
    maxTemp = -100.0; 
    minTemp = 100.0;  
}