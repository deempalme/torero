#ifndef TORERO_FRAME_BUFFER_H
#define TORERO_FRAME_BUFFER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/type_definitions.h"

#include <string>

namespace torero {
  class FrameBuffer
  {
  public:
    FrameBuffer(const bool create = false) :
      id_(0),
      attachment_(GL_COLOR_ATTACHMENT0),
      is_created_(false),
      error_log_("Frame buffer not created yet")
    {
      if(create){
        this->create();
        release();
      }
    }

    ~FrameBuffer(){
      if(is_created_)
        glDeleteFramebuffers(1, &id_);
    }

    bool create(){
      error_log_.clear();
      if(!is_created_){

        glGenFramebuffers(1, &id_);
        glBindFramebuffer(GL_FRAMEBUFFER, id_);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
          is_created_ = true;
          return true;
        }else{
          error_log_ += "Error when creating the FrameBuffer, check framebuffer_status() ";
          error_log_ += "for more information...\n----------\n";
          return false;
        }
      }else{
        if(is_created_)
          error_log_ += "FrameBuffer already created...\n----------\n";
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

    // Activate this FrameBuffer, returns false if not created yet
    bool bind(){
      glBindFramebuffer(GL_FRAMEBUFFER, id_);
      return is_created_;
    }

    // Clear the FrameBuffer data
    void clear(){
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    // Releases the actual FrameBuffer
    void release(){
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Attaching a texture object into the FrameBuffer, returns false if
    // FrameBuffer has not yet been created
    void attach2D(const GLint texture, const GLenum attachment = GL_COLOR_ATTACHMENT0,
                  const GLenum texture_target_type = GL_TEXTURE_2D, const GLint level = 0){
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture_target_type, texture, level);
      glDrawBuffers(1, &attachment);
      attachment_ = attachment;
    }

    // Returning the status of this frame buffer
    GLenum framebuffer_status(){
      return glCheckFramebufferStatus(GL_FRAMEBUFFER);
    }

    // Returns the attachment type
    const GLenum &attachment(){
      return attachment_;
    }

    void read_pixel(GLvoid *data, const GLint x, const GLint y,
                    const GLenum type = GL_UNSIGNED_BYTE, const GLenum components = GL_RGB){
      glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
      glReadBuffer(attachment_);
      glReadPixels(x, y, 1, 1, components, type, data);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // returns the frame buffer id
    const GLuint &id(){
      return id_;
    }

  private:
    GLuint id_;
    GLenum attachment_;
    bool is_created_;
    std::string error_log_;
  };
}
#endif // TORERO_FRAME_BUFFER_H
