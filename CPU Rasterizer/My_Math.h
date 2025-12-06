#include <iostream>
#pragma once
class Vec3;
class Vec2
{
public:
	float x, y;
	Vec2(float x, float y) {
		this->x = x;
		this->y = y;
	}
	Vec2() {
		this->x = 0.f;
		this->y = 0.f;
	}
	Vec2 operator+(const Vec2& v) const {
		Vec2 t;
		t.x = this->x + v.x;
		t.y = this->y + v.y;
		return t;
	}
	Vec2 operator-(const Vec2& v) const {
		Vec2 t;
		t.x = this->x - v.x;
		t.y = this->y - v.y;
		return t;
	}
	operator Vec3();
};
float dotProduct(const Vec2& v0, const Vec2& v1) {
	return v0.x * v1.x + v0.y * v1.y;
}
float crossProduct(const Vec2& v0, const Vec2& v1) {
	return v0.x * v1.y - v1.x * v0.y;
}
class Vec3
{
public:
	float x, y, z;
	Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vec3() {
		this->x = 0.f;
		this->y = 0.f;
		this->z = 0.f;
	}
	Vec3 operator+(const Vec3& v) const {
		Vec3 t;
		t.x = this->x + v.x;
		t.y = this->y + v.y;
		t.z = this->z + v.z;
		return t;
	}
	Vec3 operator-(const Vec3& v) const {
		Vec3 t;
		t.x = this->x - v.x;
		t.y = this->y - v.y;
		t.z = this->z - v.z;
		return t;
	}

	operator Vec2() const {
		Vec2 t;
		t.x = this->x;
		t.y = this->y;
		return t;
	}
};
float dotProduct(const Vec3& v0, const Vec3& v1) {
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}
Vec3 crossProduct(const Vec3& v0, const Vec3& v1) {
	Vec3 t;
	t.x = v0.y * v1.z - v1.y * v0.z;
	t.y = v1.x * v0.z - v0.x * v1.z;
	t.z = v0.x * v1.y - v1.x * v0.y;
	return t;
}
class Vec4
{
public:
	float x, y, z, w;
	Vec4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	Vec4() {
		this->x = 0.f;
		this->y = 0.f;
		this->z = 0.f;
		this->w = 0.f;
	}
	Vec4 operator+(const Vec4& v) const {
		Vec4 t;
		t.x = this->x + v.x;
		t.y = this->y + v.y;
		t.z = this->z + v.z;
		t.w = this->w + v.w;
		return t;
	}
	Vec4 operator-(const Vec4& v) const {
		Vec4 t;
		t.x = this->x - v.x;
		t.y = this->y - v.y;
		t.z = this->z - v.z;
		t.w = this->w - v.w;

		return t;
	}

	operator Vec2() const {
		Vec2 t;
		t.x = this->x;
		t.y = this->y;
		return t;
	}
	operator Vec3() const {
		Vec3 t;
		t.x = this->x;
		t.y = this->y;
		t.z = this->z;
		return t;
	}
};
float dotProduct(const Vec4& v0, const Vec4& v1) {
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
}
class Mat4 {
public:
	float mat[16];
	Mat4() {
		for (int i = 0; i < 16; i++)
			this->mat[i] = 0.f;
	}
	Mat4 operator+(const Mat4 &m) const {
		Mat4 t;
		for (int i = 0; i < 16; i++) {
			t.mat[i] = this->mat[i] + m.mat[i];
		}
		return t;
	}
	Mat4 operator-(const Mat4& m) const {
		Mat4 t;
		for (int i = 0; i < 16; i++) {
			t.mat[i] = this->mat[i] - m.mat[i];
		}
		return t;
	}
	Mat4 operator*(const Mat4& m) const {
		Mat4 t;
		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				for (int k = 0; k < 4; k++) {
					t.mat[r + c * 4] += this->mat[r + k * 4] * m.mat[c * 4  + k];
				}
			}
		}
		return t;
	}
	Vec4 operator*(const Vec4& v) const {
		Vec4 t;
		t.x = this->mat[0 * 4 + 0] * v.x
		+ this->mat[0 * 4 + 1] * v.y
		+ this->mat[0 * 4 + 2] * v.z
		+ this->mat[0 * 4 + 3] * v.w;
		t.y = this->mat[1 * 4 + 0] * v.x
		+this->mat[1 * 4 + 1] * v.y
		+this->mat[1 * 4 + 2] * v.z
		+this->mat[1 * 4 + 3] * v.w;
		t.z = this->mat[2 * 4 + 0] * v.x
		+this->mat[2 * 4 + 1] * v.y
		+this->mat[2 * 4 + 2] * v.z
		+this->mat[2 * 4 + 3] * v.w;
		t.w = this->mat[3 * 4 + 0] * v.x
		+this->mat[3 * 4 + 1] * v.y
		+this->mat[3 * 4 + 2] * v.z
		+this->mat[3 * 4 + 3] * v.w;
		return t;
	}
};
std::ostream& operator<<(std::ostream& out, const Mat4& mat) {

	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			out << mat.mat[r + c * 4] << " ";
		}
		out << "\n";
	}
	return out;
}
Vec2::operator Vec3(){
	Vec3 t;
	t.x = this->x;
	t.y = this->y;
	t.z = 0.f;
	return t;
}