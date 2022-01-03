#pragma once

#include "Math/Glm.h"

namespace VKT {

    class Transform
    {
    public:
        Transform();

        void SetPosition(const glm::vec3 &position);
        void SetRotation(const glm::quat &rotation);

        void SetRotation(const glm::vec3 &axis, float angle);
        void SetScale(const glm::vec3 &scale);
        void SetMatrix4x4(const glm::mat4 &T);

        glm::vec3 GetPosition() const { return position_; }
        glm::quat GetRotation() const { return rotation_; }
        glm::vec3 GetScale() const { return scale_; }

        glm::mat4 GetMatrix() const { return local_transform_mat_; }

    private:
        void UpdateLocalTransformMatrix();

        glm::vec3 position_;
        glm::quat rotation_;
        glm::vec3 scale_;

        glm::mat4 local_transform_mat_;

    };

}
