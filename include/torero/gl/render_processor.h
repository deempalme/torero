#ifndef TORERO_RENDER_PROCESSOR_H
#define TORERO_RENDER_PROCESSOR_H

// OpenGL loader and SDL2 library
#include "glad/glad.h"

namespace torero {
  namespace gl {
    class RenderProcessor
    {
    public:
      RenderProcessor();
      ~RenderProcessor();

    protected:
      void initialize_render();
      void render();

    private:
      GLuint frame_albedo_, frame_emission_, frame_metalnes_, frame_normal_, frame_roghness_;
    };
  }
}
#endif // TORERO_RENDER_PROCESSOR_H
