#ifndef TORERO_GUI_LOADER_H
#define TORERO_GUI_LOADER_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"

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

  class GUILoader
  {
  public:
    explicit GUILoader(const std::string folder_address, Shader *shader, Core * core);
    ~GUILoader();

    void draw();

  private:
    bool check_folder();
    void run();
    void model_ready();

    std::string folder_address_;
    bool is_ready_, is_loaded_;

    Core *core_;
    Shader *shader_;
    Buffer *buffer_;
    GLint i_position_, i_uv_, i_normal_, i_tangent_, i_bitangent_;

    GLsizei data_size_;
    std::vector<torero::ComplexShaderData> buffer_data_;

    boost::mutex protector_;
    boost::thread runner_;

    bool error_;
    std::string error_log_;
  };
}

#endif // TORERO_GUI_LOADER_H
