#pragma once
#include "ofMain.h"

// Forecast values for one day.
class ForecastDay
{
public:
    // Date and condition.
    string date;
    string condition;
    string iconUrl;

    // Values shown on the forecast card.
    float maxTempC = 0.0f;
    float minTempC = 0.0f;
    float maxWindKph = 0.0f;

    int chanceOfRain = 0;
};
