#include "WeatherService.h"

#include <cctype>
#include <iomanip>
#include <sstream>

WeatherService::WeatherService()
{
    // This endpoint returns current weather plus the forecast.
    baseUrl = "https://api.weatherapi.com/v1/forecast.json";
}

void WeatherService::setApiKey(const string& key)
{
    apiKey = key;
}

bool WeatherService::hasApiKey() const
{
    return !apiKey.empty();
}

string WeatherService::urlEncode(const string& value) const
{
    // Encode spaces and special characters in the location.
    std::ostringstream encoded;
    encoded << std::uppercase << std::hex;

    for (unsigned char character : value)
    {
        if (std::isalnum(character) ||
            character == '-' ||
            character == '_' ||
            character == '.' ||
            character == '~')
        {
            encoded << static_cast<char>(character);
        }
        else
        {
            encoded << '%'
                    << std::setw(2)
                    << std::setfill('0')
                    << static_cast<int>(character);
        }
    }

    return encoded.str();
}

string WeatherService::createWeatherUrl(const string& location) const
{
    // Request three days; AQI and alerts are not used here.
    return baseUrl +
        "?key=" + urlEncode(apiKey) +
        "&q=" + urlEncode(location) +
        "&days=3"
        "&aqi=no"
        "&alerts=no";
}

bool WeatherService::parseWeatherResponse(
    const string& responseText,
    WeatherData& weatherData,
    string& errorMessage) const
{
    try
    {
        // Parse the response body.
        const ofJson json = ofJson::parse(responseText);

        // A valid response needs both sections.
        if (!json.contains("location") ||
            !json.contains("current"))
        {
            errorMessage = "Weather data is incomplete.";
            return false;
        }

        // Clear the previous result first.
        weatherData.clear();

        weatherData.city =
            json["location"].value("name", "");

        weatherData.country =
            json["location"].value("country", "");

        weatherData.region =
            json["location"].value("region", "");

        const ofJson& current =
            json["current"];

        // Read current values with defaults for missing fields.
        weatherData.temperatureC =
            current.value("temp_c", 0.0f);

        weatherData.feelsLikeC =
            current.value("feelslike_c", 0.0f);

        weatherData.humidity =
            current.value("humidity", 0);

        weatherData.windKph =
            current.value("wind_kph", 0.0f);

        weatherData.pressureMb =
            current.value("pressure_mb", 0.0f);

        weatherData.cloud =
            current.value("cloud", 0);

        weatherData.uvIndex =
            current.value("uv", 0.0f);

        weatherData.lastUpdated =
            current.value("last_updated", "");

        weatherData.isDay =
            current.value("is_day", 1) == 1;

        if (current.contains("condition"))
        {
            weatherData.condition =
                current["condition"].value(
                    "text",
                    ""
                );

            weatherData.iconUrl =
                current["condition"].value(
                    "icon",
                    ""
                );

            // Icon paths may start with //, so add https:.
            if (weatherData.iconUrl.rfind("//", 0) == 0)
            {
                weatherData.iconUrl =
                    "https:" + weatherData.iconUrl;
            }
        }

        // Parse the forecast if it is present.
        if (json.contains("forecast") &&
            json["forecast"].contains("forecastday"))
        {
            const ofJson& forecastDays =
                json["forecast"]["forecastday"];

            // Build one ForecastDay per entry.
            for (const auto& dayJson : forecastDays)
            {
                ForecastDay day;

                day.date =
                    dayJson.value("date", "");

                if (dayJson.contains("day"))
                {
                    const ofJson& weatherDay =
                        dayJson["day"];

                    day.maxTempC =
                        weatherDay.value(
                            "maxtemp_c",
                            0.0f
                        );

                    day.minTempC =
                        weatherDay.value(
                            "mintemp_c",
                            0.0f
                        );

                    day.maxWindKph =
                        weatherDay.value(
                            "maxwind_kph",
                            0.0f
                        );

                    day.chanceOfRain =
                        weatherDay.value(
                            "daily_chance_of_rain",
                            0
                        );

                    if (weatherDay.contains("condition"))
                    {
                        day.condition =
                            weatherDay["condition"].value(
                                "text",
                                ""
                            );

                        day.iconUrl =
                            weatherDay["condition"].value(
                                "icon",
                                ""
                            );

                        if (day.iconUrl.rfind("//", 0) == 0)
                        {
                            day.iconUrl =
                                "https:" + day.iconUrl;
                        }
                    }
                }

                // Store the completed day.
                weatherData.forecast.push_back(day);
            }
        }

        return true;
    }
    catch (const std::exception& exception)
    {
        // Treat malformed responses as a parse failure.
        ofLogError("WeatherService")
            << "JSON parse error: "
            << exception.what();

        errorMessage =
            "Could not read weather data.";

        return false;
    }
}

string WeatherService::parseApiError(
    const string& responseText) const
{
    try
    {
        // WeatherAPI puts error details inside an "error" object.
        const ofJson json =
            ofJson::parse(responseText);

        if (json.contains("error"))
        {
            return json["error"].value(
                "message",
                "Weather API error."
            );
        }
    }
    catch (...)
    {
        // Use a generic message if the error body is unreadable.
    }

    return "Unable to retrieve weather data.";
}
