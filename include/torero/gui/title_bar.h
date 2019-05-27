#ifndef TORERO_GUI_TITLE_BAR_H
#define TORERO_GUI_TITLE_BAR_H

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  class Core;

  namespace gui {
    class TitleBar
    {
    public:
      TitleBar(torero::Core *core, torero::gl::Shader *color_shader,
               torero::gl::Shader *id_shader);

      void change_button_state(const torero::gui::title::ButtonType button_id,
                               const torero::gui::button::State state);
      void restart_buttons();

      void click_event(const torero::gui::title::ButtonType button_id,
                       const torero::gui::mouse::Event event);

      void draw();
      void draw_id();

      void set_buffer();

    private:

      Core *core_;
      const int *window_width_;

      torero::gl::Shader *color_shader_, *id_shader_;
      torero::gl::Buffer buffer_;
      GLsizei type_size_;

      GLint color_u_offset_, color_u_alignment_, id_u_offset_, id_u_alignment_;
      GLint i_position_, i_texture_, i_size_, i_id_, i_element_;

      algebraica::vec2f zero_;

      std::vector<torero::gui::button::Button> buttons_;
    };
  }
}
#endif // TORERO_GUI_TITLE_BAR_H
