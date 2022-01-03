#include "Transform.h"

namespace VKT {

    Transform::Transform()
        : position_(0.f),
          rotation_(glm::quat(1.f, 0.f, 0.f, 0.f)),
          scale_(glm::vec3(1.f))
    {
        UpdateLocalTransformMatrix();
    }

    void Transform::SetPosition(const glm::vec3 &position)
    {
        position_ = position;
        UpdateLocalTransformMatrix();
    }

    void Transform::SetRotation(const glm::quat &rotation)
    {
        rotation_ = rotation;
        UpdateLocalTransformMatrix();
    }

    void Transform::SetRotation(const glm::vec3 &axis, float angle)
    {
        SetRotation(glm::quat(cosf(angle / 2), axis.x * sinf(angle / 2),
                              axis.y * sinf(angle / 2), axis.z * sinf(angle / 2)));
    }

    void Transform::SetScale(const glm::vec3 &scale)
    {
        scale_ = scale;
        UpdateLocalTransformMatrix();
    }

    void Transform::SetMatrix4x4(const glm::mat4 &T)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(T, scale_, rotation_, position_, skew, perspective);
        // Won't use skew or perspective.
        UpdateLocalTransformMatrix();
    }

    void Transform::UpdateLocalTransformMatrix()
    {
        glm::mat4 new_matrix(1.f);

        // Order: scale, rotate, translate
        new_matrix = glm::scale(glm::mat4(1.f), scale_) * new_matrix;
        new_matrix = glm::mat4_cast(rotation_) * new_matrix;
        new_matrix = glm::translate(glm::mat4(1.f), position_) * new_matrix;

        local_transform_mat_ = new_matrix;
    }

}
