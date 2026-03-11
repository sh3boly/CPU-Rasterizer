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

	static Mat4 identity() {
		Mat4 mat;
		mat[0] = 1.0f;
		mat[5] = 1.0f;
		mat[10] = 1.0f;
		mat[15] = 1.0f;

		return mat;
	}

	Vec3 operator*(const Vec3& v) const {
		Vec3 t;
		t.x = mat[0] * v.x + mat[4] * v.y + mat[8]  * v.z + mat[12] * 1.0f;
		t.y = mat[1] * v.x + mat[5] * v.y + mat[9]  * v.z + mat[13] * 1.0f;
		t.z = mat[2] * v.x + mat[6] * v.y + mat[10] * v.z + mat[14] * 1.0f;
		float w = mat[3] * v.x + mat[7] * v.y + mat[11] * v.z + mat[15] * 1.0f;
        
		if (w != 1.0f && w != 0.0f) {
			t.x /= w;
			t.y /= w;
			t.z /= w;
		}
		return t;
	}

	Vec4 operator*(const Vec4& v) const {
		Vec4 t;
		t.x = mat[0] * v.x + mat[4] * v.y + mat[8]  * v.z + mat[12] * v.w;
		t.y = mat[1] * v.x + mat[5] * v.y + mat[9]  * v.z + mat[13] * v.w;
		t.z = mat[2] * v.x + mat[6] * v.y + mat[10] * v.z + mat[14] * v.w;
		t.w = mat[3] * v.x + mat[7] * v.y + mat[11] * v.z + mat[15] * v.w;
		return t;
	}
	float& operator[](int index) {
		if (index < 0 || index >= 16) {
		    throw std::out_of_range("Index out of bounds");
		}
		return mat[index];
	}

	const float& operator[](int index) const {
		if (index < 0 || index >= 16) {
		    throw std::out_of_range("Index out of bounds");
		}
		return mat[index];
	}
};
static Mat4 makeTranslationMatrix(Vec3 offset) {
	Mat4 mat = Mat4::identity();
	

	mat[12] = offset.x;
	mat[13] = offset.y;
	mat[14] = offset.z;
	return mat;
}
static Mat4 makeZRotationMatrix(float angle) {
	angle = angle * 3.14159265359f / 180.0f;
	
	float sine = std::sin(angle);
	float cosine = std::cos(angle);
	Mat4 mat = Mat4::identity();
	mat[0] = cosine; 
	mat[1] = sine;
	mat[4] = -sine;
	mat[5] = cosine;
	return mat;
}

static Mat4 makeYRotationMatrix(float angle) {
	angle = angle * 3.14159265359f / 180.0f;

	float sine = std::sin(angle);
	float cosine = std::cos(angle);
	Mat4 mat = Mat4::identity();
	mat[0] = cosine; 
	mat[2] = -sine;
	mat[5] = 1.0f;
	mat[8] = sine;
	mat[10] = cosine;
	mat[15] = 1.0f;
	return mat;
}

static Mat4 makeXRotationMatrix(float angle) {
	angle = angle * 3.14159265359f / 180.0f;

	float sine = std::sin(angle);
	float cosine = std::cos(angle);
	Mat4 mat = Mat4::identity();
	mat[5] = cosine;
	mat[6] = sine;
	mat[9] = -sine;
	mat[10] = cosine;
	return mat;
}

static Mat4 makeScalingMatrix(Vec3 scale) {
	Mat4 mat = Mat4::identity();
	mat[0] = scale.x;
	mat[5] = scale.y;
	mat[10] = scale.z; 

	return mat;
}

static Mat4 makePerspectiveMatrix(const float &fovDegrees, const float &aspectRatio, const float &near, const float &far) {
	Mat4 mat;
	float fovRad = fovDegrees * 3.14159265359f / 180.0f;
	float tanHalfFov = std::tan(fovRad / 2.0f);

	mat[0] = (1 / (tanHalfFov * aspectRatio));
	mat[5] = (1 / tanHalfFov);
	mat[10] = (-far - near) / (near - far);
	mat[14] = (2.0f * far * near) / (near - far);
	mat[11] = -1.0f;

	return mat;
}

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
