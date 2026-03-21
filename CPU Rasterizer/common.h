#pragma once
#include <windows.h>

struct Buffer {
	BITMAPINFO	info;
	void*		memory;
	int 		width;
	int		height;
	int 		pitch;
};

