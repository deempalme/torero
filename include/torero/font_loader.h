#ifndef TORERO_FONT_LOADER_H
#define TORERO_FONT_LOADER_H

#include "glad/glad.h"

#include "torero/definitions.h"
#include "torero/texture.h"
#include "torero/types.h"
// Boost
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Toreo {
  class Core;

  class FontLoader
  {
  public:
    explicit FontLoader(const std::string font_distance_path,
                        const std::string font_info_path,
                        Core *core);
    ~FontLoader();

    void use();
    const std::vector<Visualizer::FontCharacter> *characters() const;

    void run();
    void ready();
    const bool is_ready();

  private:
    bool check_path(std::string *path);
    void load_font();

    std::string font_distance_path_, font_info_path_;
    bool file_exists_, is_ready_, is_loaded_;

    Core *core_;
    Visualizer::ImageFile distance_;
    Texture *t_distance_;
    std::vector<Visualizer::FontCharacter> characters_;

    boost::mutex protector_;

    bool error_;
    std::string error_log_;
  };
}
#endif // TORERO_FONT_LOADER_H
