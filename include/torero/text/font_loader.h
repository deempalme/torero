#ifndef TORERO_TEXT_FONT_LOADER_H
#define TORERO_TEXT_FONT_LOADER_H

#include "glad/glad.h"

#include "torero/gl/texture.h"
#include "torero/definition/types.h"
// Boost
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace text {
    class FontLoader
    {
    public:
      explicit FontLoader(const std::string font_distance_path,
                          const std::string font_info_path,
                          torero::Core *core);
      ~FontLoader();

      void use();
      const std::vector<torero::text::FontCharacter> *characters() const;

      void run();
      void ready();
      bool is_ready();

    private:
      void load_font();

      std::string font_distance_path_, font_info_path_;
      bool file_exists_, is_ready_, is_loaded_;

      torero::Core *core_;
      torero::image::File distance_;
      torero::gl::Texture *t_distance_;
      std::vector<torero::text::FontCharacter> characters_;

      boost::mutex protector_;

      bool error_;
      std::string error_log_;
    };
  }
}
#endif // TORERO_TEXT_FONT_LOADER_H
