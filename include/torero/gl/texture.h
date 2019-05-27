#ifndef TORERO_TEXTURE_H
#define TORERO_TEXTURE_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/definition/types.h"

#include <string>

namespace torero{
  namespace gl {
    class Texture
    {
    public:
      Texture(const bool create = false,
              const GLuint active_texture = torero::ShaderTexture::Albedo,
              const GLenum texture_target = GL_TEXTURE_2D);
      Texture(const torero::image::File &image,
              const GLuint active_texture = torero::ShaderTexture::Albedo);

      ~Texture();
      // returns the program texture unit
      const GLuint &active_texture();
      void active_texture(const GLuint new_active_texture);

      bool allocate(const GLsizei width, const GLsizei height, const void *texture_data = nullptr,
                    const GLenum data_format = GL_RGBA, const GLenum data_type = GL_UNSIGNED_BYTE,
                    const GLint internal_format = GL_RGBA8, const GLenum target = GL_TEXTURE_2D,
                    const GLint level = 0);
      bool allocate(const GLsizei width, const GLsizei height, const void *texture_data,
                    const int component_size);

      void bind();

      bool delete_texture();

      bool error();
      // if error() = true, then, this will return the error's description of the failure
      const std::string error_log();

      bool generate();

      bool generate_mipmap();

      bool parameter(const GLint wrap_s = GL_REPEAT, const GLint wrap_t = GL_REPEAT,
                     const GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
                     const GLint mag_filter = GL_LINEAR);
      bool parameter(const GLenum parameter_name, const GLint value);
      bool parameter(const GLenum parameter_name, const GLfloat value);
      // returns the texture id
      const GLuint &id();

      void internal_format(const GLint new_internal_format);

      const GLfloat max_anisotropic_filtering();
      void max_anisotropic_filtering(const GLfloat new_max_filtering);

      bool mipmap();
      void mipmap(const bool generate_mipmap);

      const GLenum texture_target();
      void texture_target(const GLenum new_texture_target);

      static bool set_max_filtering(const GLfloat max_filtering);

    private:
      GLuint id_;
      GLuint active_texture_;
      GLenum data_type_, texture_target_;
      GLint internal_format_;
      GLfloat max_filtering_;
      bool has_mipmap_, error_, custom_filtering_;
      std::string error_log_;

      static bool filtering_initialized_;
      static GLfloat global_max_filtering_;
    };
  }
}
#endif // TORERO_TEXTURE_H
