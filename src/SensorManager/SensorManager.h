#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 

struct WeatherData {
    float temperature; // Inside (BME280)
    float tempOut;     // Outside (DS18B20)
    float humidity;
    float pressure;
    float lux;
};

struct AggregatedData {
    float avgTemperature; // Inside
    float avgTempOut;     // Outside
    float avgHumidity;
    float avgPressure;
    float avgLux;
    
    float maxTemperature; // Inside max
    float minTemperature; // Inside min
    float maxTempOut;     // Outside max
    float minTempOut;     // Outside min
    
    int dataPointsCount;
};

class SensorManager {
private:
    Adafruit_BME280 bme;
    Adafruit_VEML7700 veml;
    OneWire oneWire;
    DallasTemperature ds18b20;

    float sumTemp;
    float sumTempOut; // Running total for outside temp
    float sumHum;
    float sumPres;
    float sumLux;

    float maxTemp;
    float minTemp;
    float maxTempOut; // Daily/Hourly outside max
    float minTempOut; // Daily/Hourly outside min

    int readingCount;

    void resetAccumulators();

public:
    SensorManager();
    
    bool begin();
    WeatherData getLiveReadings();
    void accumulateData(WeatherData currentData);
    AggregatedData getHourlyAverageAndReset();
};

#endif