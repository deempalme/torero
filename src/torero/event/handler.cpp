#include "torero/camera/view.h"
#include "torero/core.h"
#include "torero/event/handler.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"
// SDL library
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

namespace torero {
  namespace event {
    Handler::Handler(torero::Core *core) :
      event_(),
      display_properties_{ 0, 0, torero::k_default_width, torero::k_default_height},
      window_properties_(display_properties_),
      initial_window_properties_(display_properties_),
      closing_(false),
      hidden_(true),
      core_(core),
      keyboard_active_(false),
      mouse_active_(false),
      mouse_blocked_(false),
      mouse_left_active_(false),
      mouse_middle_active_(false),
      mouse_right_active_(false),
      has_changed_(true),
      signal_finger_event_(),
      signal_key_event_(),
      signal_mouse_button_(),
      signal_mouse_motion_(),
      signal_mouse_wheel_(),
      signal_window_event_()
    {
    }

    Handler::~Handler(){
    }

    boost::signals2::connection Handler::connect_finger_event(
        boost::function<void (const SDL_TouchFingerEvent &)> callback_function ){
      return signal_finger_event_.connect(callback_function);
    }

    boost::signals2::connection Handler::connect_key_event(
        boost::function<void (const SDL_KeyboardEvent &)> callback_function ){
      return signal_key_event_.connect(callback_function);
    }

    boost::signals2::connection Handler::connect_mouse_button(
        boost::function<void (const SDL_MouseButtonEvent &)> callback_function ){
      return signal_mouse_button_.connect(callback_function);
    }

    boost::signals2::connection Handler::connect_mouse_motion(
        boost::function<void (const SDL_MouseMotionEvent &)> callback_function ){
      return signal_mouse_motion_.connect(callback_function);
    }

    boost::signals2::connection Handler::connect_mouse_wheel(
        boost::function<void (const SDL_MouseWheelEvent &)> callback_function ){
      return signal_mouse_wheel_.connect(callback_function);
    }

    boost::signals2::connection Handler::connect_window_event(
        boost::function<void (const SDL_WindowEvent &)> callback_function ){
      return signal_window_event_.connect(callback_function);
    }

    bool Handler::disconnect_event(boost::signals2::connection &connection){
      if(!connection.connected())
        return false;

      connection.disconnect();
      return true;
    }

    void Handler::poll_events(){
      while(SDL_PollEvent(&event_) > 0) analyze_event();
    }

    void Handler::wait_for_events(){
      SDL_WaitEvent(&event_);
      poll_events();
    }

    void Handler::wait_for_events(const int timeout){
      SDL_WaitEventTimeout(&event_, timeout);
      poll_events();
    }

    // ::::::::::::::::::::::::::::::::::: Protected functions :::::::::::::::::::::::::::::::::::

    void Handler::finger_event(const SDL_TouchFingerEvent &event){
      signal_finger_event_(event);
    }

    void Handler::key_event(const SDL_KeyboardEvent &event){
      const bool key_up{event.type == SDL_KEYUP};
      if(keyboard_active_){

      }
      key_modifier_ = event.keysym.mod & torero::keyboard::NOT_NONE;

      switch(event.keysym.sym){
        case SDLK_DOWN:
        break;
        case SDLK_UP:
        break;
        case SDLK_LEFT:
        break;
        case SDLK_RIGHT:
        break;
        case SDLK_PLUS:
          if(key_modifier_ == torero::keyboard::NONE && key_up){
            core_->camera().zoom(torero::camera::Zoom::In);
            torero::terminal::Printer::attention("zooming in");
          }else if(key_modifier_ & torero::keyboard::CTRL && key_up){
            core_->camera().zoom(torero::camera::Zoom::In);
            core_->camera().zoom(torero::camera::Zoom::In);
            torero::terminal::Printer::attention("extra zooming in");
          }
        break;
        case SDLK_MINUS:
          if(key_modifier_ == torero::keyboard::NONE && key_up){
            core_->camera().zoom(torero::camera::Zoom::Out);
            torero::terminal::Printer::attention("zooming out");
          }else if(key_modifier_ & torero::keyboard::CTRL && key_up){
            core_->camera().zoom(torero::camera::Zoom::Out);
            core_->camera().zoom(torero::camera::Zoom::Out);
            torero::terminal::Printer::attention("extra zooming out");
          }
        break;
        case SDLK_c:
          if(key_modifier_ & torero::keyboard::CTRL && key_up){
            core_->close();
            torero::terminal::Printer::attention("closing");
          }
        break;
        case SDLK_t:
          if(key_modifier_ == torero::keyboard::NONE && key_up){
            core_->camera().top_view();
            torero::terminal::Printer::attention("changed to top view");
          }
        break;
        case SDLK_i:
          if(key_modifier_ == torero::keyboard::NONE && key_up){
            core_->camera().isometric_view();
            torero::terminal::Printer::attention("changed to isometric view");
          }
        break;
        default: break;
      }
      signal_key_event_(event);
    }

    void Handler::mouse_button(const SDL_MouseButtonEvent &event){
      mouse_left_active_ = mouse_middle_active_ = mouse_right_active_ = false;

      if(!(mouse_active_ = (event.state == SDL_PRESSED)))
        core_->change_cursor(torero::gui::Cursor::Normal);

      switch(event.button){
        case SDL_BUTTON_LEFT:
          mouse_left_active_ = true;

          if(!mouse_blocked_ && mouse_active_){
            core_->change_cursor(torero::gui::Cursor::ArrowALL);
          }
        break;
        case SDL_BUTTON_MIDDLE:
          mouse_middle_active_ = true;
        break;
        case SDL_BUTTON_RIGHT:
          mouse_right_active_ = true;

          if(!mouse_blocked_ && mouse_active_){
            core_->change_cursor(torero::gui::Cursor::ArrowALL);
          }
        break;
        default: break;
      }
      signal_mouse_button_(event);
    }

    void Handler::mouse_motion(const SDL_MouseMotionEvent &event){
      if(mouse_active_ && !mouse_blocked_){
        if(mouse_left_active_ || mouse_right_active_){
          core_->camera().mouse_to_camera(event.xrel, event.yrel, mouse_left_active_);
          has_changed_ = true;
        }

        signal_mouse_motion_(event);
      }
    }

    void Handler::mouse_wheel(const SDL_MouseWheelEvent &event){
      if(!mouse_blocked_){
        core_->camera().zoom(event.y > 0 ? torero::camera::Zoom::In : torero::camera::Zoom::Out);
        has_changed_ = true;
      }

      signal_mouse_wheel_(event);
    }

    void Handler::window_event(const SDL_WindowEvent &event){
      switch(event.event){
        case SDL_WINDOWEVENT_CLOSE:
          closing_ = true;
        break;
        case SDL_WINDOWEVENT_SHOWN:
          hidden_ = false;
        break;
        case SDL_WINDOWEVENT_HIDDEN:
          hidden_ = true;
        break;
        case SDL_WINDOWEVENT_MINIMIZED:
          hidden_ = true;
        break;
        case SDL_WINDOWEVENT_MAXIMIZED:
          hidden_ = false;
        break;
        case SDL_WINDOWEVENT_RESTORED:
          hidden_ = false;
        break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          if(event.data1 > 0 && event.data2 > 0) hidden_ = false;

          initial_window_properties_.w = window_properties_.w = event.data1;
          initial_window_properties_.h = window_properties_.h = event.data2;

          glViewport(0, 0, window_properties_.w, window_properties_.h);
          core_->camera().resolution(window_properties_.w, window_properties_.h,
                                     window_properties_.w, window_properties_.h);
        break;
        case SDL_WINDOWEVENT_MOVED:
          window_properties_.x = event.data1;
          window_properties_.y = event.data2;
        break;
        default:
        break;
      }
      signal_window_event_(event);
    }

    // :::::::::::::::::::::::::::::::::::: private functions ::::::::::::::::::::::::::::::::::::

    void Handler::analyze_event(){
      switch(event_.type){
        case SDL_QUIT:
          closing_ = true;
        break;
        case SDL_MOUSEBUTTONDOWN:
          mouse_button(event_.button);
        break;
        case SDL_MOUSEBUTTONUP:
          mouse_button(event_.button);
        break;
        case SDL_MOUSEMOTION:
          mouse_motion(event_.motion);
        break;
        case SDL_MOUSEWHEEL:
          mouse_wheel(event_.wheel);
        break;
        case SDL_KEYDOWN:
          key_event(event_.key);
        break;
        case SDL_KEYUP:
          key_event(event_.key);
        break;
        case SDL_FINGERDOWN:
          finger_event(event_.tfinger);
        break;
        case SDL_FINGERUP:
          finger_event(event_.tfinger);
        break;
        case SDL_FINGERMOTION:
          finger_event(event_.tfinger);
        break;
        case SDL_WINDOWEVENT:
          window_event(event_.window);
        break;
      }
    }
  }
}
