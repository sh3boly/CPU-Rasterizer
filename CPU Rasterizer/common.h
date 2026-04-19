#pragma once
#define NOMINMAX
#include <windows.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "world.h"
#include "PolygonHelper.h"
#include "graphics.h"

#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// CONSTANTS

// GLOBALS
static int64_t Frequency;
static bool running = true;


// Structs


// Global Functions

static void resizeDibSection(Buffer* buf, uint32_t width, uint32_t height) {
    if (buf->memory) {
        VirtualFree(buf->memory, 0, MEM_RELEASE);
    }

    buf->width = width;
    buf->height = height;
    buf->info = {};

    buf->info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    buf->info.bmiHeader.biWidth = width;
    buf->info.bmiHeader.biHeight = -(int)height;
    buf->info.bmiHeader.biPlanes = 1;
    buf->info.bmiHeader.biBitCount = 32;
    buf->info.bmiHeader.biCompression = BI_RGB;

    int bytesPerPixel = 4;
    buf->pitch = width * bytesPerPixel;

    buf->memory = VirtualAlloc(
        0,
        width * height * bytesPerPixel,
        MEM_COMMIT,
        PAGE_READWRITE
    );

}
static void clearZBuffer(float*& zBuffer, int width, int height) {
    for (size_t i = 0; i < width * height; i++) {
        zBuffer[i] = INFINITY;
    }
}

static void resizeZBuffer(float*& zBuffer, uint32_t width, uint32_t height) {
    if (zBuffer) {
        delete[] zBuffer;
    }

    zBuffer = new float[width * height];
}

// Timers

static inline int64_t getTicks() {
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks)) {
        throw std::runtime_error("Failed get ticks from QueryPerformanceCounter");
    }
    return ticks.QuadPart;
}

static inline float getElapsed(int64_t start, int64_t end) {
    return (float)(end - start) / (float)Frequency;
}
