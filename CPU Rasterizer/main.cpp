#include <vector>
#include <cstdio>
#include <cstdlib>
#include "My_Math.h"
#include "PolygonHelper.h"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
struct Vertex
{
    Vec3 coordinates;
    Vec3 color;
    Vec3 normal;
};
struct Rgb
{
    unsigned char r, g, b;
};
struct Triangle
{
    Vertex vertices[3];
};
struct Polygon
{
    Triangle triangles[2];
};
inline float edgeFunction(const Vec2 &a, const Vec2 &b, const Vec2 &c)
{
    return crossProduct(b - a, c - a);
}
const uint32_t w = 512;
const uint32_t h = 512;
Rgb *framebuffer = new Rgb[w * h];
float *zbuffer = new float[w * h];
float minX = INFINITY, maxX = -INFINITY;
float minY = INFINITY, maxY = -INFINITY;
float minZ = INFINITY, maxZ = -INFINITY;
float size;
std::vector<Triangle> loadObj(std::string filename) {
    std::vector<Vec3> t_positions;
    std::vector<Vec3> temp_positions;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    std::vector<Triangle> mesh;

    std::ifstream file(filename);
    std::string line;
    while(std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            minX = std::min(pos.x, minX);
            minY = std::min(pos.y, minY);
            minZ = std::min(pos.z, minZ);
            maxX = std::max(pos.x, maxX);
            maxY = std::max(pos.y, maxY);
            maxZ = std::max(pos.z, maxZ);


            temp_positions.push_back(pos);
        }
        else if (prefix == "vt") {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        } 
        else if (prefix == "vn") {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        } 
        else if (prefix == "f") {
            // Here you parse the "v/vt/vn v/vt/vn v/vt/vn" string
            // Remember to subtract 1 from the indices!
            // Assemble 3 Vertices, create a Triangle, and add it to 'mesh'
            int v[3], t[3], n[3];
            for (int i = 0; i < 3; i++) {
                std::string vertex_chunk;
                ss >> vertex_chunk;

                std::stringstream chunk_ss(vertex_chunk);
                std::string v_str, t_str, n_str;

                std::getline(chunk_ss, v_str, '/');
                std::getline(chunk_ss, t_str, '/');
                std::getline(chunk_ss, n_str, '/');

                // Convert strings to ints (subtract 1 for 0-based indexing)
                // Position is always guaranteed to be there
                v[i] = std::stoi(v_str) - 1;

                // Only convert UVs and Normals if the string isn't empty
                t[i] = t_str.empty() ? -1 : std::stoi(t_str) - 1;
                n[i] = n_str.empty() ? -1 : std::stoi(n_str) - 1;
            }
            
            Vertex v0, v1, v2;
            v0.coordinates = temp_positions[v[0]];
            v1.coordinates = temp_positions[v[1]];
            v2.coordinates = temp_positions[v[2]];
            if (n[0] != -1) {
                v0.normal = temp_normals[n[0]];
            }
            if (n[1] != -1) {
                v1.normal = temp_normals[n[1]];
            }
            if (n[2] != -1) {
                v2.normal = temp_normals[n[2]];
            }
            mesh.push_back({v0, v1, v2});
            
        }
        else {
            continue;
        }
    }
    size = std::max(maxX - minX, std::max(maxY - minY, maxZ - minY));
    return mesh;
}

void rasterizeTriangle(Triangle &t1)
{
    Vertex &v0 = t1.vertices[0];
    Vertex &v1 = t1.vertices[1];
    Vertex &v2 = t1.vertices[2];
    float gray0 = (v0.coordinates.z - minZ) / (maxZ - minZ);
    float gray1 = (v1.coordinates.z - minZ) / (maxZ - minZ);
    float gray2 = (v2.coordinates.z - minZ) / (maxZ - minZ);

    v0.color.x = gray0;
    v0.color.y = gray0;
    v0.color.z = gray0;

    v1.color.x = gray1;
    v1.color.y = gray1;
    v1.color.z = gray1;

    v2.color.x = gray2;
    v2.color.y = gray2;
    v2.color.z = gray2;
    float area = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, (Vec2)v2.coordinates);
    if (area < 0)
        return;

    // calculate bounding boxes
    std::vector<uint32_t> bbmin = {w, h};
    std::vector<uint32_t> bbmax = {0, 0};
    bbmin[0] = std::min(v0.coordinates.x, std::min(v1.coordinates.x, v2.coordinates.x));
    bbmin[1] = std::min(v0.coordinates.y, std::min(v1.coordinates.y, v2.coordinates.y));

    bbmax[0] = std::max(v0.coordinates.x, std::max(v1.coordinates.x, v2.coordinates.x));
    bbmax[1] = std::max(v0.coordinates.y, std::max(v1.coordinates.y, v2.coordinates.y));
    bbmax[0] = std::min(w - 1, bbmax[0]);
    bbmax[1] = std::min(h - 1, bbmax[1]);
    bbmin[0] = std::min(w - 1, bbmin[0]);
    bbmin[1] = std::min(h - 1, bbmin[1]);
    Vec2 p = {bbmin[0] + 0.5f, bbmin[1] + 0.5f};

    // Barycentric Coordinates
    float w0 = edgeFunction((Vec2)v1.coordinates, (Vec2)v2.coordinates, p);
    float w1 = edgeFunction((Vec2)v2.coordinates, (Vec2)v0.coordinates, p);
    float w2 = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, p);

    float deltaX0 = v2.coordinates.x - v1.coordinates.x;
    float deltaX1 = v0.coordinates.x - v2.coordinates.x;
    float deltaX2 = v1.coordinates.x - v0.coordinates.x;

    float deltaY0 = v2.coordinates.y - v1.coordinates.y;
    float deltaY1 = v0.coordinates.y - v2.coordinates.y;
    float deltaY2 = v1.coordinates.y - v0.coordinates.y;
    float divArea = 1.0f / area;
    for (uint32_t j = bbmin[1] + 1; j < bbmax[1] + 1; j++)
    {

        w0 += deltaX0;
        w1 += deltaX1;
        w2 += deltaX2;
        float tempW0 = w0;
        float tempW1 = w1;
        float tempW2 = w2;
        for (uint32_t i = bbmin[0] + 1; i < bbmax[0] + 1; i++)
        {
            w0 -= deltaY0;
            w1 -= deltaY1;
            w2 -= deltaY2;

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {

                float normalizedW0 = std::max(0.f, w0 * divArea);
                float normalizedW1 = std::max(0.f, w1 * divArea);
                float normalizedW2 = std::max(0.f, w2 * divArea);
                float z = normalizedW0 * v0.coordinates.z + normalizedW1 * v1.coordinates.z + normalizedW2 * v2.coordinates.z;
                if (z >= zbuffer[j * w + i])
                    continue;
                Vec3 &c0 = v0.color;
                Vec3 &c1 = v1.color;
                Vec3 &c2 = v2.color;

                float r = normalizedW0 * c0.x + normalizedW1 * c1.x + normalizedW2 * c2.x;
                float g = normalizedW0 * c0.y + normalizedW1 * c1.y + normalizedW2 * c2.y;
                float b = normalizedW0 * c0.z + normalizedW1 * c1.z + normalizedW2 * c2.z;

                zbuffer[j * w + i] = z;
                framebuffer[j * w + i].r = (unsigned char)(std::min(std::max(r, 0.f), 1.f) * 255);
                framebuffer[j * w + i].g = (unsigned char)(std::min(std::max(g, 0.f), 1.f) * 255);
                framebuffer[j * w + i].b = (unsigned char)(std::min(std::max(b, 0.f), 1.f) * 255);
            }
        }
        w0 = tempW0;
        w1 = tempW1;
        w2 = tempW2;
    }
}

void tempRasterizer()
{
    Vertex v0, v1, v2, v3, v4, v5;
    v0.coordinates = {0.0, 0.0, 20};
    v1.coordinates = {512.0, 512.0, 20};
    v2.coordinates = {0, 512.0, 20};
    v0.color = {1.0, 0.0, 0.0};
    v1.color = {1.0, 0.0, 0.0};
    v2.color = {1.0, 0.0, 0.0};
    v3.coordinates = {0.0, 512.0, 10};
    v4.coordinates = {512.0, 512.0, 10};
    v5.coordinates = {512.0, 0.0, 10};
    v3.color = {0.0, 0.0, 1.0};
    v4.color = {0.0, 0.0, 1.0};
    v5.color = {0.0, 0.0, 1.0};
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
void outputPPM()
{
    std::ofstream ofs;
    ofs.open("./raster2d.ppm", std::ios::binary);
    ofs << "P6\n"
        << w << " " << h << "\n255\n";
    ofs.write((char *)framebuffer, w * h * 3);

    ofs.close();
}
void project(Triangle &tri, const Mat4 &mvp)
{
    for (int i = 0; i < 3; i++) {
        tri.vertices[i].coordinates.x /= size;
        tri.vertices[i].coordinates.y /= size;
        tri.vertices[i].coordinates.z /= size;

        tri.vertices[i].coordinates = mvp * tri.vertices[i].coordinates;


        tri.vertices[i].coordinates.x = (tri.vertices[i].coordinates.x + 1) * (w / 2.0f);
        tri.vertices[i].coordinates.y = (tri.vertices[i].coordinates.y + 1) * (h / 2.0f);
    }

}
void testTriangulate()
{
    memset(framebuffer, 0x0, w * h * sizeof(Rgb));
    memset(zbuffer, 1.0, w * h * sizeof(float));
    std::vector<Triangle> mesh = loadObj("Untitled.obj");
    

    Mat4 model = makeTranslationMatrix(Vec3(0.0f, 0.0f, -1.0f));
    Mat4 view = Mat4::identity();
    Mat4 projectionMatrix = makePerspectiveMatrix(90, (float)w/h, 0.1f, 100.0f);

    Mat4 mvp = view * model;
    mvp = projectionMatrix * mvp;

    for (auto& tri : mesh)
    {
        project(tri, mvp);
    }

    for (auto& tri : mesh)
    {
        rasterizeTriangle(tri);
    }

    outputPPM();
}

int main()
{
    //std::chrono::duration<double, std::milli> min_duration = std::chrono::duration<double, std::milli>::max();
    //for (int i = 0; i < 99; i++)
    //{ 
    //    auto start = std::chrono::high_resolution_clock::now();

    //    testTriangulate();

    //    auto end = std::chrono::high_resolution_clock::now();
    //    std::chrono::duration<double, std::milli> current_duration = end - start;

    //    // Update min_duration if the current run was faster
    //    min_duration = std::min(min_duration, current_duration);
    //}
    //std::cout << "Fastest execution time: " << min_duration.count() << " ms\n";
    //delete[] framebuffer;
    //delete[] zbuffer;
    testTriangulate();
}
