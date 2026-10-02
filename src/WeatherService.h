#pragma once

#include "ofMain.h"
#include "WeatherData.h"

// Handles WeatherAPI requests and response parsing.
class WeatherService
{
public:
    WeatherService();

    // API key loaded from config.json.
    void setApiKey(const string& key);
    bool hasApiKey() const;

    // Build the forecast URL for a location.
    string createWeatherUrl(const string& location) const;

    // Parse JSON into WeatherData.
    bool parseWeatherResponse(
        const string& responseText,
        WeatherData& weatherData,
        string& errorMessage
    ) const;

    // Read a useful message from an API error response.
    string parseApiError(const string& responseText) const;

private:
    string apiKey;
    string baseUrl;

    // URL-encode the location text.
    string urlEncode(const string& value) const;
};
