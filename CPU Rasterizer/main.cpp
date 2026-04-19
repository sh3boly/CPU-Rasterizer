#include "window.h"
#pragma comment(lib, "Winmm.lib")
#include <mmsystem.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    //AllocConsole(); // create console

    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);
    //freopen_s(&f, "CONOUT$", "w", stderr);
    //freopen_s(&f, "CONIN$", "r", stdin);

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    Frequency = freq.QuadPart;
    MSG msg;
    Buffer buffer;
    std::vector<Object> scene;

    //scene = loadObj("Untitled.obj");

    PrimitiveInfo info;
    info.x = 16;
    info.y = 6;
    info.size = {1.0f, 1.0f, 1.0f};

    Mesh sphere = createSphere(info);
    Mesh sphere2 = createSphere(info);

    Object obj1(Vec3(0.f, 0.f, -5.f), Vec3(0.f, 0.f, 0.f), Vec3(1.f, 1.f, 1.f), sphere);
    Object obj2(Vec3(3.f, 0.f, -5.f), Vec3(0.f, 0.f, 0.f), Vec3(1.f, 1.f, 1.f), sphere2);
    scene.push_back(obj1);
    scene.push_back(obj2);

    Window window(L"Renderer", hInstance, nCmdShow, &buffer);
    float* zBuffer = new float[buffer.width * buffer.height];
    clearZBuffer(zBuffer, buffer.width, buffer.height);
    bool sleepGranular = timeBeginPeriod(1);
    int framerate = 30;
    double secondsPerFrame = 1.0f / (float)framerate;
    double fps = 30.0;
    while (running) {
        window.showFPS(fps);
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

        graphicsPipeline(&buffer, zBuffer, scene);

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
        fps = 1.0 / getElapsed(start, end);
    }
}
