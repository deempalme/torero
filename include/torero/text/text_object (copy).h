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

namespace torero {
  class TextObject
  {
  public:
    TextObject(Shader *shader, const torero::TextType dimensions);

    void set_input(const std::vector<torero::TextSimple> *text);
    void set_input(const std::vector<torero::TextColored> *text);

    void set_font(FontLoader *font);

    void set_color(const float R = 1.0f, const float G = 1.0f,
                   const float B = 1.0f, const float A = 1.0f);

    void set_foggy(const bool fog = false);
    void set_size(const unsigned int size = 15u);

    void set_line_height(const float line_height = 1.2f);
    void set_vertical_alignment(const torero::Alignment
                                alignment = torero::Alignment::TOP);
    void set_horizontal_alignment(const torero::Alignment
                                  alignment = torero::Alignment::LEFT);

    void set_weight(const torero::TextWeight weight = torero::TextWeight::NORMAL);
    bool set_edge_softness(const unsigned int softness = 1u);
    bool set_border_width(const unsigned int width = 0u);
    bool set_border_softness(const unsigned int softness = 0u);
    bool set_border_color(const float R = 0.5f,
                          const float G = 0.5f,
                          const float B = 0.5f,
                          const float A = 1.0f);
    bool set_border_offset(const unsigned int x = 0u, const unsigned int y = 0u);

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
    void text_metrics(const std::string &text, algebraica::vec2f *offset,
                      std::vector<float> *line_starts, std::size_t *last,
                      std::vector<torero::TextSimpleShader> *simple = nullptr,
                      std::vector<torero::TextColoredShader> *colored = nullptr);

    Shader *shader_;
    Buffer buffer_;

    FontLoader *font_;

    torero::Complexity type_;
    torero::TextWeight weight_;

    const bool is_3D_;

    const std::vector<torero::TextSimple> *text_simple_;
    const std::vector<torero::TextColored> *text_colored_;
    std::vector<algebraica::vec3f> text_origins_;

    const algebraica::mat4f *primary_model_;
    algebraica::mat4f secondary_model_, identity_matrix_;
    algebraica::vec4f color_, border_color_;
    float line_height_;
    algebraica::vec3f size_softness_weight_;
    algebraica::vec4f border_width_softness_offset_;
    bool has_fog_, has_border_, has_color_;
    float has_orientation_;
    int has_fog_int_, has_border_int_, has_color_int_;
    torero::Alignment vertical_alignment_;
    torero::Alignment horizontal_alignment_;

    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_origin_, i_position_, i_color_, i_angle_, i_texture_, i_size_;
    GLint u_primary_model_, u_secondary_model_, u_color_, u_colored_, u_size_soft_weight_;
    GLint u_foggy_, u_border_, u_border_color_, u_border_width_soft_offset_, u_oriented_;

    GLint size_1_, size_2_, size_3_, size_4_;
  };
}
#endif // TORERO_TEXT_OBJECT_H
