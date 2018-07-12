#include "torero/gui_manager.h"
#include "torero/core.h"
#include "torero/model_manager.h"
#include "torero/text_manager.h"
// Image loader
#include "stb_image.h"

namespace torero {
  GUIManager::GUIManager(Core *core, ModelManager *model_manager, TextManager *text_manager,
                         const std::string folder_address) :
    has_changed_(core->screen_changer()),
    blocked_(core->camera_blocker()),
    folder_address_(folder_address),

    core_(core),
    cubemap_(model_manager->get_cubemap()),
    text_manager_(text_manager),

    pbr_shader_(new Shader("resources/shaders/gui_PBR.vert",
                           "resources/shaders/gui_PBR.frag")),
    u_pbr_projection_(pbr_shader_->uniform_location("u_projection")),
    u_pbr_view_(pbr_shader_->uniform_location("u_view")),
    u_pbr_camera_(pbr_shader_->uniform_location("u_camera")),

    painter_shader_(new Shader("resources/shaders/gui_3D.vert",
                               "resources/shaders/gui_3D.frag")),
    u_projection_(painter_shader_->uniform_location("u_projection")),
    u_model_(painter_shader_->uniform_location("u_model")),

    mouse_shader_(new Shader("resources/shaders/gui_mouse.vert",
                             "resources/shaders/gui_mouse.frag")),
    u_m_window_size_(mouse_shader_->uniform_location("u_window_size")),

    plane_shader_(new Shader("resources/shaders/gui_2D.vert",
                             "resources/shaders/gui_2D.frag")),
    u_p_window_size_(plane_shader_->uniform_location("u_window_size")),

    text_shader_(new Shader("resources/shaders/gui_text.vert",
                            "resources/shaders/gui_text.frag",
                            "resources/shaders/gui_text.geom")),
    u_t_projection_(text_shader_->uniform_location("u_projection")),

    ortho_(algebraica::mat4f::from_ortho(-kOrthoWidth, kOrthoWidth,
                                         -kOrthoHeight, kOrthoHeight, -1, 1)),
    window_width_(kDefaultWidth),
    window_height_(kDefaultHeight),
    frame_buffer_(true),
    render_buffer_(true),
    compass_(nullptr),
    compass_visibility_(false),
    menu_(),
    gui_2d_albedo_(nullptr),
    speedometer_(nullptr),
    speedometer_visibility_(false),
    title_bar_(new GUITitleBar(core_, plane_shader_, mouse_shader_, has_changed_, blocked_)),
    title_bar_visibility_(true),
    last_changed_object_(-1u),
    last_changed_id_(-1u),
    is_clicked_(false),
    signal_draw_all_(core->syncronize(torero::Order::GUI)
                     ->connect(boost::bind(&GUIManager::draw_all, this))),
    signal_updated_camera_(core->signal_updated_camera()
                           ->connect(boost::bind(&GUIManager::update_camera, this))),
    signal_resized_screen_(core->signal_resized_screen()
                           ->connect(boost::bind(&GUIManager::resized_screen, this, _1, _2))),
    signal_mouse_moved_(core->signal_moved_mouse()
                           ->connect(boost::bind(&GUIManager::mouse_move, this, _1, _2))),
    signal_clicked_mouse_(core->signal_clicked_mouse()
                           ->connect(boost::bind(&GUIManager::mouse_event, this, _1, _2, _3,
                                                 torero::Mouse::Click))),
    signal_released_mouse_(core->signal_released_mouse()
                           ->connect(boost::bind(&GUIManager::mouse_event, this, _1, _2, _3,
                                                 torero::Mouse::Release)))
  {
    torero::ImageFile gui_main_albedo;
    const bool checked{check_folder()};

    if(pbr_shader_->use()){
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_irradiance"), 0);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_prefilter"), 1);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_brdfLUT"), 2);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_albedo"), 3);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_normal"), 4);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_metallic"), 5);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_roughness"), 6);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_ao"), 7);
      pbr_shader_->set_value(pbr_shader_->uniform_location("u_emission"), 8);

      pbr_shader_->set_value(u_pbr_projection_, ortho_);
      update_camera();
    }else
      std::cout << pbr_shader_->error_log() << std::endl;

    if(plane_shader_->use()){
      if(checked){
        stbi_set_flip_vertically_on_load(true);

        gui_main_albedo.data = stbi_load(std::string(folder_address_ +
                                                     "two_dimensional_elements/albedo.png").c_str(),
                                         &gui_main_albedo.width, &gui_main_albedo.height,
                                         &gui_main_albedo.components_size, 0);

        if(gui_main_albedo.data)
          gui_2d_albedo_ = new Texture(4, 0.0f, &gui_main_albedo, false);

        plane_shader_->set_value(plane_shader_->uniform_location("u_albedo"), 4);
        plane_shader_->set_value(plane_shader_->uniform_location("u_texture_size"),
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
        mouse_shader_->set_value(mouse_shader_->uniform_location("u_albedo"), 4);
        mouse_shader_->set_value(mouse_shader_->uniform_location("u_texture_size"),
                                 algebraica::vec2f(static_cast<float>(gui_main_albedo.width),
                                                   static_cast<float>(gui_main_albedo.height)));
        mouse_shader_->set_value(u_m_window_size_,
                                 algebraica::vec2f(window_width_, window_height_));

        render_buffer_.bind();
        render_buffer_.storage(static_cast<GLint>(window_width_),
                               static_cast<GLint>(window_height_), GL_RGBA8);
        frame_buffer_.bind();
        render_buffer_.attach_to_framebuffer(frame_buffer_.attachment());

        render_buffer_.release();
        frame_buffer_.release();

      }
    }else
      std::cout << mouse_shader_->error_log() << std::endl;

    if(text_shader_->use()){
      if(checked){
        iceland_id_ = text_manager_->add_font("resources/fonts/Iceland.png",
                                              "resources/fonts/Iceland.fnt");
        text_shader_->set_value(text_shader_->uniform_location("u_font_atlas"), 9);
        text_shader_->set_value(u_t_projection_, ortho_);
      }
    }else
      std::cout << text_shader_->error_log() << std::endl;
  }

  GUIManager::~GUIManager(){
    if(speedometer_signal_.connected())
      speedometer_signal_.disconnect();

    if(compass_rotation_signal_.connected())
      compass_rotation_signal_.disconnect();

    if(compass_pedals_signal_.connected())
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

  void GUIManager::menu_visibility(const bool visibility){
    if(!menu_.object && visibility){
      menu_.object = new GUIMenu(core_, plane_shader_, mouse_shader_, has_changed_, blocked_);
      calculate_alignment(&menu_);
    }
    menu_.visibility = visibility;
  }

  bool GUIManager::menu_offset(const int top, const int left){
    if(menu_.object){
      menu_.object->set_offset(top, left);
      return true;
    }else
      return false;
  }

  bool GUIManager::menu_alignment(const torero::Alignment vertical,
                                  const torero::Alignment horizontal){
    if(menu_.object){
      menu_.vertical = vertical;
      menu_.horizontal = horizontal;

      calculate_alignment(&menu_);
      return true;
    }else
      return false;
  }

  bool GUIManager::menu_change_button_state(const torero::Menu::ButtonType button_id,
                                            const torero::Button::State state){
    if(menu_.object){
      menu_.object->change_button_state(button_id, state);
      return true;
    }else
      return false;
  }

  void GUIManager::title_bar_visibility(const bool visibility){
    title_bar_visibility_ = visibility;
  }

  bool GUIManager::compass_add(){
    if(compass_){
      compass_visibility_ = true;
      return false;
    }else{
      compass_ = new GUICompass(core_, pbr_shader_, nullptr, text_shader_,
                                text_manager_->font_characters(iceland_id_),
                                "resources/models3D/compass", has_changed_);
      core_->multithread_add_process(boost::bind(&GUICompass::run, compass_),
                                     boost::bind(&GUICompass::ready, compass_),
                                     boost::bind(&GUICompass::is_ready, compass_));
      compass_visibility_ = true;
    }

    return true;
  }

  bool GUIManager::compass_connect_rotation(boost::signals2::signal<void (float, float)> *signal){
    if(compass_){
      if(compass_rotation_signal_.connected())
        compass_rotation_signal_.disconnect();
      compass_rotation_signal_ = signal->connect(boost::bind(&GUIManager::compass_update_rotation,
                                                             this, _1, _2));
      return true;
    }else
      return false;
  }

  bool GUIManager::compass_connect_pedals(boost::signals2::signal<void (float, float, float,
                                                                        std::string)> *signal){
    if(compass_){
      if(compass_pedals_signal_.connected())
        compass_pedals_signal_.disconnect();
      compass_pedals_signal_ = signal->connect(boost::bind(&GUIManager::compass_update_pedals,
                                                             this, _1, _2, _3, _4));
      return true;
    }else
      return false;
  }

  bool GUIManager::compass_disconnect_rotation(){
    if(compass_){
      if(compass_rotation_signal_.connected())
        compass_rotation_signal_.disconnect();
      return true;
    }else
      return false;
  }

  bool GUIManager::compass_disconnect_pedals(){
    if(compass_){
      if(compass_pedals_signal_.connected())
        compass_pedals_signal_.disconnect();
      return true;
    }else
      return false;
  }

  bool GUIManager::compass_update_rotation(const float compass_angle, const float steering_angle){
    if(compass_){
      compass_->update_rotation(compass_angle, steering_angle);
      *has_changed_ = true;
      return true;
    }else
      return false;
  }

  bool GUIManager::compass_update_pedals(const float brake, const float accelerator,
                                         const float clutch, const std::string gear_position){
    if(compass_){
      compass_->update_pedals(brake, accelerator, clutch, gear_position);
      *has_changed_ = true;
      return true;
    }else
      return false;
  }

  void GUIManager::compass_visibility(const bool visible){
    if(visible && !compass_)
      compass_add();
    else
      compass_visibility_ = visible;
  }

  bool GUIManager::compass_delete(){
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

  bool GUIManager::speedometer_add(){
    if(speedometer_)
      return false;
    else{
      speedometer_ = new GUISpeedometer(core_, pbr_shader_, nullptr, text_shader_,
                                        text_manager_->font_characters(iceland_id_),
                                        "resources/models3D/speedo", has_changed_);
      core_->multithread_add_process(boost::bind(&GUISpeedometer::run, speedometer_),
                                     boost::bind(&GUISpeedometer::ready, speedometer_),
                                     boost::bind(&GUISpeedometer::is_ready, speedometer_));
      speedometer_visibility_ = true;
    }

    return true;
  }

  bool GUIManager::speedometer_connect(boost::signals2::signal<void (float, float)> *signal){
    if(speedometer_){
      if(speedometer_signal_.connected())
        speedometer_signal_.disconnect();
      speedometer_signal_ = signal->connect(boost::bind(&GUIManager::speedometer_update, this, _1, _2));
      return true;
    }else
      return false;
  }

  bool GUIManager::speedometer_disconnect(){
    if(speedometer_){
      if(speedometer_signal_.connected())
        speedometer_signal_.disconnect();
      return true;
    }else
      return false;
  }

  bool GUIManager::speedometer_update(const float speed, const float rpm){
    if(speedometer_){
      speedometer_->update(speed, rpm);
      *has_changed_ = true;
      return true;
    }else
      return false;
  }

  void GUIManager::speedometer_visibility(const bool visible){
    if(visible && !speedometer_){
      speedometer_add();
    }else
      speedometer_visibility_ = visible;
  }

  bool GUIManager::speedometer_delete(){
    if(speedometer_){
      if(speedometer_signal_.connected())
        speedometer_signal_.disconnect();

      delete speedometer_;

      speedometer_visibility_ = false;
      return true;
    }else
      return false;
  }

  void GUIManager::draw_all(){
    if(pbr_shader_->use()){
      cubemap_->bind_reflectance();
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
      gui_2d_albedo_->use();
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

  bool GUIManager::check_folder(){
    if(folder_address_.front() != '/') folder_address_ = "/" + folder_address_;
    if(folder_address_.back() != '/') folder_address_ += "/";

    std::string first_path(folder_address_);

    if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
      folder_address_ = boost::filesystem::current_path().string() + folder_address_;

      if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
        core_->message_handler("*** GUI manager: ***\n The folder: " + first_path +
                               " was not found.\n  Neither: " + folder_address_ + "\n",
                               torero::Message::Error);
        return false;
      }
    }
    return true;
  }

  void GUIManager::update_camera(){
    pbr_shader_->use();
    pbr_shader_->set_value(u_pbr_view_, core_->camera_matrix_view());
    pbr_shader_->set_value(u_pbr_camera_, core_->camera_relative_position());
  }

  void GUIManager::resized_screen(const int width, const int height){
    window_width_ = static_cast<float>(width);
    window_height_ = static_cast<float>(height);

    plane_shader_->use();
    plane_shader_->set_value(u_p_window_size_, algebraica::vec2f(window_width_, window_height_));

    mouse_shader_->use();
    mouse_shader_->set_value(u_m_window_size_, algebraica::vec2f(window_width_, window_height_));

    title_bar_->set_buffer();

    render_buffer_.bind();
    render_buffer_.storage(width, height, GL_RGBA8);
    render_buffer_.release();

    const float height_ratio {kOrthoWidth / (window_width_/window_height_)};
    ortho_.ortho(-kOrthoWidth, kOrthoWidth, -height_ratio, height_ratio, -1, 1);

    pbr_shader_->use();
    pbr_shader_->set_value(u_pbr_projection_, ortho_);

    text_shader_->use();
    text_shader_->set_value(u_t_projection_, ortho_);

    calculate_alignment(&menu_);
  }

  void GUIManager::calculate_alignment(auto *element){
    float top, left;

    switch(static_cast<unsigned int>(element->vertical)){
    case static_cast<unsigned int>(torero::Alignment::Center):
      top = (window_height_ - element->height) / 2.0f;
    break;
    case static_cast<unsigned int>(torero::Alignment::Bottom):
      top = window_height_ - element->height;
    break;
    default:
      top = 0.0f;
    break;
    }

    switch(static_cast<unsigned int>(element->horizontal)){
    case static_cast<unsigned int>(torero::Alignment::Center):
      left = (window_width_ - element->width) / 2.0f;
    break;
    case static_cast<unsigned int>(torero::Alignment::Right):
      left = window_width_ - element->width;
    break;
    default:
      left = 0.0f;
    break;
    }

    element->object->set_alignment(top, left);
  }

  void GUIManager::mouse_eval(const unsigned int red, const unsigned int green,
                              const unsigned int blue){
    if(blue > 0u){
      const unsigned int id{red + green};

      switch(blue){
      case static_cast<unsigned int>(torero::GUIid::Menu):
        if(menu_.object){
          if(last_changed_id_ != id){
            menu_.object->restart_buttons();
            last_changed_id_ = id;
          }

          menu_.object->change_button_state(static_cast<torero::Menu::ButtonType>(id),
                                            torero::Button::Hover);
          last_changed_object_ = static_cast<unsigned int>(torero::GUIid::Menu);
        }
      break;
      case static_cast<unsigned int>(torero::GUIid::TitleBar):
        if(last_changed_id_ != id){
          title_bar_->restart_buttons();
          last_changed_id_ = id;
        }

        title_bar_->change_button_state(static_cast<torero::Title::ButtonType>(id),
                                        torero::Button::Hover);

        last_changed_object_ = static_cast<unsigned int>(torero::GUIid::TitleBar);
      break;
      }
    }else{
      switch(static_cast<unsigned int>(last_changed_object_)){
      case static_cast<unsigned int>(torero::GUIid::Menu):
        menu_.object->restart_buttons();
        last_changed_object_ = -1u;
        last_changed_id_ = -1u;
      break;
      case static_cast<unsigned int>(torero::GUIid::TitleBar):
        title_bar_->restart_buttons();
        last_changed_object_ = -1u;
        last_changed_id_ = -1u;
      break;
      }
    }
  }

  void GUIManager::mouse_move(int x, int y){
    if(is_clicked_)
      return;

    GLubyte color[3];
    frame_buffer_.read_pixel(color, x, y);

    const unsigned int red  {static_cast<unsigned int>(color[0])};
    const unsigned int green{static_cast<unsigned int>(color[1])};
    const unsigned int blue {static_cast<unsigned int>(color[2])};

    const float factor{ToFloat(x) / window_width_};

    if(speedometer_)
      speedometer_->rotate(factor);

    if(compass_)
      compass_->rotate(factor);

    mouse_eval(red, green, blue);
//    std::cout << " R: " << red << " G: " << green
//              << " B: " << blue << " A: " << alpha << std::endl;
  }

  void GUIManager::mouse_event(int x, int y, int /*mode*/, torero::Mouse::Event state){
    GLubyte color[3];
    frame_buffer_.read_pixel(color, x, y);

    const unsigned int red  {static_cast<unsigned int>(color[0])};
    const unsigned int green{static_cast<unsigned int>(color[1])};
    const unsigned int blue {static_cast<unsigned int>(color[2])};

    if(blue > 0u){
      const unsigned int id{red + green};

      if(state == torero::Mouse::Click){
        switch(blue){
        case static_cast<unsigned int>(torero::GUIid::Menu):
          if(menu_.object){
            menu_.object->change_button_state(static_cast<torero::Menu::ButtonType>(id),
                                              torero::Button::Click);
            is_clicked_ = true;
          }
        break;
        case static_cast<unsigned int>(torero::GUIid::TitleBar):
          const torero::Title::ButtonType title_id =
              static_cast<torero::Title::ButtonType>(id);
          title_bar_->change_button_state(title_id, torero::Button::Click);
          title_bar_->click_event(title_id, torero::Mouse::Click);
          is_clicked_ = true;
        break;
        }
      }else{
        is_clicked_ = false;
        mouse_eval(red, green, blue);

        switch(blue){
        case static_cast<unsigned int>(torero::GUIid::Menu):
          if(menu_.object)
            menu_.object->click_event(static_cast<torero::Menu::ButtonType>(id));
        break;
        case static_cast<unsigned int>(torero::GUIid::TitleBar):
            title_bar_->click_event(static_cast<torero::Title::ButtonType>(id),
                                    torero::Mouse::Release);
        break;
        }
      }
    }else if(state == torero::Mouse::Release){
      is_clicked_ = false;
      mouse_eval(red, green, blue);
    }
  }
}
