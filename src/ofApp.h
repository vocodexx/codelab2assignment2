#pragma once

#include "ofMain.h"
#include "WeatherService.h"
#include "WeatherData.h"

// Main app class: input, drawing and screen state.
class ofApp : public ofBaseApp
{
public:
    // openFrameworks lifecycle.
    void setup();
    void update();
    void draw();
    void exit();

    // Input and HTTP callbacks.
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void urlResponse(ofHttpResponse& response);

private:
    // Current screen.
    enum class AppState
    {
        Idle,
        Loading,
        Success,
        Error
    };

    static const int FORECAST_DAYS = 3;

    // API service and parsed weather data.
    WeatherService weatherService;
    WeatherData weatherData;

    AppState appState = AppState::Idle;

    // Search input and status messages.
    string searchText;
    string lastSuccessfulSearch;
    string errorMessage;

    bool isSearchBoxActive = true;

    // Clickable areas.
    ofRectangle searchBox;
    ofRectangle searchButton;
    ofRectangle retryButton;
    ofRectangle refreshButton;

    // Current weather icon.
    ofImage weatherIcon;
    bool weatherIconLoaded = false;

    // One icon per forecast day.
    ofImage forecastIcons[FORECAST_DAYS];
    bool forecastIconLoaded[FORECAST_DAYS] =
    {
        false,
        false,
        false
    };

    // Search and refresh.
    void searchWeather();
    void refreshWeather();

    // Icon loading helpers.
    void loadWeatherIcon(const string& iconUrl);
    void loadForecastIcons();

    // Drawing helpers.
    void drawBackground();
    void drawHeader();
    void drawSearchArea();
    void drawIdleState();
    void drawLoadingState();
    void drawWeatherCard();
    void drawForecast();
    void drawErrorMessage();
    void drawRetryButton();
    void drawRefreshButton();

    // Formatting helpers.
    string getTemperatureText() const;
    string getTemperatureDescription() const;

    bool conditionContains(const string& keyword) const;

    // Pick background colours from condition and time of day.
    ofColor getBackgroundTop() const;
    ofColor getBackgroundBottom() const;

    // Trim the search text before validation.
    string trimCopy(const string& value) const;
};
