// Copyright 2020 Varjo Technologies Oy. All rights reserved.

// Internal includes
#include "Globals.hpp"
#include "AppLogic.hpp"
#include "AppView.hpp"

// Common main function called from the entry point
void commonMain()
{
    // Instantiate application logic and view
    auto appLogic = std::make_unique<AppLogic>();
    auto appView = std::make_unique<AppView>(*appLogic);

    try {
        // Init application
        LOGD("Initializing application..");
        if (appView->init()) {
            // Enter the main loop
            LOGD("Running application..");
            appView->run();
        } else {
            LOGE("Initializing application failed.");
        }
    } catch (const std::runtime_error& e) {
        LOGE("Critical error caught: %s", e.what());
    }

    // Deinitialize client app
    LOGD("Deinitializing application..");
    appView.reset();
    appLogic.reset();

    // Exit successfully
    LOGI("Done!");
}

// Console application entry point
int main(int argc, char** argv)
{
    // Call common main function
    commonMain();

    // Application finished
    return EXIT_SUCCESS;
}

// Windows application entry point
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc = 0;
    LPWSTR* args = CommandLineToArgvW(pCmdLine, &argc);

    bool console = false;
    for (int i = 0; i < argc; i++) {
        if (std::wstring(args[i]) == (L"--console")) {
            console = true;
        }
    }

    if (console) {
        if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
            errno_t err;
            FILE* stream = nullptr;
            err = freopen_s(&stream, "CONOUT$", "w", stdout);
            err = freopen_s(&stream, "CONOUT$", "w", stderr);
        }
    }

    // Call common main function
    commonMain();

    // Application finished
    return 0;
}
