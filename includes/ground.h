#ifndef TORERO_GROUND_H
#define TORERO_GROUND_H

#include "glad/glad.h"

#include "includes/buffer.h"
#include "includes/definitions.h"
#include "includes/shader.h"
#include "includes/types.h"

#include "Algebraica.h"

#include <vector>

namespace Toreo {
  class Ground
  {
  public:
    Ground(Shader *ground_shader, Shader *lines_shader,
           const std::vector<Visualizer::Ground2D> *ground);
    Ground(Shader *ground_shader, Shader *lines_shader,
           const std::vector<Visualizer::Ground3D> *ground);

    void change_input(const std::vector<Visualizer::Ground2D> *ground);
    void change_input(const std::vector<Visualizer::Ground3D> *ground);

    void set_transformation_matrix(const Algebraica::mat4f *transformation_matrix);

    void set_ground_size(const float width = 100.0f,
                         const float length = 100.0f,
                         const unsigned int number_of_elements_through_width = 100u,
                         const unsigned int number_of_elements_through_length = 100u);

    void set_lines(const unsigned int line_quantity_in_width = 100u,
                   const unsigned int line_quantity_in_length = 100u);
    void line_color(const float r = 255.0f, const float g = 255.0f, const float b = 255.0f,
                    const float alpha = 255.0f);
    void line_visibility(const bool visible = true);

    void fog_visibility(const bool visible = true);

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

    Shader *ground_shader_, *lines_shader_;
    Buffer ground_buffer_, lines_buffer_;

    float width_, length_, element_width_, element_length_;
    unsigned int quantity_width_, quantity_length_;
    bool line_visibility_, fog_visibility_;
    Algebraica::vec4f line_color_;
    Algebraica::vec3f ground_position_;

    const std::vector<Visualizer::Ground2D> *ground_2D_;
    const std::vector<Visualizer::Ground3D> *ground_3D_;

    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_, identity_matrix_;

    GLsizei type_size_, line_type_size_;
    GLsizei data_size_, line_data_size_;

    GLint i_position_, i_color_, i_height_;
    GLint u_primary_model_, u_secondary_model_, u_fog_;
    GLint u_width_, u_length_, u_2D_, u_position_;

    GLint i_position_lines_;
    GLint u_primary_model_lines_, u_secondary_model_lines_, u_color_lines_, u_fog_lines_;
  };
}

#endif // TORERO_GROUND_H
