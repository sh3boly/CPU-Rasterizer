#define NOMINMAX
#include "common.h"
#include <stdexcept>


class Window {
private:
	const wchar_t*	name;
	HINSTANCE	hInstance;
	HWND		hWnd;
	Buffer 		*buffer;
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		Window* window;

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			window = (Window*)cs->lpCreateParams;

			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		} else {

			window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (window) {
			switch (uMsg)
				{
				case WM_PAINT: {
				    PAINTSTRUCT ps;
				    HDC dc = BeginPaint(hwnd, &ps);

				    StretchDIBits(
					dc,
					0, 0, window->buffer->width, window->buffer->height,
					0, 0, window->buffer->width, window->buffer->height,
					window->buffer->memory,
					&window->buffer->info,
					DIB_RGB_COLORS,
					SRCCOPY
				    );

				    EndPaint(hwnd, &ps);
				} break;
				case WM_CLOSE: {
					DestroyWindow(window->hWnd);
				} break;
				case WM_DESTROY: {
					PostQuitMessage(0);
					return 0;

				} break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

public:
	Window(const wchar_t* name, HINSTANCE hInstance, int nCmdShow, Buffer *buffer) : name(name), hInstance(hInstance), buffer(buffer) {
		WNDCLASS wc = {};

		wc.lpfnWndProc		= WindowProc;
		wc.hInstance 		= hInstance;
		wc.lpszClassName	= this->name;
		
		RegisterClass(&wc);


		this->hWnd = CreateWindowEx(
		    0,                              // Optional window styles.
		    this->name,                     // Window class
		    this->name,			    // Window text
		    WS_OVERLAPPEDWINDOW,            // Window style

		    // Size and position
		    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		    NULL,       // Parent window    
		    NULL,       // Menu
		    this->hInstance,  // Instance handle
		    this
		    );

		if (this->hWnd == NULL)
		{
			throw std::runtime_error("failed to create a window");
		}

		ShowWindow(this->hWnd, nCmdShow);
	}

	
};

