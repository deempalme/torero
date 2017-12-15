#ifndef TORERO_OBJECTS_H
#define TORERO_OBJECTS_H

#include "glad/glad.h"

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/shader.h"
#include "include/texture.h"
#include "include/types.h"

#include "Algebraica.h"

#include <vector>

namespace Toreo {
  class Objects
  {
  public:
    Objects(Shader *shader_program, const std::vector<Visualizer::Object> *objects,
            Buffer *hollow, Texture *texture, Buffer *solid, Buffer *arrow,
            Texture *arrow_ao, const Visualizer::Shape type);

    void change_input(const std::vector<Visualizer::Object> *objects);

    void set_transformation_matrix(const Algebraica::mat4f *transformation_matrix);

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

    Shader *shader_;
    Buffer *buffer_hollow_data_, buffer_hollow_, *buffer_solid_data_, buffer_solid_;
    Buffer *buffer_arrow_data_, buffer_arrow_;
    Texture *ao_, *ao_arrow_;

    const std::vector<Visualizer::Object> *object_;
    Visualizer::Shape type_;
    GLsizei type_size_hollow_, type_size_solid_, type_size_arrow_;

    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_, identity_matrix_;
    GLsizei hollow_data_size_, solid_data_size_, arrow_data_size_;
    GLsizei hollow_type_size_, solid_type_size_;
    GLsizei buffer_size_;

    GLint i_position_, i_normal_, i_uv_, i_scales_;
    GLint i_translation_, i_rotation_, i_color_, i_scale_, i_line_width_;
    GLint u_primary_model_, u_secondary_model_, u_solid_;
  };
  }

#endif // TORERO_OBJECTS_H
