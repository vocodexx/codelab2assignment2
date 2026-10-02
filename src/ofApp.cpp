#include "ofApp.h"

#include <algorithm>
#include <cctype>
#include <cmath>

// GUI, input and request handling live here.

void ofApp::setup()
{
    // Initial app setup.
    ofSetWindowTitle("Weather App");
    ofSetWindowShape(1100, 900);
    ofSetFrameRate(60);

    ofEnableAntiAliasing();
    ofSetCircleResolution(50);

    // Load the API key from the data folder.
    try
    {
        const ofJson config =
            ofLoadJson("config.json");

        weatherService.setApiKey(
            config.value("weatherApiKey", "")
        );

        if (!weatherService.hasApiKey())
        {
            errorMessage =
                "Weather API key is missing.";

            appState =
                AppState::Error;
        }
    }
    catch (const std::exception&)
    {
        errorMessage =
            "Could not load config.json.";

        appState =
            AppState::Error;
    }

    // Listen for async URL responses.
    ofRegisterURLNotification(this);

    // Set the clickable areas for search, retry and refresh.
    searchBox.set(250, 135, 430, 58);
    searchButton.set(700, 135, 150, 58);

    retryButton.set(445, 455, 210, 55);
    refreshButton.set(770, 65, 150, 45);
}

void ofApp::update()
{
}

void ofApp::draw()
{
    // Draw common UI, then the current screen.
    drawBackground();
    drawHeader();
    drawSearchArea();

    // Draw the screen that matches the current state.
    switch (appState)
    {
        case AppState::Idle:
            drawIdleState();
            break;

        case AppState::Loading:
            drawLoadingState();
            break;

        case AppState::Success:
            drawWeatherCard();
            drawForecast();
            drawRefreshButton();
            break;

        case AppState::Error:
            drawErrorMessage();
            break;
    }
}

void ofApp::exit()
{
    // Remove the async URL listener.
    ofUnregisterURLNotification(this);
}

void ofApp::drawBackground()
{
    ofBackgroundGradient(
        getBackgroundTop(),
        getBackgroundBottom(),
        OF_GRADIENT_LINEAR
    );
}

void ofApp::drawHeader()
{
    ofSetColor(255);

    ofDrawBitmapString(
        "WEATHER",
        80,
        65
    );

    ofSetColor(255, 255, 255, 190);

    ofDrawBitmapString(
        "Live current weather and 3-day forecast",
        80,
        90
    );
}

void ofApp::drawSearchArea()
{
    // Search box and button.
    ofSetColor(0, 0, 0, 30);

    ofDrawRectRounded(
        searchBox.x + 3,
        searchBox.y + 4,
        searchBox.width,
        searchBox.height,
        10
    );

    ofSetColor(255, 255, 255, 238);

    ofDrawRectRounded(
        searchBox.x,
        searchBox.y,
        searchBox.width,
        searchBox.height,
        10
    );

    if (isSearchBoxActive &&
        appState != AppState::Loading)
    {
        ofNoFill();

        ofSetColor(255);
        ofSetLineWidth(2);

        ofDrawRectRounded(
            searchBox.x,
            searchBox.y,
            searchBox.width,
            searchBox.height,
            10
        );

        ofFill();
    }

    if (searchText.empty())
    {
        ofSetColor(130);

        ofDrawBitmapString(
            "Enter city e.g. London",
            searchBox.x + 20,
            searchBox.y + 35
        );
    }
    else
    {
        ofSetColor(35);

        ofDrawBitmapString(
            searchText,
            searchBox.x + 20,
            searchBox.y + 35
        );
    }

    bool loading =
        appState == AppState::Loading;

    bool hover =
        searchButton.inside(
            ofGetMouseX(),
            ofGetMouseY()
        );

    if (loading)
    {
        ofSetColor(105, 115, 130);
    }
    else if (hover)
    {
        ofSetColor(30, 65, 110);
    }
    else
    {
        ofSetColor(20, 50, 90);
    }

    ofDrawRectRounded(
        searchButton.x,
        searchButton.y,
        searchButton.width,
        searchButton.height,
        10
    );

    ofSetColor(255);

    if (loading)
    {
        ofDrawBitmapString(
            "LOADING...",
            searchButton.x + 35,
            searchButton.y + 35
        );
    }
    else
    {
        ofDrawBitmapString(
            "SEARCH",
            searchButton.x + 48,
            searchButton.y + 35
        );
    }
}

void ofApp::drawIdleState()
{
    ofSetColor(255, 255, 255, 220);

    ofDrawBitmapString(
        "Search for a city to view live weather data.",
        365,
        320
    );
}

void ofApp::drawLoadingState()
{
    // Loading spinner.
    float centreX =
        ofGetWidth() / 2.0f;

    float centreY = 345;

    float angle =
        ofGetElapsedTimef() * 220.0f;

    const int dotCount = 12;
    const float radius = 27.0f;

    for (int i = 0; i < dotCount; ++i)
    {
        float dotAngle =
            angle +
            static_cast<float>(i) *
            (360.0f / static_cast<float>(dotCount));

        float radians =
            ofDegToRad(dotAngle);

        float dotX =
            centreX +
            std::cos(radians) * radius;

        float dotY =
            centreY +
            std::sin(radians) * radius;

        int alpha =
            70 +
            (i * 185 / dotCount);

        ofSetColor(
            255,
            255,
            255,
            alpha
        );

        ofDrawCircle(
            dotX,
            dotY,
            4.0f
        );
    }

    ofSetColor(255);

    ofDrawBitmapString(
        "Loading weather...",
        centreX - 72,
        centreY + 75
    );
}

void ofApp::drawWeatherCard()
{
    // Current weather card.
    const float x = 180;
    const float y = 230;
    const float width = 740;
    const float height = 350;

    ofSetColor(0, 0, 0, 35);

    ofDrawRectRounded(
        x + 5,
        y + 7,
        width,
        height,
        20
    );

    ofSetColor(255, 255, 255, 228);

    ofDrawRectRounded(
        x,
        y,
        width,
        height,
        20
    );

    string location =
        weatherData.city +
        ", " +
        weatherData.country;

    ofSetColor(35, 45, 60);

    ofDrawBitmapString(
        location,
        x + 40,
        y + 42
    );

    if (!weatherData.region.empty())
    {
        ofSetColor(115);

        ofDrawBitmapString(
            weatherData.region,
            x + 40,
            y + 66
        );
    }

    if (weatherIconLoaded)
    {
        ofSetColor(255);

        weatherIcon.draw(
            x + 570,
            y + 30,
            100,
            100
        );
    }

    ofSetColor(25, 40, 60);

    ofDrawBitmapString(
        getTemperatureText(),
        x + 40,
        y + 120
    );

    ofSetColor(90);

    ofDrawBitmapString(
        weatherData.condition,
        x + 40,
        y + 150
    );

    ofSetColor(120);

    ofDrawBitmapString(
        getTemperatureDescription(),
        x + 40,
        y + 177
    );

    ofSetColor(210, 215, 220);

    ofDrawLine(
        x + 40,
        y + 195,
        x + width - 40,
        y + 195
    );

    ofSetColor(125);

    ofDrawBitmapString(
        "FEELS LIKE",
        x + 40,
        y + 228
    );

    ofDrawBitmapString(
        "HUMIDITY",
        x + 180,
        y + 228
    );

    ofDrawBitmapString(
        "WIND",
        x + 310,
        y + 228
    );

    ofDrawBitmapString(
        "PRESSURE",
        x + 430,
        y + 228
    );

    ofDrawBitmapString(
        "UV",
        x + 565,
        y + 228
    );

    ofSetColor(35, 45, 60);

    ofDrawBitmapString(
        ofToString(
            weatherData.feelsLikeC,
            1
        ) + " C",
        x + 40,
        y + 260
    );

    ofDrawBitmapString(
        ofToString(
            weatherData.humidity
        ) + "%",
        x + 180,
        y + 260
    );

    ofDrawBitmapString(
        ofToString(
            weatherData.windKph,
            1
        ) + " km/h",
        x + 310,
        y + 260
    );

    ofDrawBitmapString(
        ofToString(
            weatherData.pressureMb,
            0
        ) + " mb",
        x + 430,
        y + 260
    );

    ofDrawBitmapString(
        ofToString(
            weatherData.uvIndex,
            1
        ),
        x + 565,
        y + 260
    );

    ofSetColor(135);

    ofDrawBitmapString(
        "Last updated: " +
        weatherData.lastUpdated,
        x + 40,
        y + 315
    );
}

void ofApp::drawForecast()
{
    // Draw up to three forecast cards.
    if (weatherData.forecast.empty())
    {
        return;
    }

    const float startX = 180;
    const float startY = 630;

    const float cardWidth = 225;
    const float cardHeight = 190;
    const float gap = 30;

    ofSetColor(255, 255, 255, 220);

    ofDrawBitmapString(
        "3 DAY FORECAST",
        startX,
        startY - 25
    );

    int daysToDraw =
        std::min(
            FORECAST_DAYS,
            static_cast<int>(
                weatherData.forecast.size()
            )
        );

    for (int i = 0;
         i < daysToDraw;
         ++i)
    {
        const ForecastDay& day =
            weatherData.forecast[i];

        float x =
            startX +
            i * (cardWidth + gap);

        float y = startY;

        ofSetColor(0, 0, 0, 30);

        ofDrawRectRounded(
            x + 4,
            y + 5,
            cardWidth,
            cardHeight,
            14
        );

        ofSetColor(255, 255, 255, 222);

        ofDrawRectRounded(
            x,
            y,
            cardWidth,
            cardHeight,
            14
        );

        ofSetColor(35, 45, 60);

        string title;

        if (i == 0)
        {
            title = "TODAY";
        }
        else if (i == 1)
        {
            title = "TOMORROW";
        }
        else
        {
            title = day.date;
        }

        ofDrawBitmapString(
            title,
            x + 20,
            y + 30
        );

        if (forecastIconLoaded[i])
        {
            ofSetColor(255);

            forecastIcons[i].draw(
                x + cardWidth - 72,
                y + 16,
                52,
                52
            );
        }

        ofSetColor(100);

        ofDrawBitmapString(
            day.condition,
            x + 20,
            y + 70
        );

        ofSetColor(25, 40, 60);

        string temp =
            ofToString(day.maxTempC, 0) +
            " C / " +
            ofToString(day.minTempC, 0) +
            " C";

        ofDrawBitmapString(
            temp,
            x + 20,
            y + 105
        );

        ofSetColor(65, 110, 165);

        ofDrawBitmapString(
            "Rain: " +
            ofToString(day.chanceOfRain) +
            "%",
            x + 20,
            y + 135
        );

        ofSetColor(110);

        ofDrawBitmapString(
            "Wind: " +
            ofToString(day.maxWindKph, 0) +
            " km/h",
            x + 20,
            y + 162
        );
    }
}

void ofApp::drawErrorMessage()
{
    // Show input and API errors in the interface.
    float x = 300;
    float y = 285;

    ofSetColor(255, 245, 245, 240);

    ofDrawRectRounded(
        x,
        y,
        500,
        145,
        15
    );

    ofSetColor(185, 55, 55);

    ofDrawBitmapString(
        "WEATHER ERROR",
        x + 35,
        y + 42
    );

    ofSetColor(80);

    ofDrawBitmapString(
        errorMessage,
        x + 35,
        y + 80
    );

    ofSetColor(125);

    ofDrawBitmapString(
        "Check the location and try again.",
        x + 35,
        y + 112
    );

    drawRetryButton();
}

void ofApp::drawRetryButton()
{
    bool hover =
        retryButton.inside(
            ofGetMouseX(),
            ofGetMouseY()
        );

    if (hover)
    {
        ofSetColor(35, 95, 165);
    }
    else
    {
        ofSetColor(45, 110, 185);
    }

    ofDrawRectRounded(
        retryButton.x,
        retryButton.y,
        retryButton.width,
        retryButton.height,
        10
    );

    ofSetColor(255);

    ofDrawBitmapString(
        "TRY AGAIN",
        retryButton.x + 65,
        retryButton.y + 34
    );
}

void ofApp::drawRefreshButton()
{
    bool hover =
        refreshButton.inside(
            ofGetMouseX(),
            ofGetMouseY()
        );

    if (hover)
    {
        ofSetColor(255, 255, 255, 245);
    }
    else
    {
        ofSetColor(255, 255, 255, 195);
    }

    ofDrawRectRounded(
        refreshButton.x,
        refreshButton.y,
        refreshButton.width,
        refreshButton.height,
        9
    );

    ofSetColor(35, 55, 80);

    ofDrawBitmapString(
        "REFRESH",
        refreshButton.x + 43,
        refreshButton.y + 28
    );
}

void ofApp::searchWeather()
{
    // Validate the search, show loading, then start the request.
    if (appState == AppState::Loading)
    {
        return;
    }

    if (!weatherService.hasApiKey())
    {
        errorMessage =
            "Weather API key is missing.";

        appState =
            AppState::Error;

        return;
    }

    // Trim whitespace before checking the search.
    string location =
        trimCopy(searchText);

    if (location.empty())
    {
        errorMessage =
            "Please enter a city.";

        appState =
            AppState::Error;

        return;
    }

    // Clear old icons before loading new ones.
    weatherIconLoaded = false;

    for (int i = 0;
         i < FORECAST_DAYS;
         ++i)
    {
        forecastIconLoaded[i] = false;
    }

    errorMessage.clear();

    // Show the loading state while the request runs.
    appState =
        AppState::Loading;

    string url =
        weatherService.createWeatherUrl(
            location
        );

    // Keep the UI responsive while the request is running.
    ofLoadURLAsync(
        url,
        "weather_request"
    );
}

void ofApp::refreshWeather()
{
    // Refresh the last successful search.
    if (lastSuccessfulSearch.empty())
    {
        return;
    }

    searchText =
        lastSuccessfulSearch;

    searchWeather();
}

// Load the current-condition icon.
void ofApp::loadWeatherIcon(
    const string& iconUrl)
{
    if (iconUrl.empty())
    {
        weatherIconLoaded = false;
        return;
    }

    ofLoadURLAsync(
        iconUrl,
        "weather_icon"
    );
}

void ofApp::loadForecastIcons()
{
    // Give each forecast icon request a unique name.
    int count =
        std::min(
            FORECAST_DAYS,
            static_cast<int>(
                weatherData.forecast.size()
            )
        );

    for (int i = 0;
         i < count;
         ++i)
    {
        const string& iconUrl =
            weatherData.forecast[i].iconUrl;

        if (iconUrl.empty())
        {
            forecastIconLoaded[i] = false;
            continue;
        }

        ofLoadURLAsync(
            iconUrl,
            "forecast_icon_" +
            ofToString(i)
        );
    }
}

// Handle completed weather and icon requests.
void ofApp::urlResponse(
    ofHttpResponse& response)
{
    // Main WeatherAPI response.
    if (response.request.name ==
        "weather_request")
    {
        // HTTP 200: request succeeded.
        if (response.status == 200)
        {
            WeatherData newData;
            string parseError;

            bool success =
                weatherService
                .parseWeatherResponse(
                    response.data.getText(),
                    newData,
                    parseError
                );

            if (success)
            {
                // Update the displayed data only after a successful parse.
                weatherData =
                    newData;

                lastSuccessfulSearch =
                    trimCopy(searchText);

                appState =
                    AppState::Success;

                loadWeatherIcon(
                    weatherData.iconUrl
                );

                loadForecastIcons();
            }
            else
            {
                errorMessage =
                    parseError;

                appState =
                    AppState::Error;
            }
        }
        else
        {
            errorMessage =
                weatherService.parseApiError(
                    response.data.getText()
                );

            if (response.status <= 0)
            {
                errorMessage =
                    "Network connection failed.";
            }

            appState =
                AppState::Error;
        }

        return;
    }

    // Current-condition icon response.
    if (response.request.name ==
        "weather_icon")
    {
        if (response.status == 200)
        {
            weatherIconLoaded =
                weatherIcon.load(
                    response.data
                );
        }
        else
        {
            weatherIconLoaded = false;
        }

        return;
    }

    // Match each icon response to its forecast day.
    for (int i = 0;
         i < FORECAST_DAYS;
         ++i)
    {
        string requestName =
            "forecast_icon_" +
            ofToString(i);

        if (response.request.name ==
            requestName)
        {
            if (response.status == 200)
            {
                forecastIconLoaded[i] =
                    forecastIcons[i].load(
                        response.data
                    );
            }
            else
            {
                forecastIconLoaded[i] =
                    false;
            }

            return;
        }
    }
}

void ofApp::keyPressed(int key)
{
    // Ignore typing while a search is loading.
    if (appState == AppState::Loading)
    {
        return;
    }

    if (!isSearchBoxActive)
    {
        return;
    }

    // Enter triggers Search.
    if (key == OF_KEY_RETURN)
    {
        searchWeather();
        return;
    }

    if (key == OF_KEY_BACKSPACE)
    {
        if (!searchText.empty())
        {
            searchText.pop_back();
        }

        return;
    }

    if (key == OF_KEY_ESC)
    {
        searchText.clear();
        weatherData.clear();

        appState =
            AppState::Idle;

        return;
    }

    if (key >= 32 &&
        key <= 126 &&
        searchText.length() < 45)
    {
        searchText +=
            static_cast<char>(key);
    }
}

// Handle clicks on the active controls.
void ofApp::mousePressed(
    int x,
    int y,
    int button)
{
    isSearchBoxActive =
        searchBox.inside(x, y);

    if (searchButton.inside(x, y))
    {
        searchWeather();
        return;
    }

    if (appState == AppState::Error &&
        retryButton.inside(x, y))
    {
        searchWeather();
        return;
    }

    if (appState == AppState::Success &&
        refreshButton.inside(x, y))
    {
        refreshWeather();
    }
}

string ofApp::getTemperatureText() const
{
    return ofToString(
        weatherData.temperatureC,
        1
    ) + " C";
}

string ofApp::getTemperatureDescription() const
{
    // Turn the temperature into a short description.
    float temperature =
        weatherData.temperatureC;

    if (temperature <= 0)
    {
        return "Very cold";
    }

    if (temperature < 10)
    {
        return "Cold";
    }

    if (temperature < 18)
    {
        return "Cool";
    }

    if (temperature < 25)
    {
        return "Mild";
    }

    if (temperature < 30)
    {
        return "Warm";
    }

    return "Hot";
}

// Case-insensitive condition check for the background theme.
bool ofApp::conditionContains(
    const string& keyword) const
{
    string condition =
        weatherData.condition;

    string searchKeyword =
        keyword;

    std::transform(
        condition.begin(),
        condition.end(),
        condition.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    std::transform(
        searchKeyword.begin(),
        searchKeyword.end(),
        searchKeyword.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::tolower(character)
            );
        }
    );

    return condition.find(
        searchKeyword
    ) != string::npos;
}

ofColor ofApp::getBackgroundTop() const
{
    // Pick the top gradient colour.
    if (appState != AppState::Success)
    {
        return ofColor(35, 75, 125);
    }

    if (!weatherData.isDay)
    {
        return ofColor(15, 25, 55);
    }

    if (conditionContains("thunder"))
    {
        return ofColor(45, 45, 65);
    }

    if (conditionContains("rain") ||
        conditionContains("drizzle"))
    {
        return ofColor(60, 80, 105);
    }

    if (conditionContains("snow"))
    {
        return ofColor(150, 175, 195);
    }

    if (conditionContains("cloud"))
    {
        return ofColor(75, 105, 135);
    }

    return ofColor(45, 125, 205);
}

ofColor ofApp::getBackgroundBottom() const
{
    // Pick the lower gradient colour.
    if (appState != AppState::Success)
    {
        return ofColor(100, 165, 210);
    }

    if (!weatherData.isDay)
    {
        return ofColor(55, 65, 110);
    }

    if (conditionContains("rain") ||
        conditionContains("drizzle"))
    {
        return ofColor(120, 145, 165);
    }

    if (conditionContains("snow"))
    {
        return ofColor(220, 230, 240);
    }

    return ofColor(150, 205, 235);
}

// Return the string without leading or trailing whitespace.
string ofApp::trimCopy(
    const string& value) const
{
    auto first =
        std::find_if_not(
            value.begin(),
            value.end(),
            [](unsigned char character)
            {
                return std::isspace(character);
            }
        );

    auto last =
        std::find_if_not(
            value.rbegin(),
            value.rend(),
            [](unsigned char character)
            {
                return std::isspace(character);
            }
        ).base();

    if (first >= last)
    {
        return "";
    }

    return string(first, last);
}
