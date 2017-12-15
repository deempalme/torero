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
    explicit Skybox(const char *folder_path, const char *file_extension, Core *core);
    ~Skybox();

    void draw();
    void calculate_reflectance();
    void bind_reflectance();

    const bool is_ready();

  private:
    void load_images();
    void load_ready();
    void update_camera();

    void write_data_opengl(const Visualizer::ImageFile &image, const int level);

    void prepare_cube();
    void prepare_quad();

    void render_cube();
    void render_quad();

    std::string folder_path_;
    std::string file_extension_;
    Core *core_;
    bool is_ready_, is_loaded_;

    Shader *irradiance_shader_;
    GLuint irr_map_id_;

    Shader *prefilter_shader_;
    GLuint pfr_map_id_;

    Shader *sky_shader_;
    GLint sky_u_pv_, sky_u_skybox_;
    GLuint sky_texture_id_;

    GLuint frame_buffer_, render_buffer_;

    Shader *brdf_shader_;
    GLuint brdf_texture_id_;

    Buffer *buffer_cube_, *buffer_squad_;
    GLint i_position_, i_normal_, i_uv_;
    GLsizei stride_size_short_, stride_size_medium_, stride_size_long_;

    Visualizer::ImageFile up_, down_, left_, right_, front_, back_, brdf_;

    boost::thread runner_;
    boost::mutex protector_;

    boost::signals2::connection signal_update_camera_, signal_update_screen_;
  };
  }

#endif // TORERO_SKYBOX_H
