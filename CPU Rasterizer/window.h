#define NOMINMAX
#include "common.h"
#include "bitmaps.h"
#include "camera.h"
#include <stdexcept>

class Window {
private:
	const wchar_t*	name;
	HINSTANCE	hInstance;
	HWND		hWnd;
	Buffer 		*buffer;
	Camera      *camera;

	void moveForward() {
		if (camera) {
			camera->moveForward();
		}
	}

	void moveBackward() {
		if (camera) {
			camera->moveBackward();
		}
	}

	void moveLeft() {
		if (camera) {
			camera->moveLeft();
		}
	}

	void moveRight() {
		if (camera) {
			camera->moveRight();
		}
	}

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
				case WM_KEYDOWN: {
				    uint32_t key = wParam;
					switch (key) {
						case 0x57: {
						    // W
							// Move the camera forward
							window->moveForward();
						} break;
						case 0x41: {
						    // A
							// Move the camera left
							window->moveLeft();
						} break;
						case 0x53: {
						    // S
							// Move the camera backward
							window->moveBackward();
						} break;
						case 0x44: {
						    // D
							// Move the camera right
							window->moveRight();
						} break;
					}
				} break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

public:
	bool		shouldResize;
	uint32_t	width;
	uint32_t	height;
	CharacterBitMap fps[3];
	CharacterBitMap numbers[10];
	CharacterBitMap equal;

	Window(const wchar_t* name, HINSTANCE hInstance, int nCmdShow, Buffer *buffer, Camera* camera, uint32_t w = 512,
 uint32_t h = 512) : name(name), hInstance(hInstance), buffer(buffer), camera(camera) {
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

		generateFPSMask(this->fps);
		generate7SegmentMask();
		generateNumberMask(this->numbers);
		this->equal = generateEqualMask();

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
		ReleaseDC(hWnd, dc);
	}

	void applyMask(uint32_t* pixel, const CharacterBitMap& mask, int blockSize = 1) {
		for (int j = 0; j < 7 * blockSize; j++) {
		    for (int i = 0; i < 8; i++) {
				uint32_t color = 0x0000;
				if (mask.data[j / blockSize] & (1 << (7 - i))) {
    			    color = 0xFFFF;
    			} else {
        			color = 0x0000;
				}
				for (int k = 0; k < blockSize; k++) {
					pixel[(i * blockSize + j * buffer->width) + k] = color;
				}
			}
		}
	}

	void showFPS(double fps) {
	    int startX = width / 16;
	    int startY = height / 16;
		int unit = (int)fps % 10;
		int tens = ((int)fps / 10) % 10;

		CharacterBitMap unitChar = this->numbers[unit];
		CharacterBitMap tensChar = this->numbers[tens];

		uint32_t* pixel = (uint32_t*)buffer->memory + (startY * buffer->width
		+ startX);
		int blockSize = 2;
		for (int i = 0; i < 3; i++) {
            applyMask(pixel, this->fps[i], 2);
			pixel += (int)(8 * 1.5 * blockSize);
		}
		applyMask(pixel, this->equal, blockSize);
		pixel += (int)(8 * 1.5 * blockSize);
		applyMask(pixel, tensChar, blockSize);
		pixel += (int)(8 * 1.5 * blockSize);
		applyMask(pixel, unitChar, blockSize);
	}

};
