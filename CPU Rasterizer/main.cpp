#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "My_Math.h"
#include "PolygonHelper.h"
struct Vertex {
    Vec3 coordinates;
    Vec3 color;
};
struct Rgb {
    unsigned char r, g, b;
};
struct Triangle {
    Vertex vertices[3];

};
struct Polygon {
    Triangle triangles[2];
};
inline float edgeFunction(const Vec2 &a, const Vec2 &b, const Vec2 &c) {
    return crossProduct(b - a, c - a);
}
#include <iostream>
const uint32_t w = 512;
const uint32_t h = 512;
Rgb* framebuffer = new Rgb[w * h];
float* zbuffer = new float[w * h];
void rasterizeTriangle(Triangle& t1) {
    Vertex& v0 = t1.vertices[0];
    Vertex& v1 = t1.vertices[1];
    Vertex& v2 = t1.vertices[2];

    float area = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, (Vec2)v2.coordinates);
    if (area < 0) {
        std::swap(v1, v2);
        area = -area;
    }
    for (uint32_t j = 0; j < h; j++) {
        for (uint32_t i = 0; i < w; i++) {
            Vec2 p = { i + 0.5f, j + 0.5f};

            // Barycentric Coordinates and Colors
            float w0 = edgeFunction((Vec2)v1.coordinates, (Vec2)v2.coordinates, p);
            float w1 = edgeFunction((Vec2)v2.coordinates, (Vec2)v0.coordinates, p);
            float w2 = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, p);


            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {

                w0 = std::max(0.f, w0 / area);
                w1 = std::max(0.f, w1 / area);
                w2 = std::max(0.f, w2 / area);
                Vec3& c0 = v0.color;
                Vec3& c1 = v1.color;
                Vec3& c2 = v2.color;

                float r = w0 * c0.x + w1 * c1.x + w2 * c2.x;
                float g = w0 * c0.y + w1 * c1.y + w2 * c2.y;
                float b = w0 * c0.z + w1 * c1.z + w2 * c2.z;

                float z = w0 * v0.coordinates.z + w1 * v1.coordinates.z + w2 * v2.coordinates.z;
                z = -z;
                if (z < zbuffer[j * w + i]) {
                    zbuffer[j * w + i] = z;
                    framebuffer[j * w + i].r = (unsigned char)(std::min(std::max(r, 0.f), 1.f) * 255);
                    framebuffer[j * w + i].g = (unsigned char)(std::min(std::max(g, 0.f), 1.f) * 255);
                    framebuffer[j * w + i].b = (unsigned char)(std::min(std::max(b, 0.f), 1.f) * 255);
                }
            }
        }
    }
}

void tempRasterizer() {
    Vertex v0, v1, v2, v3, v4, v5;
    v0.coordinates = { 0.0, 0.0, 20 };
    v1.coordinates = { 512.0, 512.0, 20 };
    v2.coordinates = { 0, 512.0, 20 };
    v0.color = { 1.0, 0.0, 0.0 };
    v1.color = { 1.0, 0.0, 0.0 };
    v2.color = { 1.0, 0.0, 0.0 };
    v3.coordinates = { 0.0, 512.0, 10 };
    v4.coordinates = { 512.0, 512.0, 10 };
    v5.coordinates = { 512.0, 0.0, 10 };
    v3.color = { 0.0, 0.0, 1.0 };
    v4.color = { 0.0, 0.0, 1.0 };
    v5.color = { 0.0, 0.0, 1.0 };
    Triangle t1;
    t1.vertices[0] = v0;
    t1.vertices[1] = v1;
    t1.vertices[2] = v2;
    Triangle t2;
    t2.vertices[0] = v3;
    t2.vertices[1] = v4;
    t2.vertices[2] = v5;
    
    memset(framebuffer, 0x0, w * h * sizeof(Rgb));
    memset(zbuffer, 1.0, w * h * sizeof(float));
    rasterizeTriangle(t1);
    rasterizeTriangle(t2);

    

    delete[] framebuffer;
}
void outputPPM() {
    std::ofstream ofs;
    ofs.open("./raster2d.ppm", std::ios::binary);
    ofs << "P6\n" << w << " " << h << "\n255\n";
    ofs.write((char*)framebuffer, w * h * 3);

    ofs.close();
}
void testTriangulate() {
    memset(framebuffer, 0x0, w * h * sizeof(Rgb));
    memset(zbuffer, 1.0, w * h * sizeof(float));
    std::vector<Vec2> vertices(5);
    vertices[4] = Vec2(200, 100);
    vertices[3] = Vec2(0, 0);
    vertices[2] = Vec2(50, 70);
    vertices[1] = Vec2(50, 130);
    vertices[0] = Vec2(0, 200);
    PolygonHelper h;
    std::string errorMessage;
    std::vector<int> triangles = h.triangulate(vertices, errorMessage);
    std::vector<Triangle> tris(3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            tris[i].vertices[j].coordinates = (Vec3)(vertices[triangles[i * 3 + j]]);
            tris[i].vertices[j].color = Vec3(static_cast<float>(rand()) / RAND_MAX
                ,static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX);
        }
    }
    for (auto tri : tris) {
        rasterizeTriangle(tri);
    }
    outputPPM();
    delete[] framebuffer;
}
int main() {
    testTriangulate();
}