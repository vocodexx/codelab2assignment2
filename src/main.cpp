#include "ofMain.h"
#include "ofApp.h"

int main()
{
    // Set up the app window.
    ofGLFWWindowSettings settings;

    settings.setSize(
        1100,
        900
    );

    settings.windowMode =
        OF_WINDOW;

    // Create the window and start the app.
    auto window =
        ofCreateWindow(settings);

    ofRunApp(
        window,
        make_shared<ofApp>()
    );

    // Start the main loop.
    ofRunMainLoop();
}
