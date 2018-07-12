#ifndef TORERO_CAMERA_ELEMENT_H
#define TORERO_CAMERA_ELEMENT_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"
// Linear mathematic library
#include "algebraica/algebraica.h"
// Standard libraries
#include <vector>

namespace torero {
  class CameraElement
  {
  public:
    CameraElement(Shader *shader);

    void set_camera(const torero::ImageFile *video_input);

    void set_curvature(const float curvature = 1.0f);

    void set_dimensions(const torero::Dimensionality number_of_dimensions);
    void set_transformation_matrix(const algebraica::mat4f *transformation_matrix);

    void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    void translate(const algebraica::vec3f translation);

    void resize(const float width, const float height);

    void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void rotate(const algebraica::vec3f rotation);
    void rotate_in_x(const float angle);
    void rotate_in_y(const float angle);
    void rotate_in_z(const float angle);

    bool update();

    bool draw();

  private:
    void prepare_plane();

    Shader *shader_;
    Buffer buffer_, buffer_plane_;

    const torero::ImageFile *camera_;
    torero::Dimensionality dimensionality_;

    const algebraica::mat4f identity_matrix_;
    const algebraica::mat4f *primary_model_;
    algebraica::mat4f secondary_model_;

    float curvature_;
    algebraica::vec2f dimensions_;
    float pixel_width_, pixel_height_, channels_;

    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_position_, i_intensity_, i_color_, i_alpha_;
    GLint u_primary_model_, u_secondary_model_;
  };
}

#endif // TORERO_CAMERA_ELEMENT_H
