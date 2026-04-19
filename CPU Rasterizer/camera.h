#include "My_Math.h"

class Camera {
    Vec3 position;
    Vec3 direction;
    Vec3 up;
public:
    Camera(Vec3 position) {
        this->position = position;
    }

    Mat4 lookAt(Vec3 target) {
        // u v w
        this->direction = normalize((target - this->position)); // w
        Vec3 temp(0.0f, 1.f, 0.f); // v
        Vec3 right = normalize(crossProduct(temp, this->direction));
        this->up = normalize(crossProduct(this->direction, right));

        Mat4 lookAt = Mat4::identity();
        lookAt[0] = right.x;
        lookAt[1] = right.y;
        lookAt[2] = right.z;
        lookAt[4] = this->up.x;
        lookAt[5] = this->up.y;
        lookAt[6] = this->up.z;
        lookAt[8] = -this->direction.x;
        lookAt[9] = -this->direction.y;
        lookAt[10] = -this->direction.z;
        lookAt[12] = -dotProduct(right, this->position);
        lookAt[13] = -dotProduct(this->up, this->position);
        lookAt[14] = -dotProduct(this->direction, this->position);
        return lookAt;
    }

};
