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
				case WM_SIZE: {
					uint32_t width = LOWORD(lParam);
					uint32_t height = HIWORD(lParam);
					window->resize(width, height);
				} break;
				case WM_CLOSE: {
					DestroyWindow(window->hWnd);
				} break;
				case WM_DESTROY: {
					PostQuitMessage(0);
					running = false;
					return 0;

				} break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

public:
	bool		shouldResize;
	uint32_t	width;
	uint32_t	height;

	Window(const wchar_t* name, HINSTANCE hInstance, int nCmdShow, Buffer *buffer, uint32_t w = 512, uint32_t h = 512) : name(name), hInstance(hInstance), buffer(buffer) {
		WNDCLASS wc = {};

		wc.lpfnWndProc		= WindowProc;
		wc.hInstance 		= hInstance;
		wc.lpszClassName	= this->name;
		
		RegisterClass(&wc);

		this->width = w;
		this->height = h;
		this->shouldResize = false;

		this->hWnd = CreateWindowEx(
		    0,                              // Optional window styles.
		    this->name,                     // Window class
		    this->name,			    // Window text
		    WS_OVERLAPPEDWINDOW,            // Window style

			CW_USEDEFAULT, // X
			CW_USEDEFAULT, // Y
			w,				// Width
			h,				// Height

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
	
	void resize(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		resizeDibSection(this->buffer, this->width, this->height);
		shouldResize = true;
	}

	void display() {
		HDC dc = GetDC(this->hWnd);

		StretchDIBits(
			dc,
			0, 0, this->buffer->width, this->buffer->height,
			0, 0, this->buffer->width, this->buffer->height,
			this->buffer->memory,
			&this->buffer->info,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}

	void showFPS(double fps) {
		HDC hdc = GetDC(hWnd);
		WCHAR buffer[64];
		swprintf_s(buffer, L"FPS: %.2f", fps);;
		RECT rect;

		rect.left = 10;
		rect.top = 10;
		rect.right = 200;
		rect.bottom = 50;

		DrawText(
			hdc,
			buffer,
			-1,
			&rect,
			DT_CENTER
		);

		ReleaseDC(hWnd, hdc);
	}
	
};

