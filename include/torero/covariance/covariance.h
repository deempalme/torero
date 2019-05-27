#ifndef TORERO_COVARIANCE_COVARIANCES_H
#define TORERO_COVARIANCE_COVARIANCES_H

#include "glad/glad.h"

#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/gl/texture.h"
#include "torero/definition/types.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  namespace covariance {
    class Covariances
    {
    public:
      Covariances(torero::gl::Shader *shader,
                  const std::vector<torero::covariance::Covariance> *matrices,
                  torero::gl::Buffer *plane, const GLsizei plane_data_size);

      void change_input(const std::vector<torero::covariance::Covariance> *matrices);

      void set_transformation_matrix(const algebraica::mat4f *transformation_matrix);

      void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      void rotate_in_x(const float angle);
      void rotate_in_y(const float angle);
      void rotate_in_z(const float angle);

      bool update();
      bool draw();

    private:
      void initialize();
      void restart();

      torero::gl::Shader *shader_;
      torero::gl::Buffer *buffer_plane_, buffer_data_;

      const std::vector<torero::covariance::Covariance> *matrices_;
      const float pi_pi_twice_;

      const algebraica::mat4f *primary_model_;
      algebraica::mat4f secondary_model_;
      const algebraica::mat4f identity_matrix_;
      GLsizei type_size_, data_size_, plane_data_size_;

      GLint i_position_, i_mean_, i_inverse_covariance_, i_equation_constant_;
      GLint u_primary_model_, u_secondary_model_;
    };
  }
}
#endif // TORERO_COVARIANCE_COVARIANCES_H
