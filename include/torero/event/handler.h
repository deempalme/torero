#ifndef TORERO_EVENT_HANDLER_H
#define TORERO_EVENT_HANDLER_H

#include "torero/definition/types.h"

// Boost
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

namespace torero {
  class Core;

  namespace event {
    class Handler
    {
    public:
      Handler(torero::Core *core);
      ~Handler();

      boost::signals2::connection connect_finger_event(
          boost::function<void (const SDL_TouchFingerEvent &)> callback_function );
      boost::signals2::connection connect_key_event(
          boost::function<void (const SDL_KeyboardEvent &)> callback_function );
      boost::signals2::connection connect_mouse_button(
          boost::function<void (const SDL_MouseButtonEvent &)> callback_function );
      boost::signals2::connection connect_mouse_motion(
          boost::function<void (const SDL_MouseMotionEvent &)> callback_function );
      boost::signals2::connection connect_mouse_wheel(
          boost::function<void (const SDL_MouseWheelEvent &)> callback_function );
      boost::signals2::connection connect_window_event(
          boost::function<void (const SDL_WindowEvent &)> callback_function );

      bool disconnect_event(boost::signals2::connection &connection);

      void poll_events();
      /*
     * ### Waiting for triggering events
     *
     * Waits until an event is triggered (mouse, keyboard, etc). This will pause the program's
     * execution until an event is triggered.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
      void wait_for_events();
      /*
     * ### Waiting for triggering events or until time runs out
     *
     * Waits until an event is triggered (mouse, keyboard, etc) or until the timeout
     * (in seconds) has passed. This will pause the program's execution until an event
     * is triggered or the time is over.
     *
     * **Arguments**
     * {const int} timeout = Waiting time in milliseconds.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
      void wait_for_events(const int timeout);

    protected:
      void finger_event(const SDL_TouchFingerEvent &event);
      void key_event(const SDL_KeyboardEvent &event);
      void mouse_button(const SDL_MouseButtonEvent &event);
      void mouse_motion(const SDL_MouseMotionEvent &event);
      void mouse_wheel(const SDL_MouseWheelEvent &event);
      void window_event(const SDL_WindowEvent &event);

      SDL_Event event_;
      SDL_Rect display_properties_, window_properties_, initial_window_properties_;
      bool closing_, hidden_;
      bool mouse_active_, mouse_blocked_;
      bool mouse_left_active_, mouse_middle_active_, mouse_right_active_;
      bool has_changed_;

    private:
      void analyze_event();
      void key_hold(const int key, int mods);
      void key_press(const int key, const int mods);
      void key_release(const int key, int mods);
      void mouse_move(const double position_x, const double position_y);
      void mouse_press(const int button, const int mods);
      void mouse_release(const int button, const int mods);
      void mouse_wheel(const double offset_x, const double offset_y);

      torero::Core *const core_;

      bool keyboard_active_;

      int key_modifier_;
      int previous_x_, previous_y_;

      // signals
      boost::signals2::signal<void (const SDL_TouchFingerEvent &)> signal_finger_event_;
      boost::signals2::signal<void (const SDL_KeyboardEvent &)> signal_key_event_;
      boost::signals2::signal<void (const SDL_MouseButtonEvent &)> signal_mouse_button_;
      boost::signals2::signal<void (const SDL_MouseMotionEvent &)> signal_mouse_motion_;
      boost::signals2::signal<void (const SDL_MouseWheelEvent &)> signal_mouse_wheel_;
      boost::signals2::signal<void (const SDL_WindowEvent &)> signal_window_event_;
    };
  }
}
#endif // TORERO_EVENT_HANDLER_H
