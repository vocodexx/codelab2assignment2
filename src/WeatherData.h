#pragma once

#include "ofMain.h"
#include "ForecastDay.h"

// Weather data used by the app.
// The API response is parsed into this object before display.
class WeatherData
{
public:
    // Location and current condition.
    string city;
    string country;
    string region;
    string condition;
    string iconUrl;
    string lastUpdated;

    // Current readings.
    float temperatureC = 0.0f;
    float feelsLikeC = 0.0f;
    float windKph = 0.0f;
    float pressureMb = 0.0f;
    float uvIndex = 0.0f;

    int humidity = 0;
    int cloud = 0;

    // WeatherAPI uses 1 for day and 0 for night.
    bool isDay = true;

    // Forecast for the next few days.
    vector<ForecastDay> forecast;

    // Clear old data before loading a new response.
    void clear()
    {
        city.clear();
        country.clear();
        region.clear();
        condition.clear();
        iconUrl.clear();
        lastUpdated.clear();

        temperatureC = 0.0f;
        feelsLikeC = 0.0f;
        windKph = 0.0f;
        pressureMb = 0.0f;
        uvIndex = 0.0f;

        humidity = 0;
        cloud = 0;
        isDay = true;

        forecast.clear();
    }
};
