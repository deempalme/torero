#ifndef TORERO_RENDER_BUFFER_H
#define TORERO_RENDER_BUFFER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/type_definitions.h"

#include <string>

namespace torero {
  class RenderBuffer
  {
  public:
    RenderBuffer(const bool create = false) :
      id_(0),
      is_created_(false),
      error_log_("Frame buffer not created yet")
    {
      if(create){
        this->create();
        release();
      }
    }

    ~RenderBuffer(){
      if(is_created_)
        glDeleteRenderbuffers(1, &id_);
    }

    bool create(){
      error_log_.clear();
      if(!is_created_){

        glGenRenderbuffers(1, &id_);
        glBindRenderbuffer(GL_RENDERBUFFER, id_);

        is_created_ = true;
        return true;
      }else{
        if(is_created_)
          error_log_ += "RenderBuffer already created...\n----------\n";
        return false;
      }
    }

    // returns true if the Frame buffer was properly created
    bool is_created(){
      return is_created_;
    }

    // if create() or is_created() are false, this will return the error's
    // description of the failure
    const std::string error_log(){
      return error_log_;
    }

    // Activate this RenderBuffer, returns false if not created yet
    bool bind(){
      glBindRenderbuffer(GL_RENDERBUFFER, id_);
      return is_created_;
    }

    // Releases the actual RenderBuffer
    void release(){
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    // Establish data storage, format and dimensions of a RenderBuffer object's image.
    // Returns false if RenderBuffer has not yet been created
    void storage(const GLsizei width, const GLsizei height,
                 const GLenum format = GL_DEPTH24_STENCIL8){
      glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }

    // Attaching the RenderBuffer to the bound FrameBuffer.
    // Returns false if RenderBuffer has not yet been created
    void attach_to_framebuffer(const GLenum internal_format = GL_DEPTH_STENCIL_ATTACHMENT){
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, internal_format, GL_RENDERBUFFER, id_);
    }

    // returns the render buffer id
    const GLuint id(){
      return id_;
    }

  private:
    GLuint id_;
    bool is_created_;
    std::string error_log_;
  };
}
#endif // TORERO_RENDER_BUFFER_H
