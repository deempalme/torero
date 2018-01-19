#ifndef TORERO_TEXT_OBJECT_H
#define TORERO_TEXT_OBJECT_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/definitions.h"
#include "torero/font_loader.h"
#include "torero/shader.h"
#include "torero/types.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace Toreo {
  class TextObject
  {
  public:
    TextObject(Shader *shader);

    void set_input(const std::vector<Visualizer::TextSimple> *text);
    void set_input(const std::vector<Visualizer::TextColored> *text);
    void set_input(const std::vector<Visualizer::TextExtended> *text);

    void set_font(FontLoader *font);

    void set_color(const float r = 1.0f, const float g = 1.0f,
                   const float b = 1.0f, const float a = 1.0f);

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

    Shader *shader_;
    Buffer buffer_;
    FontLoader *font_;
    Visualizer::Complexity type_;

    const std::vector<Visualizer::TextSimple> *text_simple_;
    const std::vector<Visualizer::TextColored> *text_colored_;
    const std::vector<Visualizer::TextExtended> *text_extended_;

    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_, identity_matrix_;
    Algebraica::vec4f color_;

    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_position_, i_color_, i_distance_, i_angle_;
    GLint u_primary_model_, u_secondary_model_;
  };
}
#endif // TORERO_TEXT_OBJECT_H
