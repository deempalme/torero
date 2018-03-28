#ifndef TORERO_GUI_TITLE_BAR_H
#define TORERO_GUI_TITLE_BAR_H

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/types.h"

#include "algebraica/algebraica.h"

#include <vector>

namespace Toreo {
  class Core;

  class GUITitleBar
  {
  public:
    GUITitleBar(Core *core, Shader *color_shader, Shader *id_shader,
                bool *screen_changer, bool *blocker);

    void change_button_state(const Visualizer::Title::ButtonType button_id,
                             const Visualizer::Button::State state);
    void restart_buttons();

    void click_event(const Visualizer::Title::ButtonType button_id,
                     const Visualizer::Mouse::Event event);

    void draw();
    void draw_id();

    void set_buffer();

  private:

    Core *core_;
    const int *window_width_;

    Shader *color_shader_, *id_shader_;
    bool *has_changed_, *blocked_, *window_mover_;
    Buffer buffer_;
    GLsizei type_size_;

    GLint color_u_offset_, color_u_alignment_, id_u_offset_, id_u_alignment_;
    GLint i_position_, i_texture_, i_size_, i_id_, i_element_;

    Algebraica::vec2f zero_;

    std::vector<Visualizer::Button::Button> buttons_;

  };
}

#endif // TORERO_GUI_TITLE_BAR_H
