# CPU Software Rasterizer (WIP)
This is a personal learning project where I am building a 3D software rendering engine from scratch in C++. It is currently a work in progress and serves as a hands-on way to understand the core mathematics and the traditional graphics pipeline without relying on hardware-accelerated APIs like OpenGL or DirectX.

## What's Implemented So Far
- Custom Math Library: Built-in Vec2, Vec3, Vec4, and Mat4 classes, supporting vector operations and matrix transformations (Translation, Rotation, Scaling, Perspective Projection).

- 3D Mesh Loading: A custom .obj parser that reads vertices, texture coordinates, and normals.

- The Graphics Pipeline: Projects 3D world coordinates into 2D screen space using Model, View, and Projection (MVP) matrices.

- Triangle Rasterization: Fills triangles pixel-by-pixel using Barycentric Coordinates (via edge functions).

- Depth Testing (Z-Buffering): Ensures proper overlapping and occlusion of 3D objects in the scene.

- Depth Shading: Currently maps the Z-depth of vertices to a grayscale color to visually represent depth.

- Polygon Triangulation: Implemented an ear-clipping algorithm to decompose simple polygons into rasterizable triangles.

- Image Export: Renders directly to raw .ppm (Portable Pixmap) image files with zero external UI or dependencies.
