#include "torero/gl/render_buffer.h"

namespace torero {
  namespace gl {
    RenderBuffer::RenderBuffer(const bool create) :
      id_(0),
      is_created_(false),
      error_log_("Frame buffer not created yet")
    {
      if(create){
        this->create();
        release();
      }
    }

    RenderBuffer::~RenderBuffer(){
      if(is_created_)
        glDeleteRenderbuffers(1, &id_);
    }

    void RenderBuffer::attach_to_framebuffer(const GLenum internal_format){
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, internal_format, GL_RENDERBUFFER, id_);
    }

    bool RenderBuffer::bind(){
      glBindRenderbuffer(GL_RENDERBUFFER, id_);
      return is_created_;
    }

    bool RenderBuffer::create(){
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

    const std::string RenderBuffer::error_log(){
      return error_log_;
    }

    const GLuint RenderBuffer::id(){
      return id_;
    }

    bool RenderBuffer::is_created(){
      return is_created_;
    }

    void RenderBuffer::release(){
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderBuffer::storage(const GLsizei width, const GLsizei height, const GLenum format){
      glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }
  }
}
