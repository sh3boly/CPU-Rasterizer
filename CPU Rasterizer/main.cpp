#include "window.h"
#pragma comment(lib, "Winmm.lib")
#include <mmsystem.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    Frequency = freq.QuadPart;
    MSG msg;
    Buffer buffer; 
    std::vector<Triangle> mesh = loadObj("Untitled.obj");
    Window window(L"Renderer", hInstance, nCmdShow, &buffer);
    float* zBuffer = new float[buffer.width * buffer.height];
    clearZBuffer(zBuffer, buffer.width, buffer.height);
    bool sleepGranular = timeBeginPeriod(1);
    int framerate = 30;
    double secondsPerFrame = 1.0f / (float)framerate;
    while (running) {
        int64_t start = getTicks();
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (window.shouldResize) {
            resizeZBuffer(zBuffer, window.width, window.height);
            window.shouldResize = false;
        }
        int64_t end = getTicks();
        double elapsed = getElapsed(start, end);

       

        graphicsPipeline(&buffer, zBuffer, mesh);

        window.display();
        if (elapsed < secondsPerFrame) {
            if (sleepGranular) {
                DWORD msToSleep = (DWORD)(1000.0 * (secondsPerFrame - elapsed));
                if (msToSleep > 0.002) {
                    Sleep(msToSleep);
                }
                else {

                }
                while (elapsed < secondsPerFrame) {
                    elapsed = getElapsed(start, getTicks());
                }
            }
            else {
                while (elapsed < secondsPerFrame) {
                    elapsed = getElapsed(start, getTicks());
                }
            }
        }
        end = getTicks();
        double fps = 1.0 / getElapsed(start, end);
        window.showFPS(fps);
    }
}
