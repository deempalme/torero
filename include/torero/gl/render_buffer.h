#ifndef TORERO_RENDER_BUFFER_H
#define TORERO_RENDER_BUFFER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/definition/types.h"

#include <string>

namespace torero {
  namespace gl {
    class RenderBuffer
    {
    public:
      RenderBuffer(const bool create = false);

      ~RenderBuffer();

      // Attaching the RenderBuffer to the bound FrameBuffer.
      // Returns false if RenderBuffer has not yet been created
      void attach_to_framebuffer(const GLenum internal_format = GL_DEPTH_STENCIL_ATTACHMENT);

      // Activate this RenderBuffer, returns false if not created yet
      bool bind();

      bool create();

      // if create() or is_created() are false, this will return the error's
      // description of the failure
      const std::string error_log();

      // returns the render buffer id
      const GLuint id();

      // returns true if the Frame buffer was properly created
      bool is_created();

      // Releases the actual RenderBuffer
      void release();

      // Establish data storage, format and dimensions of a RenderBuffer object's image.
      // Returns false if RenderBuffer has not yet been created
      void storage(const GLsizei width, const GLsizei height,
                   const GLenum format = GL_DEPTH24_STENCIL8);

    private:
      GLuint id_;
      bool is_created_;
      std::string error_log_;
    };
  }
}
#endif // TORERO_RENDER_BUFFER_H
