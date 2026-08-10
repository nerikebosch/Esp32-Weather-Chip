#include "SensorManager.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// --- Safely declare the DS18B20 outside the class ---
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
// ----------------------------------------------------

SensorManager::SensorManager() {
    resetAccumulators();
}

bool SensorManager::begin() {
    // Start I2C bus (ESP32 defaults: SDA = 21, SCL = 22)
    Wire.begin();

    // 1. Initialize BME280
    if (!bme.begin(0x76)) {
        Serial.println("Error: BME280 sensor not found!");
        return false;
    }

    // 2. Initialize VEML7700
    if (!veml.begin()) {
        Serial.println("Error: VEML7700 sensor not found!");
        return false;
    }

    veml.setGain(VEML7700_GAIN_1_8);
    veml.setIntegrationTime(VEML7700_IT_100MS);
    veml.interruptEnable(false);

    // 3. Initialize the DS18B20
    ds18b20.begin();

    Serial.println("Sensors initialized successfully.");
    return true;
}

WeatherData SensorManager::getLiveReadings() {
    WeatherData data;
    
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F; // Convert Pa to hPa
    data.lux = veml.readLux();

    // Get Outside Temperature
    ds18b20.requestTemperatures(); 
    data.tempOut = ds18b20.getTempCByIndex(0); 

    return data;
}

void SensorManager::accumulateData(WeatherData currentData) {
    // Add to running totals
    sumTemp += currentData.temperature;
    sumTempOut += currentData.tempOut;
    sumHum += currentData.humidity;
    sumPres += currentData.pressure;
    sumLux += currentData.lux;
    readingCount++;

    // Track extremes
    if (currentData.temperature > maxTemp) maxTemp = currentData.temperature;
    if (currentData.temperature < minTemp) minTemp = currentData.temperature;

    if (currentData.tempOut > maxTempOut) maxTempOut = currentData.tempOut;
    if (currentData.tempOut < minTempOut) minTempOut = currentData.tempOut;
}

AggregatedData SensorManager::getHourlyAverageAndReset() {
    AggregatedData result;

    // Prevent division by zero
    if (readingCount == 0) {
        result = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return result;
    }

    // Calculate averages
    result.avgTemperature = sumTemp / readingCount;
    result.avgTempOut = sumTempOut / readingCount;
    result.avgHumidity = sumHum / readingCount;
    result.avgPressure = sumPres / readingCount;
    result.avgLux = sumLux / readingCount;
    
    // Transfer extremes
    result.maxTemperature = maxTemp;
    result.minTemperature = minTemp;
    result.maxTempOut = maxTempOut;
    result.minTempOut = minTempOut;
    
    result.dataPointsCount = readingCount;

    // Reset everything for the next hour
    resetAccumulators();

    return result;
}

void SensorManager::resetAccumulators() {
    sumTemp = 0.0;
    sumTempOut = 0.0;
    sumHum = 0.0;
    sumPres = 0.0;
    sumLux = 0.0;
    readingCount = 0;
    
    maxTemp = -100.0; 
    minTemp = 100.0;  
    maxTempOut = -100.0; 
    minTempOut = 100.0;
}