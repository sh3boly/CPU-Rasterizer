#include "window.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{

    MSG msg;
    Buffer buffer; 
    std::vector<Triangle> mesh = loadObj("Untitled.obj");
    Window window(L"Renderer", hInstance, nCmdShow, &buffer);
    float* zBuffer = new float[buffer.width * buffer.height];
    clearZBuffer(zBuffer, buffer.width, buffer.height);

    while(running) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        if (window.shouldResize) {
            resizeZBuffer(zBuffer, window.width, window.height);
            window.shouldResize = false;
        }
        graphicsPipeline(&buffer, zBuffer, mesh);

        window.display();

    }
}
