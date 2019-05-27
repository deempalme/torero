#ifndef TORERO_GUI_OPTIONS_H
#define TORERO_GUI_OPTIONS_H

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  class Core;

  namespace gui {
    class Options
    {
    public:
      Options(torero::Core *core, torero::gl::Shader *color_shader,
              torero::gl::Shader *id_shader, bool *screen_changer, bool *blocker);

      void change_button_state(const torero::gui::menu::ButtonType button_id,
                               const torero::gui::button::State state);
      void restart_buttons();

      void click_event(const torero::gui::menu::ButtonType button_id);

      void set_offset(const int top, const int left);
      void set_alignment(const float top, const float left);
      void draw();
      void draw_id();

    private:
      void set_buffer();

      torero::Core *core_;

      torero::gl::Shader *color_shader_, *id_shader_;
      bool *has_changed_, *blocked_;
      torero::gl::Buffer buffer_;
      GLsizei type_size_;

      GLint color_u_offset_, color_u_alignment_, id_u_offset_, id_u_alignment_;
      GLint i_position_, i_texture_, i_size_, i_id_, i_element_;

      algebraica::vec2f menu_offset_, menu_alignment_;

      std::vector<torero::gui::button::Button> buttons_;
    };
  }
}
#endif // TORERO_GUI_OPTIONS_H
