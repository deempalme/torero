#ifndef TORERO_GUI_LOADER_H
#define TORERO_GUI_LOADER_H

#include "glad/glad.h"

#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/definition/types.h"

#include "algebraica/algebraica.h"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace gui {
    class Loader
    {
    public:
      explicit Loader(const std::string folder_address, torero::gl::Shader *shader,
                      torero::Core * core);
      ~Loader();

      void draw();

    private:
      bool check_folder();
      void run();
      void model_ready();

      std::string folder_address_;
      bool is_ready_, is_loaded_;

      torero::Core *core_;
      torero::gl::Shader *shader_;
      torero::gl::Buffer *buffer_;

      GLsizei data_size_;
      std::vector<torero::ComplexShaderData> buffer_data_;

      boost::mutex protector_;
      boost::thread runner_;

      bool error_;
      std::string error_log_;
    };
  }
}
#endif // TORERO_GUI_LOADER_H
