#ifndef TORERO_GUI_H
#define TORERO_GUI_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/frame_buffer.h"
#include "torero/gui_compass.h"
#include "torero/gui_menu.h"
#include "torero/gui_speedometer.h"
#include "torero/gui_title_bar.h"
#include "torero/render_buffer.h"
#include "torero/shader.h"
#include "torero/texture.h"
#include "torero/type_definitions.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;
  class Cubemap;
  class ModelManager;
  class TextManager;

  class GUIManager
  {
  public:
    GUIManager(Core *core, ModelManager *model_manager, TextManager *text_manager,
               const std::string folder_address = "resources/GUI");
    ~GUIManager();

    bool compass_add();
    bool compass_connect_rotation(boost::signals2::signal<void (float, float)> *signal);
    bool compass_connect_pedals(boost::signals2::signal<void (float, float, float,
                                                              std::string)> *signal);
    bool compass_disconnect_rotation();
    bool compass_disconnect_pedals();
    bool compass_update_rotation(const float compass_angle = 0.0f,
                                 const float steering_angle = 0.0f);
    bool compass_update_pedals(const float brake = 0.0f, const float accelerator = 0.0f,
                               const float clutch = 0.0f, const std::string gear_position = "P");
    void compass_visibility(const bool visible = true);
    bool compass_delete();

    bool speedometer_add();
    bool speedometer_connect(boost::signals2::signal<void (float, float)> *signal);
    bool speedometer_disconnect();
    bool speedometer_update(const float speed = 0.0f, const float rpm = 0.0f);
    void speedometer_visibility(const bool visible = true);
    bool speedometer_delete();

    void menu_visibility(const bool visibility = false);
    bool menu_offset(const int top = 0, const int left = 8);
    bool menu_alignment(const torero::Alignment vertical = torero::Alignment::Center,
                        const torero::Alignment horizontal = torero::Alignment::Left);
    bool menu_change_button_state(const torero::Menu::ButtonType button_id,
                                  const torero::Button::State state);

    void title_bar_visibility(const bool visibility = true);

    void draw_all();

  private:
    bool check_folder();
    void update_camera();
    void resized_screen(const int width, const int height);
    void calculate_alignment(auto *element);

    void mouse_eval(const unsigned int red, const unsigned int green, const unsigned int blue);
    void mouse_move(int x, int y);
    void mouse_event(int x, int y, int /*mode*/, torero::Mouse::Event state);

    bool *has_changed_, *blocked_;
    std::string folder_address_;

    Core *core_;
    Cubemap *cubemap_;
    TextManager *text_manager_;
    FTid iceland_id_;

    Shader *pbr_shader_;
    GLint u_pbr_projection_, u_pbr_view_, u_pbr_camera_;

    Shader *painter_shader_;
    GLint u_projection_, u_model_;

    Shader *mouse_shader_;
    GLint u_m_window_size_;

    Shader *plane_shader_;
    GLint u_p_window_size_;

    Shader *text_shader_;
    GLint u_t_projection_;

    algebraica::mat4f ortho_;
    float window_width_, window_height_;

    FrameBuffer frame_buffer_;
    RenderBuffer render_buffer_;

    GUICompass *compass_;
    bool compass_visibility_;
    boost::signals2::connection compass_rotation_signal_, compass_pedals_signal_;

    torero::Menu::Object menu_;
    Texture *gui_2d_albedo_;

    GUISpeedometer *speedometer_;
    bool speedometer_visibility_;
    boost::signals2::connection speedometer_signal_;

    GUITitleBar *title_bar_;
    bool title_bar_visibility_;

    unsigned int last_changed_object_;
    unsigned int last_changed_id_;
    bool is_clicked_;

    boost::signals2::connection signal_draw_all_, signal_updated_all_, signal_updated_camera_;
    boost::signals2::connection signal_resized_screen_, signal_mouse_moved_;
    boost::signals2::connection signal_clicked_mouse_, signal_released_mouse_;
  };
}

#endif // TORERO_GUI_H
