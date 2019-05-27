#ifndef TORERO_GUI_CURSORS_H
#define TORERO_GUI_CURSORS_H

#include "torero/definition/types.h"

#include <map>
#include <SDL2/SDL.h>
#include <string>

namespace torero {
  class Core;

  namespace gui {
    class Cursors
    {
    public:
      Cursors(torero::Core *core);
      ~Cursors();

      bool change_cursor(const torero::gui::Cursor type);
      void change_cursor(const std::string &path, const int x_offset = 0, const int y_offset = 0);

      bool delete_cursor(const torero::gui::Cursor type);
      bool delete_cursor(const std::string &path);

      bool show_cursor(const bool showing = true);

    private:
      SDL_SystemCursor get_cursor_id(const torero::gui::Cursor type);
      void ready(const int x_offset, const int y_offset, const std::string path,
                 torero::image::Surface *surface);


      torero::Core *core_;
      std::map<const torero::gui::Cursor, SDL_Cursor*> system_cursors_;
      std::map<const std::string, SDL_Cursor*> custom_cursors_;

      bool actived_system_;
      torero::gui::Cursor actived_system_id_;
      std::string actived_id_;
    };
  }
}
#endif // TORERO_GUI_CURSORS_H
