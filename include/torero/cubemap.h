#ifndef TORERO_CUBEMAP_H
#define TORERO_CUBEMAP_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "torero/buffer.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <string>

namespace torero {
  class Core;

  class Cubemap
  {
  public:
    explicit Cubemap(const char *folder_path, const char *file_extension, Core *core);
    ~Cubemap();

    void bind_reflectance();

    void run();
    void ready();
    bool is_ready();

  private:
    void write_data_opengl(torero::ImageFile *image, const int level);

    void prepare_cube();
    void prepare_quad();

    void render_cube();
    void render_quad();

    std::string folder_path_;
    std::string file_extension_;
    bool folder_exist_;
    Core *core_;
    bool is_ready_, is_loaded_;

    Shader *irradiance_shader_;
    GLuint irr_map_id_;

    Shader *prefilter_shader_;
    GLuint pfr_map_id_;

    GLuint brdf_texture_id_;

    Buffer *buffer_cube_, *buffer_squad_;
    GLint i_position_, i_normal_, i_uv_;

    torero::ImageFile up_, down_, left_, right_, front_, back_, brdf_;

    boost::mutex protector_;
  };
}

#endif // TORERO_CUBEMAP_H
