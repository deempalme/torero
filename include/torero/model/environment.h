#ifndef TORERO_ENVIRONMENT_H
#define TORERO_ENVIRONMENT_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/definition/types.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
// Standard
#include <fstream>
#include <string>
#include <sys/stat.h>

namespace torero {
  class Core;

  namespace model {
    class Environment
    {
    public:
      explicit Environment(torero::Core *core);
      ~Environment();

      void bind_reflectance();
      void bind_skybox();

      void draw_skybox();

      bool load_enviroment_map(std::string environment_map = torero::k_default_environment_path);
      bool load_skybox(std::string skybox = torero::k_default_skybox_path);

      bool precompute_brdf(std::string output_path = torero::k_brdf_path);

      void remove_environment();
      void remove_skybox();

      bool skybox_visibility(const bool visible = true);

    private:
      bool create_cubemap(GLuint *cubemap_id, torero::image::FileFloat *image);
      bool load_brdf();

      void environment_ready();
      void environment_run();

      void skybox_ready();
      void skybox_run();

      void prepare_cube();
      void prepare_quad();

      void render_cube();

      torero::Core *core_;
      std::string environment_path_;
      bool environment_exist_, environment_is_loaded_;
      GLuint brdf_texture_id_;
      GLuint irradiance_map_id_;
      GLuint prefilter_map_id_;

      std::string skybox_path_;
      bool skybox_exist_, sky_is_loaded_, skybox_visible_;
      GLuint sky_cubemap_id_;

      torero::gl::Shader *skybox_shader_;
      torero::gl::Buffer buffer_cube_, *buffer_quad_;
      torero::image::FileFloat skybox_, environment_, brdf_;

      boost::mutex environment_protector_, sky_protector_;
    };
  }
}
#endif // TORERO_ENVIRONMENT_H
