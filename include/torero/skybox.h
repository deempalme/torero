#ifndef TORERO_SKYBOX_H
#define TORERO_SKYBOX_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "torero/buffer.h"
#include "torero/definitions.h"
#include "torero/shader.h"
#include "torero/types.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <string>

namespace Toreo {
  class Core;

  class Skybox
  {
  public:
    explicit Skybox(const std::string up, const std::string down,
                    const std::string left, const std::string right,
                    const std::string front, const std::string back,
                    Core *core);
    ~Skybox();

    void draw();

    // Multithread functions
    void run();
    void ready();
    const bool is_ready();

  private:
    const bool check_path(std::string *path);
    void update_camera();

    void write_data_opengl(Visualizer::ImageFile *image, const int level);

    void prepare_cube();

    std::string up_path_, dn_path_, lf_path_, rt_path_, ft_path_, bk_path_;
    Core *core_;
    bool files_exists_, is_ready_, is_loaded_;

    Shader *sky_shader_;
    GLint sky_u_pv_, sky_u_skybox_;
    GLuint sky_texture_id_;

    Buffer *buffer_cube_;

    Visualizer::ImageFile up_, down_, left_, right_, front_, back_;

    boost::mutex protector_;

    boost::signals2::connection signal_update_camera_, signal_update_screen_;
  };
}

#endif // TORERO_SKYBOX_H
