#ifndef GUI_MENU_H
#define GUI_MENU_H

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/types.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace Toreo {
  class Core;

  class GUIMenu
  {
  public:
    GUIMenu(Core *core, Shader *color_shader, Shader *id_shader,
            bool *screen_changer, bool *blocker);

    void change_button_state(const Visualizer::Menu::ButtonType button_id,
                             const Visualizer::Button::State state);
    void restart_buttons();

    void click_event(const Visualizer::Menu::ButtonType button_id);

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

    Algebraica::vec2f menu_offset_, menu_alignment_;

    std::vector<Visualizer::Button::Button> buttons_;

  };
}

#endif // GUI_MENU_H
