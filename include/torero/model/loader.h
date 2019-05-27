#ifndef TORERO_MODEL_LOADER_H
#define TORERO_MODEL_LOADER_H

#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/gl/texture.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace model {
    class Loader
    {
    public:
      explicit Loader(const std::string folder_address, torero::gl::Shader *shader_program,
                      torero::Core *core);
      explicit Loader(const torero::model::Models model, torero::gl::Shader *shader_program,
                      torero::Core *core);
      ~Loader();

      void pre_drawing();
      void draw();
      void post_drawing();

      void run();
      void ready();
      bool is_ready();

    private:
      std::string folder_address_;
      bool folder_exists_, is_ready_, is_loaded_;

      torero::Core *core_;
      torero::gl::Shader *shader_;
      torero::gl::Buffer *buffer_;

      GLsizei data_size_;
      std::vector<torero::ComplexShaderData> buffer_data_;
      torero::image::File albedo_, normal_, pbr_emission_;
      torero::gl::Texture *t_albedo_, *t_normal_, *t_pbr_emission_;

      boost::mutex protector_;

      bool error_;
      std::string error_log_;
    };
  }
}
#endif // TORERO_MODEL_LOADER_H
