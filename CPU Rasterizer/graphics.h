#pragma once
#include "My_Math.h"
#include "world.h"
#include "camera.h"
#include <fstream>
#include <sstream>
#include <iostream>

struct Buffer {
    BITMAPINFO	info;
    void* memory;
    uint32_t 		width;
    uint32_t		height;
    int 		pitch;
};

inline float edgeFunction(const Vec2& a, const Vec2& b, const Vec2& c)
{
    return crossProduct(b - a, c - a);
}
inline bool topLeft(const float dy, const float dx) {
    return (dy > 0) || (dy == 0 && dx < 0);
}

float minX = INFINITY, maxX = -INFINITY;
float minY = INFINITY, maxY = -INFINITY;
float minZ = INFINITY, maxZ = -INFINITY;
float size = 1;
std::vector<Triangle> loadObj(std::string filename) {
    std::vector<Vec3> t_positions;
    std::vector<Vec3> temp_positions;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    std::vector<Triangle> mesh;

    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
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
            mesh.push_back({ v0, v1, v2 });

        }
        else {
            continue;
        }
    }
    size = std::max(maxX - minX, std::max(maxY - minY, maxZ - minZ));
    return mesh;
}

void rasterizeTriangle(Triangle& t1, Buffer* buffer, float* zbuffer)
{
    Vertex& v0 = t1.vertices[0];
    Vertex& v1 = t1.vertices[1];
    Vertex& v2 = t1.vertices[2];

    /*float gray0 = (v0.coordinates.z - minZ) / (maxZ - minZ);
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
    v2.color.z = gray2;*/
    float area = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, (Vec2)v2.coordinates);
    if (area < 0)
        return;

    // calculate bounding boxes
    float minX = std::min(v0.coordinates.x, std::min(v1.coordinates.x, v2.coordinates.x));
    float minY = std::min(v0.coordinates.y, std::min(v1.coordinates.y, v2.coordinates.y));
    float maxX = std::max(v0.coordinates.x, std::max(v1.coordinates.x, v2.coordinates.x));
    float maxY = std::max(v0.coordinates.y, std::max(v1.coordinates.y, v2.coordinates.y));

    int x0 = std::max(0, (int)std::floor(minX));
    int y0 = std::max(0, (int)std::floor(minY));
    int x1 = std::min((int)buffer->width - 1, (int)std::floor(maxX));
    int y1 = std::min((int)buffer->height - 1, (int)std::floor(maxY));
    if (y0 < 0 || y1 < 0 || x0 < 0 || x1 < 0) return;
    Vec2 p = { x0 + 0.5f, y0 + 0.5f };
    // Barycentric Coordinates
    float initialW0 = edgeFunction((Vec2)v1.coordinates, (Vec2)v2.coordinates, p);
    float initialW1 = edgeFunction((Vec2)v2.coordinates, (Vec2)v0.coordinates, p);
    float initialW2 = edgeFunction((Vec2)v0.coordinates, (Vec2)v1.coordinates, p);

    float deltaX0 = v2.coordinates.x - v1.coordinates.x;
    float deltaX1 = v0.coordinates.x - v2.coordinates.x;
    float deltaX2 = v1.coordinates.x - v0.coordinates.x;

    float deltaY0 = v2.coordinates.y - v1.coordinates.y;
    float deltaY1 = v0.coordinates.y - v2.coordinates.y;
    float deltaY2 = v1.coordinates.y - v0.coordinates.y;

    bool topLeft0 = topLeft(deltaY0, deltaX0);
    bool topLeft1 = topLeft(deltaY1, deltaX1);
    bool topLeft2 = topLeft(deltaY2, deltaX2);

    float divArea = 1.0f / area;
    float w0Row = initialW0;
    float w1Row = initialW1;
    float w2Row = initialW2;
    uint32_t* pixel = (uint32_t*)buffer->memory;
    for (uint32_t j = y0; j <= y1; j++)
    {
        float w0 = w0Row;
        float w1 = w1Row;
        float w2 = w2Row;
        for (uint32_t i = x0; i <= x1; i++)
        {

            if ((w0 > 0 || (w0 == 0 && topLeft0))
                && (w1 > 0 || (w1 == 0 && topLeft1))
                && (w2 > 0 || (w2 == 0 && topLeft2))
                )
            {

                float normalizedW0 = std::max(0.f, w0 * divArea);
                float normalizedW1 = std::max(0.f, w1 * divArea);
                float normalizedW2 = std::max(0.f, w2 * divArea);
                float z = normalizedW0 * v0.coordinates.z + normalizedW1 * v1.coordinates.z + normalizedW2 * v2.coordinates.z;
                if (z < zbuffer[j * buffer->width + i])
                {
                    Vec3& c0 = v0.color;
                    Vec3& c1 = v1.color;
                    Vec3& c2 = v2.color;

                    float r = normalizedW0 * c0.x + normalizedW1 * c1.x + normalizedW2 * c2.x;
                    float g = normalizedW0 * c0.y + normalizedW1 * c1.y + normalizedW2 * c2.y;
                    float b = normalizedW0 * c0.z + normalizedW1 * c1.z + normalizedW2 * c2.z;

                    uint32_t rInt = (uint32_t)(std::min(std::max(r, 0.f), 1.f) * 255);
                    uint32_t gInt = (uint32_t)(std::min(std::max(g, 0.f), 1.f) * 255);
                    uint32_t bInt = (uint32_t)(std::min(std::max(b, 0.f), 1.f) * 255);

                    zbuffer[j * buffer->width + i] = z;
                    uint32_t* curr = pixel + (j * buffer->width + i);
                    *curr = (rInt << 16) | (gInt << 8) | (bInt);
                }
            }
            w0 -= deltaY0;
            w1 -= deltaY1;
            w2 -= deltaY2;
        }
        w0Row += deltaX0;
        w1Row += deltaX1;
        w2Row += deltaX2;
    }
}

//void outputPPM()
//{
//    std::ofstream ofs;
//    ofs.open("./raster2d.ppm", std::ios::binary);
//    ofs << "P6\n"
//        << w << " " << h << "\n255\n";
//    ofs.write((char *)framebuffer, w * h * 3);
//
//    ofs.close();
//}

void project(Triangle& tri, const Mat4& mvp, uint32_t w, uint32_t h)
{
    for (int i = 0; i < 3; i++) {
        /*tri.vertices[i].coordinates.x /= size;
        tri.vertices[i].coordinates.y /= size;
        tri.vertices[i].coordinates.z /= size;*/

        tri.vertices[i].coordinates = mvp * tri.vertices[i].coordinates;


        tri.vertices[i].coordinates.x = (tri.vertices[i].coordinates.x + 1) * (w / 2.0f);
        tri.vertices[i].coordinates.y = (tri.vertices[i].coordinates.y + 1) * (h / 2.0f);
    }

}

static void graphicsPipeline(Buffer* buffer, float* zBuffer, std::vector<Object> scene, Camera& camera)
{
    Mat4 view = camera.lookAt(Vec3(0.f, 0.f, 0.f));
    Mat4 projectionMatrix = makePerspectiveMatrix(90, (float)buffer->width / buffer->height, 0.1f, 100.0f);

    Mat4 mvp = view;
    mvp = projectionMatrix * mvp;

    for (Object& object : scene)
    {
        object.transform();
        for (Triangle& tri : object.mesh.triangles) {
            Triangle projected = tri;
            project(projected, mvp, buffer->width, buffer->height);
            rasterizeTriangle(projected, buffer, zBuffer);
        }
    }
}
