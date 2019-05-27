#include "torero/gui/manager.h"

#include "torero/core.h"
#include "torero/camera/view.h"
#include "torero/event/handler.h"
#include "torero/model/environment.h"
#include "torero/text/manager.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"
#include <SDL2/SDL_events.h>

namespace torero {
  namespace gui {
    Manager::Manager(torero::Core *core, const std::string folder_address) :
      folder_address_(folder_address),

      core_(core),
      environment_(nullptr),
      text_manager_(nullptr),

      pbr_shader_(new torero::gl::Shader(torero::k_pbr_gui_vertex, torero::k_pbr_gui_fragment)),
      u_pbr_projection_(pbr_shader_->uniform_location("u_projection")),
      u_pbr_camera_(pbr_shader_->uniform_location("u_camera")),

      painter_shader_(new torero::gl::Shader(torero::k_3D_gui_vertex, torero::k_3D_gui_fragment)),
      u_projection_(painter_shader_->uniform_location("u_projection")),
      u_model_(painter_shader_->uniform_location("u_model")),

      mouse_shader_(new torero::gl::Shader(torero::k_gui_mouse_vertex, torero::k_gui_mouse_fragment)),
      u_m_window_size_(mouse_shader_->uniform_location("u_window_size")),

      plane_shader_(new torero::gl::Shader(torero::k_2D_gui_vertex, torero::k_2D_gui_fragment)),
      u_p_window_size_(plane_shader_->uniform_location("u_window_size")),

      text_shader_(new torero::gl::Shader(torero::k_gui_text_vertex, torero::k_gui_text_fragment,
                                          torero::k_gui_text_geometry)),
      u_t_projection_(text_shader_->uniform_location("u_projection")),

      ortho_(algebraica::mat4f::from_ortho(-k_ortho_width, k_ortho_width,
                                           -k_ortho_height, k_ortho_height, -1, 1)),
      window_width_(k_default_width),
      window_height_(k_default_height),
      frame_buffer_(true),
      render_buffer_(true),
      compass_(nullptr),
      compass_visibility_(false),
      menu_(),
      gui_2d_albedo_(nullptr),
      speedometer_(nullptr),
      speedometer_visibility_(false),
      title_bar_(new gui::TitleBar(core_, plane_shader_, mouse_shader_)),
      title_bar_visibility_(true),
      last_changed_object_(-1u),
      last_changed_id_(-1u),
      is_clicked_(false),
      connection_draw_(core->syncronize(torero::Order::GUI, boost::bind(&gui::Manager::draw_all,
                                                                        this))),
      connection_camera_(core->connect_camera_updates(boost::bind(&gui::Manager::update_camera,
                                                                  this))),
      connection_window_event_(core->connect_window_event(
                                 boost::bind(&gui::Manager::window_event, this, _1))),
      connection_mouse_motion_event_(core->connect_mouse_motion(
                                       boost::bind(&gui::Manager::mouse_motion, this, _1))),
      connection_mouse_button_event_(core->connect_mouse_button(
                                       boost::bind(&gui::Manager::mouse_button, this, _1)))
    {
      core_->set_module(this, environment_, text_manager_);

      torero::image::File gui_main_albedo;
      const bool checked{torero::file::Manager::check_path(&folder_address_)};

      if(pbr_shader_->use()){
        pbr_shader_->set_value("u_irradiance", 0);
        pbr_shader_->set_value("u_prefilter", 1);
        pbr_shader_->set_value("u_brdfLUT", 2);
        pbr_shader_->set_value("u_albedo", 3);
        pbr_shader_->set_value("u_normal", 4);
        pbr_shader_->set_value("u_metallic", 5);
        pbr_shader_->set_value("u_roughness", 6);
        pbr_shader_->set_value("u_ao", 7);
        pbr_shader_->set_value("u_emission", 8);

        pbr_shader_->set_value(u_pbr_projection_, ortho_);
      }else
        std::cout << pbr_shader_->error_log() << std::endl;

      if(plane_shader_->use()){
        if(checked){
          stbi_set_flip_vertically_on_load(true);

          gui_main_albedo.data = stbi_load(std::string(folder_address_ +
                                                       "two_dimensional_elements/albedo.png").c_str(),
                                           &gui_main_albedo.width, &gui_main_albedo.height,
                                           &gui_main_albedo.components_size, 0);

          if(gui_main_albedo.data){
            gui_2d_albedo_ = new torero::gl::Texture(true, torero::ShaderTexture::Albedo);
            gui_2d_albedo_->bind();
            gui_2d_albedo_->allocate(gui_main_albedo.width, gui_main_albedo.height,
                                     gui_main_albedo.data, gui_main_albedo.components_size);
            gui_2d_albedo_->mipmap(false);
            gui_2d_albedo_->parameter();
          }

          plane_shader_->set_value("u_albedo", torero::ShaderTexture::Albedo);
          plane_shader_->set_value("u_texture_size",
                                   algebraica::vec2f(static_cast<float>(gui_main_albedo.width),
                                                     static_cast<float>(gui_main_albedo.height)));
          plane_shader_->set_value(u_p_window_size_,
                                   algebraica::vec2f(window_width_, window_height_));

          stbi_image_free(gui_main_albedo.data);
        }
      }else
        std::cout << plane_shader_->error_log() << std::endl;

      if(mouse_shader_->use()){
        if(checked){
          mouse_shader_->set_value("u_albedo", torero::ShaderTexture::Albedo);
          mouse_shader_->set_value("u_texture_size",
                                   algebraica::vec2f(static_cast<float>(gui_main_albedo.width),
                                                     static_cast<float>(gui_main_albedo.height)));
          mouse_shader_->set_value(u_m_window_size_,
                                   algebraica::vec2f(window_width_, window_height_));

          render_buffer_.bind();
          render_buffer_.storage(static_cast<GLint>(window_width_),
                                 static_cast<GLint>(window_height_), GL_RGBA8);
          frame_buffer_.bind();
          render_buffer_.attach_to_framebuffer(frame_buffer_.frame_attachment());

          render_buffer_.release();
          frame_buffer_.release();

        }
      }else
        std::cout << mouse_shader_->error_log() << std::endl;

      if(text_shader_->use()){
        if(checked){
          iceland_id_ = text_manager_->add_font(torero::k_font_iceland_image,
                                                torero::k_font_iceland_config);
          text_shader_->set_value("u_font_atlas", 9);
          text_shader_->set_value(u_t_projection_, ortho_);
        }
      }else
        std::cout << text_shader_->error_log() << std::endl;
    }

    Manager::~Manager(){
      connection_draw_.disconnect();
      connection_camera_.disconnect();
      connection_window_event_.disconnect();
      connection_mouse_motion_event_.disconnect();
      connection_mouse_button_event_.disconnect();

      speedometer_signal_.disconnect();
      compass_rotation_signal_.disconnect();
      compass_pedals_signal_.disconnect();

      if(menu_.object) delete menu_.object;
      if(title_bar_) delete title_bar_;
      if(compass_) delete compass_;
      if(speedometer_) delete speedometer_;

      if(gui_2d_albedo_) delete gui_2d_albedo_;

      delete pbr_shader_;
      delete painter_shader_;
      delete mouse_shader_;
      delete plane_shader_;
      delete text_shader_;
    }

    void Manager::menu_visibility(const bool visibility){
      if(!menu_.object && visibility){
        menu_.object = new gui::Menu(core_, plane_shader_, mouse_shader_);
        calculate_alignment(&menu_);
      }
      menu_.visibility = visibility;
    }

    bool Manager::menu_offset(const int top, const int left){
      if(menu_.object){
        menu_.object->set_offset(top, left);
        return true;
      }else
        return false;
    }

    bool Manager::menu_alignment(const torero::text::Alignment vertical,
                                 const torero::text::Alignment horizontal){
      if(menu_.object){
        menu_.vertical = vertical;
        menu_.horizontal = horizontal;

        calculate_alignment(&menu_);
        return true;
      }else
        return false;
    }

    bool Manager::menu_change_button_state(const torero::gui::menu::ButtonType button_id,
                                           const torero::gui::button::State state){
      if(menu_.object){
        menu_.object->change_button_state(button_id, state);
        return true;
      }else
        return false;
    }

    void Manager::title_bar_visibility(const bool visibility){
      title_bar_visibility_ = visibility;
    }

    bool Manager::compass_add(){
      if(compass_){
        compass_visibility_ = true;
        return false;
      }else{
        compass_ = new gui::Compass(core_, pbr_shader_, text_shader_,
                                    text_manager_->font_characters(iceland_id_),
                                    torero::k_compass_model);
        compass_visibility_ = true;
      }

      return true;
    }

    bool Manager::compass_connect_rotation(boost::signals2::signal<void (float, float)> *signal){
      if(compass_){
        if(compass_rotation_signal_.connected())
          compass_rotation_signal_.disconnect();
        compass_rotation_signal_ = signal->connect(boost::bind(&gui::Manager::compass_update_rotation,
                                                               this, _1, _2));
        return true;
      }else
        return false;
    }

    bool Manager::compass_connect_pedals(boost::signals2::signal<void (float, float, float,
                                                                       std::string)> *signal){
      if(compass_){
        if(compass_pedals_signal_.connected())
          compass_pedals_signal_.disconnect();
        compass_pedals_signal_ = signal->connect(boost::bind(&gui::Manager::compass_update_pedals,
                                                             this, _1, _2, _3, _4));
        return true;
      }else
        return false;
    }

    bool Manager::compass_disconnect_rotation(){
      if(compass_){
        if(compass_rotation_signal_.connected())
          compass_rotation_signal_.disconnect();
        return true;
      }else
        return false;
    }

    bool Manager::compass_disconnect_pedals(){
      if(compass_){
        if(compass_pedals_signal_.connected())
          compass_pedals_signal_.disconnect();
        return true;
      }else
        return false;
    }

    bool Manager::compass_update_rotation(const float compass_angle, const float steering_angle){
      if(compass_){
        compass_->update_rotation(compass_angle, steering_angle);
        core_->reload_screen();
        return true;
      }else
        return false;
    }

    bool Manager::compass_update_pedals(const float brake, const float accelerator,
                                        const float clutch, const std::string gear_position){
      if(compass_){
        compass_->update_pedals(brake, accelerator, clutch, gear_position);
        core_->reload_screen();
        return true;
      }else
        return false;
    }

    void Manager::compass_visibility(const bool visible){
      if(visible && !compass_)
        compass_add();
      else
        compass_visibility_ = visible;
    }

    bool Manager::compass_delete(){
      if(compass_){
        if(compass_rotation_signal_.connected())
          compass_rotation_signal_.disconnect();

        if(compass_pedals_signal_.connected())
          compass_pedals_signal_.disconnect();

        delete compass_;

        compass_visibility_ = false;
        return true;
      }else
        return false;
    }

    bool Manager::speedometer_add(){
      if(speedometer_)
        return false;
      else{
        speedometer_ = new gui::Speedometer(core_, pbr_shader_, text_shader_,
                                            text_manager_->font_characters(iceland_id_),
                                            torero::k_speedo_model);
        speedometer_visibility_ = true;
      }

      return true;
    }

    bool Manager::speedometer_connect(boost::signals2::signal<void (float, float)> *signal){
      if(speedometer_){
        if(speedometer_signal_.connected())
          speedometer_signal_.disconnect();
        speedometer_signal_ = signal->connect(boost::bind(&gui::Manager::speedometer_update,
                                                          this, _1, _2));
        return true;
      }else
        return false;
    }

    bool Manager::speedometer_disconnect(){
      if(speedometer_){
        if(speedometer_signal_.connected())
          speedometer_signal_.disconnect();
        return true;
      }else
        return false;
    }

    bool Manager::speedometer_update(const float speed, const float rpm){
      if(speedometer_){
        speedometer_->update(speed, rpm);
        core_->reload_screen();
        return true;
      }else
        return false;
    }

    void Manager::speedometer_visibility(const bool visible){
      if(visible && !speedometer_){
        speedometer_add();
      }else
        speedometer_visibility_ = visible;
    }

    bool Manager::speedometer_delete(){
      if(speedometer_){
        if(speedometer_signal_.connected())
          speedometer_signal_.disconnect();

        delete speedometer_;

        speedometer_visibility_ = false;
        return true;
      }else
        return false;
    }

    void Manager::draw_all(){
      if(pbr_shader_->use()){
        environment_->bind_reflectance();
        if(speedometer_visibility_)
          speedometer_->draw();
        if(compass_visibility_)
          compass_->draw();
      }

      if(text_shader_->use()){
        text_manager_->use_font(iceland_id_);
        if(speedometer_visibility_)
          speedometer_->draw_text();
        if(compass_visibility_)
          compass_->draw_text();
      }

      if(gui_2d_albedo_){
        gui_2d_albedo_->bind();
        if(plane_shader_->use()){
          if(menu_.visibility)
            menu_.object->draw();

          if(title_bar_visibility_)
            title_bar_->draw();
        }

        if(mouse_shader_->use()){
          frame_buffer_.bind();
          frame_buffer_.clear();

          if(menu_.visibility)
            menu_.object->draw_id();

          if(title_bar_visibility_)
            title_bar_->draw_id();

          frame_buffer_.release();
        }
      }
    }

    void Manager::update_camera(){
      pbr_shader_->use();
      pbr_shader_->set_value(u_pbr_camera_, core_->camera().position());
    }

    void Manager::window_event(const SDL_WindowEvent &event){
      window_width_ = static_cast<float>(event.data1);
      window_height_ = static_cast<float>(event.data2);

      plane_shader_->use();
      plane_shader_->set_value(u_p_window_size_, algebraica::vec2f(window_width_, window_height_));

      mouse_shader_->use();
      mouse_shader_->set_value(u_m_window_size_, algebraica::vec2f(window_width_, window_height_));

      title_bar_->set_buffer();

      render_buffer_.bind();
      render_buffer_.storage(event.data1, event.data2, GL_RGBA8);
      render_buffer_.release();

      const float height_ratio {torero::k_ortho_width / (window_width_/window_height_)};
      ortho_.ortho(-torero::k_ortho_width, torero::k_ortho_width, -height_ratio,
                   height_ratio, -1, 1);

      pbr_shader_->use();
      pbr_shader_->set_value(u_pbr_projection_, ortho_);

      text_shader_->use();
      text_shader_->set_value(u_t_projection_, ortho_);

      calculate_alignment(&menu_);
    }

    void Manager::calculate_alignment(auto *element){
      float top, left;

      switch(static_cast<unsigned int>(element->vertical)){
        case static_cast<unsigned int>(torero::text::Alignment::Center):
          top = (window_height_ - element->height) / 2.0f;
        break;
        case static_cast<unsigned int>(torero::text::Alignment::Bottom):
          top = window_height_ - element->height;
        break;
        default:
          top = 0.0f;
        break;
      }

      switch(static_cast<unsigned int>(element->horizontal)){
        case static_cast<unsigned int>(torero::text::Alignment::Center):
          left = (window_width_ - element->width) / 2.0f;
        break;
        case static_cast<unsigned int>(torero::text::Alignment::Right):
          left = window_width_ - element->width;
        break;
        default:
          left = 0.0f;
        break;
      }

      element->object->set_alignment(top, left);
    }

    void Manager::mouse_eval(const unsigned int red, const unsigned int green,
                             const unsigned int blue){
      if(blue > 0u){
        const unsigned int id{red + green};

        switch(blue){
          case static_cast<unsigned int>(torero::gui::object::Menu):
            if(menu_.object){
              if(last_changed_id_ != id){
                menu_.object->restart_buttons();
                last_changed_id_ = id;
              }

              menu_.object->change_button_state(static_cast<torero::gui::menu::ButtonType>(id),
                                                torero::gui::button::Hover);
              last_changed_object_ = static_cast<unsigned int>(torero::gui::object::Menu);
            }
          break;
          case static_cast<unsigned int>(torero::gui::object::TitleBar):
            if(last_changed_id_ != id){
              title_bar_->restart_buttons();
              last_changed_id_ = id;
            }

            title_bar_->change_button_state(static_cast<torero::gui::title::ButtonType>(id),
                                            torero::gui::button::Hover);

            last_changed_object_ = static_cast<unsigned int>(torero::gui::object::TitleBar);
          break;
        }
      }else{
        switch(static_cast<unsigned int>(last_changed_object_)){
          case static_cast<unsigned int>(torero::gui::object::Menu):
            menu_.object->restart_buttons();
            last_changed_object_ = -1u;
            last_changed_id_ = -1u;
          break;
          case static_cast<unsigned int>(torero::gui::object::TitleBar):
            title_bar_->restart_buttons();
            last_changed_object_ = -1u;
            last_changed_id_ = -1u;
          break;
        }
      }
    }

    void Manager::mouse_motion(const SDL_MouseMotionEvent &event){
      if(is_clicked_)
        return;

      GLubyte color[3];
      frame_buffer_.read_pixel(color, event.x, event.y);

      const unsigned int red  {static_cast<unsigned int>(color[0])};
      const unsigned int green{static_cast<unsigned int>(color[1])};
      const unsigned int blue {static_cast<unsigned int>(color[2])};

      const float factor{to_float(event.x) / window_width_};

      if(speedometer_)
        speedometer_->rotate(factor);

      if(compass_)
        compass_->rotate(factor);

      mouse_eval(red, green, blue);
      //    std::cout << " R: " << red << " G: " << green
      //              << " B: " << blue << " A: " << alpha << std::endl;
    }

    void Manager::mouse_button(const SDL_MouseButtonEvent &event){
      GLubyte color[3];
      frame_buffer_.read_pixel(color, event.x, event.y);

      const unsigned int red  {static_cast<unsigned int>(color[0])};
      const unsigned int green{static_cast<unsigned int>(color[1])};
      const unsigned int blue {static_cast<unsigned int>(color[2])};

      if(blue > 0u){
        const unsigned int id{red + green};

        if(event.state == SDL_PRESSED){
          switch(blue){
            case static_cast<unsigned int>(torero::gui::object::Menu):
              if(menu_.object){
                menu_.object->change_button_state(static_cast<torero::gui::menu::ButtonType>(id),
                                                  torero::gui::button::Click);
                is_clicked_ = true;
              }
            break;
            case static_cast<unsigned int>(torero::gui::object::TitleBar):
              const torero::gui::title::ButtonType title_id =
                  static_cast<torero::gui::title::ButtonType>(id);
              title_bar_->change_button_state(title_id, torero::gui::button::Click);
              title_bar_->click_event(title_id, torero::gui::mouse::Click);
              is_clicked_ = true;
            break;
          }
        }else{
          is_clicked_ = false;
          mouse_eval(red, green, blue);

          switch(blue){
            case static_cast<unsigned int>(torero::gui::object::Menu):
              if(menu_.object)
                menu_.object->click_event(static_cast<torero::gui::menu::ButtonType>(id));
            break;
            case static_cast<unsigned int>(torero::gui::object::TitleBar):
              title_bar_->click_event(static_cast<torero::gui::title::ButtonType>(id),
                                      torero::gui::mouse::Release);
            break;
          }
        }
      }else if(event.state == SDL_RELEASED){
        is_clicked_ = false;
        mouse_eval(red, green, blue);
      }
    }
  }
}
