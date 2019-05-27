#ifndef TORERO_LINE_GRID_H
#define TORERO_LINE_GRID_H

#include "glad/glad.h"

#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/definition/types.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  namespace ground {
    class LineGrid
    {
    public:
      LineGrid(torero::gl::Shader *line_shader, const float width, const float length,
               const unsigned int quantity_width, const unsigned int quantity_length);

      void fog(const bool foggy);
      void color(const float r, const float g, const float b, const float alpha);
      void properties(const float width, const float length,
                      const unsigned int line_quantity_through_width,
                      const unsigned int line_quantity_through_length);
      void rotate(const float pitch, const float yaw, const float roll);
      void translate(const float x, const float y, const float z);
      void transformation_matrix(const algebraica::mat4f *transformation_matrix);

      void draw();

    private:
      torero::gl::Shader *shader_;
      torero::gl::Buffer buffer_;

      algebraica::vec4f color_;
      const algebraica::mat4f *primary_model_, identity_matrix_;
      algebraica::mat4f secondary_model_;
      GLsizei data_size_;

      GLint i_position_;
      GLint u_primary_model_, u_secondary_model_, u_color_, u_fog_;
      int fog_;
    };
  }
}
#endif // TORERO_LINE_GRID_H
