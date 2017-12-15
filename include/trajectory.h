#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "glad/glad.h"

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/shader.h"
#include "include/types.h"

#include "Algebraica.h"

#include <vector>

namespace Toreo {
  class Trajectory
  {
  public:
    Trajectory(Shader *shader_program, const std::vector<Visualizer::Trajectory> *trajectories);

    void change_input(const std::vector<Visualizer::Trajectory> *trajectories);

    void set_transformation_matrix(const Algebraica::mat4f *transformation_matrix);

    void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    void translate(const Algebraica::vec3f translation);
    void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void rotate(const Algebraica::vec3f rotation);
    void rotate_in_x(const float angle);
    void rotate_in_y(const float angle);
    void rotate_in_z(const float angle);

    bool update();
    bool draw();

  private:
    void initialize();

    Shader *shader_;
    Buffer buffer_;

    const std::vector<Visualizer::Trajectory> *trajectories_;

    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_, identity_matrix_;

    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_position_, i_color_, i_line_width_, i_distance_, i_angle_;
    GLint u_primary_model_, u_secondary_model_;
  };
}

#endif // TRAJECTORY_H
