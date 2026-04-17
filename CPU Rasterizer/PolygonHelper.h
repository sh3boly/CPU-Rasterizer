#pragma once
#include "My_Math.h"
#include "common.h"
#include <algorithm>
#include <stdint.h>
#include <vector>
enum WindingOrder {
	CCW,
	CW,
	INVALID
};

class PolygonHelper {
	int getIndex(int i, int n) {
		return (i + n) % n;
	}
	bool isIntersecting(Vec2& a, Vec2& b, Vec2& c, Vec2& d) {
		Vec2 ab = b - a;
		Vec2 cd = d - c;
		Vec2 ac = c - a;
		Vec2 ad = d - a;
		if ((crossProduct(ab, ac) > 0 && crossProduct(ab, ad) < 0)) return true;
		if ((crossProduct(ab, ad) < 0 && crossProduct(ab, ad) > 0)) return true;
		if ((crossProduct(ab, ad) == 0 || crossProduct(ab, ad) == 0)) return true;
		return false;
	}
	bool isSimplePolygon(std::vector<Vec2> vertices) {
		// generate edges
		std::vector<std::pair<Vec2, Vec2>> edges;
		for (int i = 1; i < vertices.size(); i++) {
			edges.push_back(std::make_pair(vertices[i - 1], vertices[i]));
		}
		edges.push_back(std::make_pair(vertices[0], vertices[vertices.size() - 1]));

		// Loop over all edges
		// For each two edges except(i + 1, i -1)
		// check if they intersect
		for (int i = 0; i < edges.size(); i++) {
			for (int j = 0; j < edges.size(); j++) {
				if (j == i || j == getIndex(i + 1, edges.size()) || j == getIndex(i - 1, edges.size()))
					continue;
				if (isIntersecting(edges[i].first, edges[i].second, edges[j].first, edges[j].second)) {
					return false;
				}
			}
		}
		return true;
	}
	WindingOrder getOrientation(std::vector<Vec2> vertices) {
		float sum = 0;
		int n = vertices.size();
		for (int i = 0; i < n; i++) {
			sum += vertices[i].x * vertices[(i + 1) % n].y - vertices[i].y * vertices[(i + 1) % n].x;
		}
		if (sum < 0) return CW;
		return CCW;
	}

public:

	float findPolygonArea(std::vector<Vec2> vertices) {

	}
	std::vector<int> triangulate(std::vector<Vec2>& vertices, std::string errorMessage) {
		// Assuming CCW
		if (!isSimplePolygon(vertices)) {
			errorMessage = "This is not a simple polygon.\n";
			return {};
		}
		WindingOrder windingOrder = getOrientation(vertices);
		if (windingOrder == CW) {
			reverse(vertices.begin(), vertices.end());
		}
		std::vector<int> indexList;
		for (int i = 0; i < vertices.size(); i++) {
			indexList.push_back(i);
		}
		int totalTrianglesCount = vertices.size() - 2;
		int totalTriangleIndexCount = totalTrianglesCount * 3;

		std::vector<int> triangles(totalTriangleIndexCount);
		int triangleIndex = 0;

		while (indexList.size() > 3) {
			for (int i = 0; i < indexList.size(); i++) {
				int a = indexList[i];
				int b = indexList[getIndex(i - 1, indexList.size())];
				int c = indexList[getIndex(i + 1, indexList.size())];

				Vec2 vA = vertices[a];
				Vec2 vB = vertices[b];
				Vec2 vC = vertices[c];

				Vec2 vAB = vB - vA;
				Vec2 vAC = vA - vA;

				if (crossProduct(vAB, vAC) < 0.f) continue;
				bool isEar = true;
				for (int j = 0; j < vertices.size(); j++) {
					if (j == a || j == b || j == c) continue;



					Vec2 p = vertices[j];
					if (isPointInTriangle(p, vB, vA, vC)) {
						isEar = false;
						break;
					}


				}
				if (isEar) {
					triangles[triangleIndex++] = b;
					triangles[triangleIndex++] = a;
					triangles[triangleIndex++] = c;

					indexList.erase(indexList.begin() + i);
					break;
				}
			}
		}

		triangles[triangleIndex++] = indexList[0];
		triangles[triangleIndex++] = indexList[1];
		triangles[triangleIndex++] = indexList[2];

		return triangles;
	}
	bool isPointInTriangle(Vec2& p, Vec2& a, Vec2& b, Vec2& c) {
		Vec2 vAP = p - a;
		Vec2 vCP = p - b;
		Vec2 vBP = p - c;

		Vec2 vAB = b - a;
		Vec2 vBC = c - b;
		Vec2 vCA = a - c;

		return (crossProduct(vBP, vBC) >= 0.f && crossProduct(vCP, vCA) >= 0.f && crossProduct(vAP, vAB) >= 0.f);
	}
};

struct PrimitiveInfo {
    uint32_t x;
    uint32_t y;
    Vec3 size;
};

class Mesh {
public:
    std::vector<uint32_t> rawIndices;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Mesh() {}
};

void buildTriangles(Mesh& mesh) {
	mesh.triangles.clear();
	for (uint32_t i = 0; i < mesh.rawIndices.size(); i += 3) {
		Triangle tri;
		tri.vertices[0] = mesh.vertices[mesh.rawIndices[i]];
		tri.vertices[1] = mesh.vertices[mesh.rawIndices[i + 1]];
		tri.vertices[2] = mesh.vertices[mesh.rawIndices[i + 2]];
		mesh.triangles.push_back(tri);
	}

}

Mesh createSphere(const PrimitiveInfo& info) {
	Mesh mesh;

	const uint32_t thetaCount = std::max(info.y, (uint32_t)2);
	const uint32_t phiCount = std::max(info.x, (uint32_t)3);
	const float thetaStep = PI / thetaCount;
	const float phiStep = 2.f * PI / phiCount;
	const uint32_t numIndices = 2 * 3 * phiCount + 2 * 3 * phiCount * (thetaCount - 2);
	const uint32_t numVertices = 2 + phiCount * (thetaCount - 1);

	mesh.vertices.resize(numVertices);
	// Top vertex
	uint32_t c = 0;

	mesh.vertices[c++] = {
		{0.f, info.size.y, 0.0f},
		{1.f , 1.f, 1.f},
		normalize(Vec3(0.f, info.size.y, 0.0f))
	};

	for (uint32_t j = 1; j < thetaCount; j++) {
	    const float theta = j * thetaStep;
		for (uint32_t i = 0; i < phiCount; i++) {
		    const float phi = i * phiStep;
			Vec3 p = {
				info.size.x * std::sin(theta) * std::cos(phi),
				info.size.y * std::cos(theta),
				info.size.z * std::sin(theta) * std::sin(phi)
			};

			Vec3 n = normalize(p);
			Vec3 color = Vec3(1.f, 1.f, 1.f);
			mesh.vertices[c++] = { p, color, n };

		}
	}
	// Bottom vertex
	mesh.vertices[c++] = {
		{0.f, -info.size.y, 0.0f},
		{1.f, 1.f, 1.f},
		normalize({0.f, -info.size.y, 0.f})
	};

	// Triangles construction
	mesh.rawIndices.resize(numIndices);
	c = 0;

	for (uint32_t i = 0; i < phiCount - 1; i++) {
	    mesh.rawIndices[c++] = 0;
		mesh.rawIndices[c++] = i + 1;
		mesh.rawIndices[c++] = i + 2;
	}


	mesh.rawIndices[c++] = 0;
	mesh.rawIndices[c++] = phiCount;
	mesh.rawIndices[c++] = 1;

	for (uint32_t j = 0; j < thetaCount - 2; j++) {

		for (uint32_t i = 0; i < phiCount - 1; i++) {
		    const uint32_t indices[4] = {
					1 + i + j * phiCount,
					1 + i + (j + 1) * phiCount,
					1 + (i + 1) + (j + 1) * phiCount,
					1 + (i + 1) + (j) * phiCount
			};

			mesh.rawIndices[c++] = indices[0];
			mesh.rawIndices[c++] = indices[1];
			mesh.rawIndices[c++] = indices[2];

			mesh.rawIndices[c++] = indices[0];
			mesh.rawIndices[c++] = indices[2];
			mesh.rawIndices[c++] = indices[3];
		}

		const uint32_t indices[4] = {
			(phiCount) + j * phiCount,
			(phiCount) + (j + 1) * phiCount,
			1 + (j + 1) * phiCount,
			1 + j * phiCount
		};

		mesh.rawIndices[c++] = indices[0];
		mesh.rawIndices[c++] = indices[1];
		mesh.rawIndices[c++] = indices[2];

		mesh.rawIndices[c++] = indices[0];
		mesh.rawIndices[c++] = indices[2];
		mesh.rawIndices[c++] = indices[3];
	}

	const uint32_t southPoleIndex = (uint32_t)mesh.vertices.size() - 1;
	for (uint32_t i = 0; i < phiCount - 1; i++) {
		mesh.rawIndices[c++] = southPoleIndex;
		mesh.rawIndices[c++] = southPoleIndex - phiCount + i + 1;
		mesh.rawIndices[c++] = southPoleIndex - phiCount + i;
	}

	mesh.rawIndices[c++] = southPoleIndex;
	mesh.rawIndices[c++] = southPoleIndex - phiCount;
	mesh.rawIndices[c++] = southPoleIndex - 1;

	buildTriangles(mesh);

	return mesh;
}
