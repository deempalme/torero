#include "torero/core.h"
#include "torero/gui/cursors.h"
#include "torero/image/manager.h"

#include <SDL2/SDL_mouse.h>
// Image loader
#include "stb_image.h"

namespace torero {
  namespace gui {
    Cursors::Cursors(torero::Core *core) :
      core_(core),
      system_cursors_(),
      custom_cursors_(),
      actived_system_(true),
      actived_system_id_(torero::gui::Cursor::Normal),
      actived_id_()
    {
    }

    Cursors::~Cursors(){
      for(auto &cursor : system_cursors_)
        if(cursor.second) SDL_FreeCursor(cursor.second);

      for(auto &cursor : custom_cursors_)
        if(cursor.second) SDL_FreeCursor(cursor.second);
    }

    bool Cursors::change_cursor(const torero::gui::Cursor type){
      if(actived_system_ && actived_system_id_ == type)
        return true;

      auto found = system_cursors_.find(type);
      if(found != system_cursors_.end()){
        SDL_SetCursor(found->second);
      }else{
        SDL_Cursor *new_cursor{SDL_CreateSystemCursor(get_cursor_id(type))};
        if(!new_cursor){
          core_->sdl_error();
          return false;
        }
        system_cursors_[type] = new_cursor;
        SDL_SetCursor(new_cursor);
      }
      actived_system_ = true;
      actived_system_id_ = type;
      return true;
    }

    void Cursors::change_cursor(const std::string &path,
                                const int x_offset, const int y_offset){
      auto found = custom_cursors_.find(path);
      if(found != custom_cursors_.end()){
        SDL_SetCursor(found->second);
      }else{
        core_->image_manager().load_surface_concurrently(boost::bind(&gui::Cursors::ready, this,
                                                                     x_offset, y_offset, path, _1),
                                                         path, false);
      }
    }

    bool Cursors::delete_cursor(const torero::gui::Cursor type){
      if(type == torero::gui::Cursor::Normal)
        return false;

      auto found = system_cursors_.find(type);
      if(found != system_cursors_.end()){
        if(actived_system_ && actived_system_id_ == type){
          SDL_SetCursor(system_cursors_[torero::gui::Cursor::Normal]);
          actived_system_ = true;
          actived_system_id_ = torero::gui::Cursor::Normal;
        }
        SDL_FreeCursor(found->second);
        system_cursors_.erase(found);
        return true;
      }
      return false;
    }

    bool Cursors::delete_cursor(const std::string &path){
      auto found = custom_cursors_.find(path);
      if(found != custom_cursors_.end()){
        if(!actived_system_ && actived_id_ == path){
          SDL_SetCursor(system_cursors_[torero::gui::Cursor::Normal]);
          actived_system_ = true;
          actived_system_id_ = torero::gui::Cursor::Normal;
        }
        SDL_FreeCursor(found->second);
        custom_cursors_.erase(found);
        return true;
      }
      return false;
    }

    bool Cursors::show_cursor(const bool showing){
      if(showing){
        SDL_ShowCursor(SDL_ENABLE);
        return true;
      }else{
        SDL_ShowCursor(SDL_DISABLE);
        return false;
      }
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PRIVATES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //

    SDL_SystemCursor Cursors::get_cursor_id(const torero::gui::Cursor type){
      switch(type){
        case torero::gui::Cursor::Text:
        return SDL_SYSTEM_CURSOR_IBEAM;
        break;
        case torero::gui::Cursor::Wait:
        return SDL_SYSTEM_CURSOR_WAIT;
        break;
        case torero::gui::Cursor::CrossHair:
        return SDL_SYSTEM_CURSOR_CROSSHAIR;
        break;
        case torero::gui::Cursor::WaitArrow:
        return SDL_SYSTEM_CURSOR_WAITARROW;
        break;
        case torero::gui::Cursor::ArrowNWSE:
        return SDL_SYSTEM_CURSOR_SIZENWSE;
        break;
        case torero::gui::Cursor::ArrowNESW:
        return SDL_SYSTEM_CURSOR_SIZENESW;
        break;
        case torero::gui::Cursor::ArrowWE:
        return SDL_SYSTEM_CURSOR_SIZEWE;
        break;
        case torero::gui::Cursor::ArrowNS:
        return SDL_SYSTEM_CURSOR_SIZENS;
        break;
        case torero::gui::Cursor::ArrowALL:
        return SDL_SYSTEM_CURSOR_SIZEALL;
        break;
        case torero::gui::Cursor::Forbidden:
        return SDL_SYSTEM_CURSOR_NO;
        break;
        case torero::gui::Cursor::Hand:
        return SDL_SYSTEM_CURSOR_HAND;
        break;
        default:
        return SDL_SYSTEM_CURSOR_ARROW;
        break;
      }
    }

    void Cursors::ready(const int x_offset, const int y_offset,
                        const std::string path, torero::image::Surface *surface){

      SDL_Cursor *new_cursor{SDL_CreateColorCursor(surface->surface, x_offset, y_offset)};
      if(!new_cursor){
        core_->sdl_error();
        return;
      }
      custom_cursors_[path] = new_cursor;
      SDL_SetCursor(new_cursor);
      actived_system_ = false;
      actived_id_ = path;

      core_->image_manager().free(*surface);
      delete surface;
    }
  }
}
