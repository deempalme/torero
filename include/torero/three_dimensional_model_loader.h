#ifndef TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H
#define TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/texture.h"
#include "torero/type_definitions.h"
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

  class ThreeDimensionalModelLoader
  {
  public:
    explicit ThreeDimensionalModelLoader(const std::string folder_address,
                                         Shader *shader_program, Core *core);
    explicit ThreeDimensionalModelLoader(const torero::Models model,
                                         Shader *shader_program, Core *core);
    ~ThreeDimensionalModelLoader();

    void pre_drawing();
    void draw();
    void post_drawing();

    void run();
    void ready();
    bool is_ready();

  private:
    bool check_folder();

    std::string folder_address_;
    bool folder_exists_, is_ready_, is_loaded_;

    Core *core_;
    Shader *shader_;
    Buffer *buffer_;
    GLint i_position_, i_uv_, i_normal_, i_tangent_, i_bitangent_;

    GLsizei data_size_;
    std::vector<torero::ComplexShaderData> buffer_data_;
    torero::ImageFile albedo_, normal_, metallic_, roughness_, ao_, emission_;
    Texture *t_albedo_, *t_normal_, *t_metallic_, *t_roughness_, *t_ao_, *t_emission_;

    boost::mutex protector_;

    bool error_;
    std::string error_log_;
  };
}

#endif // TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H
