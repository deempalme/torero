#ifndef TORERO_TRAJECTORY_TRAJECTORIES_H
#define TORERO_TRAJECTORY_TRAJECTORIES_H

#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  namespace trajectory {
    class Trajectories
    {
    public:
      Trajectories(torero::gl::Shader *shader_program,
                   const std::vector<torero::trajectory::Trajectory> *trajectories);

      void change_input(const std::vector<torero::trajectory::Trajectory> *trajectories);

      void set_transformation_matrix(const algebraica::mat4f *transformation_matrix);

      void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      void translate(const algebraica::vec3f translation);
      void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      void rotate(const algebraica::vec3f rotation);
      void rotate_in_x(const float angle);
      void rotate_in_y(const float angle);
      void rotate_in_z(const float angle);

      bool update();
      bool draw();

    private:
      void initialize();

      torero::gl::Shader *shader_;
      torero::gl::Buffer buffer_;

      const std::vector<torero::trajectory::Trajectory> *trajectories_;

      const algebraica::mat4f *primary_model_;
      algebraica::mat4f secondary_model_, identity_matrix_;

      GLsizei type_size_;
      GLsizei data_size_;

      GLint i_position_, i_color_, i_line_width_, i_distance_, i_angle_;
      GLint u_primary_model_, u_secondary_model_;
    };
  }
}
#endif // TORERO_TRAJECTORY_TRAJECTORIES_H
