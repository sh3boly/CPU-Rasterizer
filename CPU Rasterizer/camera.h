#pragma once
#include "My_Math.h"

class Camera {
    Vec3 position;
    Vec3 front = Vec3(0.f, 0.f, -1.f);
    Vec3 right;
    Vec3 up;
    const float cameraSpeed = 0.15f;
public:
    Camera(Vec3 position) {
        this->position = position;
    }

    Mat4 lookAt() {
        // u v w
        this->front = normalize(this->front);
        Vec3 temp(0.0f, 1.f, 0.f); // v
        this->right = normalize(crossProduct(this->front , temp));
        this->up = normalize(crossProduct(this->right, this->front));

        Mat4 lookAt = Mat4::identity();

        lookAt[0] = this->right.x;
        lookAt[1] = this->right.y;
        lookAt[2] = this->right.z;
        lookAt[4] = this->up.x;
        lookAt[5] = this->up.y;
        lookAt[6] = this->up.z;
        lookAt[8] = -this->front.x;
        lookAt[9] = -this->front.y;
        lookAt[10] = -this->front.z;
        lookAt[12] = -dotProduct(this->right, this->position);
        lookAt[13] = -dotProduct(this->up, this->position);
        lookAt[14] = dotProduct(this->front, this->position);
        return lookAt;
    }

    void moveForward() {
        this->position += cameraSpeed * this->front;
    }

    void moveBackward() {
        this->position -= cameraSpeed * this->front;
    }

    void moveLeft() {
        this->position -= cameraSpeed * this->right;
    }

    void moveRight() {
        this->position += cameraSpeed * this->right;
    }

};
