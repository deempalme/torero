#ifndef TORERO_FONT_LOADER_H
#define TORERO_FONT_LOADER_H

#include "glad/glad.h"

#include "torero/definitions.h"
#include "torero/texture.h"
#include "torero/types.h"

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

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

  private:
    bool check_path(std::string *path);
    void run();
    void ready();

    std::string font_distance_path_, font_info_path_;
    bool is_ready_, is_loaded_;

    Core *core_;
    Visualizer::ImageFile distance_;
    Texture *t_distance_;
    std::vector<Visualizer::FontCharacter> characters_;

    boost::mutex protector_;
    boost::thread runner_;

    int padding_left_, padding_top_, padding_right_, padding_bottom_;
    int space_x_, space_y_, scale_w_, scale_h_;

    bool error_;
    std::string error_log_;
  };
}
#endif // TORERO_FONT_LOADER_H
