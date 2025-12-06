#pragma once
#include "My_Math.h"
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
