#ifndef TORERO_SKYBOX_H
#define TORERO_SKYBOX_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/shader.h"
#include "include/types.h"

#include "Algebraica.h"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

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
    const bool is_ready();

  private:
    void check_path(std::string *path);
    void load_images();
    void load_ready();
    void update_camera();

    void write_data_opengl(const Visualizer::ImageFile &image, const int level);

    void prepare_cube();

    std::string up_path_, dn_path_, lf_path_, rt_path_, ft_path_, bk_path_;
    Core *core_;
    bool is_ready_, is_loaded_;

    Shader *sky_shader_;
    GLint sky_u_pv_, sky_u_skybox_;
    GLuint sky_texture_id_;

    Buffer *buffer_cube_;

    Visualizer::ImageFile up_, down_, left_, right_, front_, back_;

    boost::thread runner_;
    boost::mutex protector_;

    boost::signals2::connection signal_update_camera_, signal_update_screen_;
  };
}

#endif // TORERO_SKYBOX_H
