#ifndef TORERO_GUI_MANAGER_H
#define TORERO_GUI_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/frame_buffer.h"
#include "torero/gl/render_buffer.h"
#include "torero/gl/shader.h"
#include "torero/gl/texture.h"
#include "torero/gui/compass.h"
#include "torero/gui/menu.h"
#include "torero/gui/speedometer.h"
#include "torero/gui/title_bar.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

typedef struct SDL_WindowEvent SDL_WindowEvent;
typedef struct SDL_MouseButtonEvent SDL_MouseButtonEvent;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;

namespace torero {
  class Core;
  namespace model {
    class Environment;
    class Manager;
  }
  namespace text { class Manager; }

  namespace gui {
    class Manager
    {
    public:
      Manager(torero::Core *core,
              const std::string folder_address = torero::k_gui_folder);
      ~Manager();

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
      bool menu_alignment(const torero::text::Alignment vertical = torero::text::Alignment::Center,
                          const torero::text::Alignment horizontal = torero::text::Alignment::Left);
      bool menu_change_button_state(const torero::gui::menu::ButtonType button_id,
                                    const torero::gui::button::State state);

      void title_bar_visibility(const bool visibility = true);

      void draw_all();

    private:
      void update_camera();
      void window_event(const SDL_WindowEvent &event);
      void calculate_alignment(auto *element);

      void mouse_eval(const unsigned int red, const unsigned int green, const unsigned int blue);
      void mouse_motion(const SDL_MouseMotionEvent &event);
      void mouse_button(const SDL_MouseButtonEvent &event);

      std::string folder_address_;

      torero::Core *core_;
      torero::model::Environment *environment_;
      torero::text::Manager *text_manager_;
      FTid iceland_id_;

      torero::gl::Shader *pbr_shader_;
      GLint u_pbr_projection_, u_pbr_camera_;

      torero::gl::Shader *painter_shader_;
      GLint u_projection_, u_model_;

      torero::gl::Shader *mouse_shader_;
      GLint u_m_window_size_;

      torero::gl::Shader *plane_shader_;
      GLint u_p_window_size_;

      torero::gl::Shader *text_shader_;
      GLint u_t_projection_;

      algebraica::mat4f ortho_;
      float window_width_, window_height_;

      torero::gl::FrameBuffer frame_buffer_;
      torero::gl::RenderBuffer render_buffer_;

      torero::gui::Compass *compass_;
      bool compass_visibility_;
      boost::signals2::connection compass_rotation_signal_, compass_pedals_signal_;

      torero::gui::menu::Object menu_;
      torero::gl::Texture *gui_2d_albedo_;

      torero::gui::Speedometer *speedometer_;
      bool speedometer_visibility_;
      boost::signals2::connection speedometer_signal_;

      torero::gui::TitleBar *title_bar_;
      bool title_bar_visibility_;

      unsigned int last_changed_object_;
      unsigned int last_changed_id_;
      bool is_clicked_;

      boost::signals2::connection connection_draw_, connection_camera_;
      boost::signals2::connection connection_window_event_, connection_mouse_motion_event_;
      boost::signals2::connection connection_mouse_button_event_;
    };
  }
}
#endif // TORERO_GUI_MANAGER_H
