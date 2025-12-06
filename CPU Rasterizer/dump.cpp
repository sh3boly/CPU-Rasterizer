
//
//bool pixelContainedIn2DTriangle(float v0, float v1, float v2, int x, int y, float z);
//float edgeFunction(std::vector<float>& a, std::vector<float>& b, std::vector<float>& c);
//void rasterize(int imageWidth, int imageHeight) {
//	std::vector<std::vector<float>> z_buffer(imageWidth, std::vector<float>(imageHeight, INFINITY));
//	std::vector<std::vector<float>> image(imageWidth, std::vector<float>(imageHeight));
//
//	for (auto triangle : scene) {
//		// Project Vertices 
//		float nearClippingPlane = 0.1f;
//		std::vector<float> pCamera(3);
//		worldToCamera.multVecMatrix(pWorld, pCamera);
//		std::vector<float> pScreen(2);
//		pScreen[0] = nearClippingPlane * pCamera[0] / -pCamera[2];
//		pScreen[1] = nearClippingPlane * pCamera[1] / -pCamera[2];
//
//		// from Screen Space to -1, 1 NDC Space;
//		
//		std::vector<float> pNDC(2);
//		pNDC[0] = 2 * pScreen[0] / (r - l) - (r + l) / (r - l);
//		pNDC[1] = 2 * pScreen[1] / (t - b) - (t + b) / (t - b);
//
//		// convert from NDC to Raster Space
//
//		std::vector<float> pRaster(3);
//		pRaster[0] = (pNDC[0] + 1) / 2 * imageWidth;
//		pRaster[1] = (1 - pNDC[1]) / 2 * imageHeight;
//
//		pRaster[2] = -pCamera[2];
//
//		flaot area = edgeFunction(triangle[], triangle[1], triangle[2]);
//		float w0 = edgeFunction(triangle[1], triangle[2], p);
//		float w1 = edgeFunction(triangle[2], triangle[0], p);
//		float w2 = edgeFunction(triangle[0], triangle[1], p);
//
//		if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
//			w0 /= area;
//			w1 /= area;
//			w2 /= area;
//		}
//
//		// Compute Bounding Boxes of the tris
//		std::vector<float> bbmin = { INFINITY, INFINITY };
//		std::vector<float> bbmax = { -INFINITY, -INFINITY };
//		std::vector<std::vector<float>> triangleProjection(3, std::vector<float>(3));
//		for (int i = 0; i < 3; i++) {
//			triangleProjection[i] = projectAndConvertToNDC(triangle[i]);
//			triangleProjection[i][0] *= imageWidth;
//			triangleProjection[i][1] *= imageHeight;
//
//			bbmin[0] = std::min(triangleProjection[i][0], bbmin[0]);
//			bbmin[1] = std::min(triangleProjection[i][1], bbmin[1]);
//			bbmax[0] = std::min(triangleProjection[i][0], bbmax[0]);
//			bbmax[1] = std::min(triangleProjection[i][1], bbmax[1]);
//
//		}
//		uint16_t xmin = std::max(0, std::min(imageWidth - 1, (int)std::floor(bbmin[0])));
//		uint16_t ymin = std::max(0, std::min(imageHeight - 1, (int)std::floor(bbmin[1])));
//		uint16_t xmax = std::min(imageWidth - 1, (int)std::floor(bbmax[0]));
//		uint16_t ymax = std::min(imageHeight - 1, (int)std::floor(bbmax[1]));
//
//		for (uint16_t y = ymin; y <= ymax; y++) {
//			for (uint16_t x = xmin; x <= xmax; x++) {
//
//				float z;
//				if (pixelContainedIn2DTriangle(v0, v1, v2, x, y, &z)) {
//					if (z < z_buffer[x][y]) {
//						z_buffer[x][y] = z;
//						image[x][y] = triangle[i].color;
//					}
//				}
//
//			}
//		}
//	}
//}