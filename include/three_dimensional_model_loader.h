#ifndef TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H
#define TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/shader.h"
#include "include/texture.h"
#include "include/types.h"

#include "Algebraica.h"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>

namespace Toreo {
  class Core;

  class ThreeDimensionalModelLoader
  {
  public:
    explicit ThreeDimensionalModelLoader(const std::string folder_address,
                                         Shader *shader_program, Core *core);
    explicit ThreeDimensionalModelLoader(const Visualizer::Models model,
                                         Shader *shader_program, Core *core);
    ~ThreeDimensionalModelLoader();

    void pre_drawing();
    void draw();
    void post_drawing();
    const bool is_ready();

  private:
    bool check_folder();
    void initialize();
    void model_ready();

    std::string folder_address_;
    bool is_ready_, is_loaded_;

    Core *core_;
    Shader *shader_;
    Buffer *buffer_;
    GLint i_position_, i_uv_, i_normal_, i_tangent_, i_bitangent_;

    GLsizei data_size_;
    std::vector<Visualizer::ComplexShaderData> buffer_data_;
    Visualizer::ImageFile albedo_, normal_, metallic_, roughness_, ao_, emission_;
    Texture *t_albedo_, *t_normal_, *t_metallic_, *t_roughness_, *t_ao_, *t_emission_;

    boost::mutex protector_;
    boost::thread runner_;

    bool error_;
    std::string error_log_;
  };
}

#endif // TORERO_THREE_DIMENSIONAL_MODEL_LOADER_H
