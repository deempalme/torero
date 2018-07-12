#ifndef GUI_OPTIONS_H
#define GUI_OPTIONS_H

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace torero {
  class Core;

  class GUIOptions
  {
  public:
    GUIOptions(Core *core, Shader *color_shader, Shader *id_shader,
               bool *screen_changer, bool *blocker);

    void change_button_state(const torero::Menu::ButtonType button_id,
                             const torero::Button::State state);
    void restart_buttons();

    void click_event(const torero::Menu::ButtonType button_id);

    void set_offset(const int top, const int left);
    void set_alignment(const float top, const float left);
    void draw();
    void draw_id();

  private:
    void set_buffer();

    Core *core_;

    Shader *color_shader_, *id_shader_;
    bool *has_changed_, *blocked_;
    Buffer buffer_;
    GLsizei type_size_;

    GLint color_u_offset_, color_u_alignment_, id_u_offset_, id_u_alignment_;
    GLint i_position_, i_texture_, i_size_, i_id_, i_element_;

    algebraica::vec2f menu_offset_, menu_alignment_;

    std::vector<torero::Button::Button> buttons_;

  };
}

#endif // GUI_OPTIONS_H
