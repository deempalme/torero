#include "torero/gl/frame_buffer.h"

namespace torero {
  namespace gl {
    FrameBuffer::FrameBuffer(const bool generate, const bool add_render_buffer) :
      frame_id_(0),
      render_id_(0),
      frame_attachment_(GL_NONE),
      render_attachment_(GL_NONE),
      render_format_(GL_NONE),
      error_log_("Frame buffer not created yet")
    {
      if(generate)
        this->generate_frame(add_render_buffer);
    }

    FrameBuffer::~FrameBuffer(){
      delete_frame();
    }

    bool FrameBuffer::attach_2D(const GLint texture, const GLenum attachment, const GLenum texture_target_type, const GLint level){
      if(frame_id_ == 0) return false;

      glFramebufferTexture2D(GL_FRAMEBUFFER, frame_attachment_ = attachment,
                             texture_target_type, texture, level);
      glDrawBuffers(1, &frame_attachment_);
      return true;
    }

    bool FrameBuffer::attach_render(const GLenum render_attachment){
      if(frame_id_ == 0 || render_id_ == 0) return false;

      glFramebufferRenderbuffer(GL_FRAMEBUFFER, render_attachment_ = render_attachment,
                                GL_RENDERBUFFER, render_id_);
      return true;
    }

    bool FrameBuffer::bind(){
      if(frame_id_ == 0) return false;

      glBindFramebuffer(GL_FRAMEBUFFER, frame_id_);
      return true;
    }

    bool FrameBuffer::bind_render(){
      if(render_id_ == 0) return false;

      glBindRenderbuffer(GL_RENDERBUFFER, render_id_);
      return true;
    }

    void FrameBuffer::clear(){
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    bool FrameBuffer::delete_frame(){
      if(frame_id_ == 0) return false;

      glDeleteFramebuffers(1, &frame_id_);
      frame_id_ = 0;

      delete_render();
      return true;
    }

    bool FrameBuffer::delete_render(){
      if(render_id_ == 0) return false;

      glDeleteRenderbuffers(1, &render_id_);
      render_id_ = 0;
      return true;
    }

    const std::string &FrameBuffer::error_log(){
      return error_log_;
    }

    const GLenum &FrameBuffer::frame_attachment(){
      return frame_attachment_;
    }

    const GLuint &FrameBuffer::frame_id(){
      return frame_id_;
    }

    bool FrameBuffer::generate_render(){
      if(render_id_ != 0) return false;

      glGenRenderbuffers(1, &render_id_);
      return true;
    }

    bool FrameBuffer::generate_frame(const bool generate_render_buffer){
      if(frame_id_ == 0){
        error_log_.clear();
        glGenFramebuffers(1, &frame_id_);

        if(generate_render_buffer && render_id_ == 0)
          generate_render();

        status();

        return true;
      }else{
        error_log_ += "FrameBuffer already created...\n----------\n";
        return false;
      }
    }

    bool FrameBuffer::is_created(){
      return frame_id_ != 0;
    }

    bool FrameBuffer::read_pixel(GLvoid *data, const GLint x, const GLint y, const GLenum type, const GLenum components){
      if(frame_id_ == 0) return false;

      glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_id_);
      glReadBuffer(frame_attachment_);
      glReadPixels(x, y, 1, 1, components, type, data);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      return true;
    }

    bool FrameBuffer::release(){
      if(frame_id_ == 0) return false;

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      return true;
    }

    bool FrameBuffer::release_render(){
      if(render_id_ == 0) return false;

      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      return true;
    }

    const GLenum &FrameBuffer::render_attachment(){
      return render_attachment_;
    }

    const GLenum &FrameBuffer::render_format(){
      return render_format_;
    }

    const GLuint &FrameBuffer::render_id(){
      return render_id_;
    }

    bool FrameBuffer::render_storage(const GLsizei width, const GLsizei height, const GLenum internal_format){
      if(render_id_ == 0) return false;

      glRenderbufferStorage(GL_RENDERBUFFER, render_format_ = internal_format, width, height);
      return true;
    }

    GLenum FrameBuffer::status(){
      const GLenum frame_status{glCheckFramebufferStatus(GL_FRAMEBUFFER)};

      if(frame_status != GL_FRAMEBUFFER_COMPLETE){
        switch(frame_status){
          case GL_FRAMEBUFFER_UNDEFINED:
            error_log_ += "The specified framebuffer is the default read or draw framebuffer, "
                          "but the default framebuffer does not exist.";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            error_log_ += "The framebuffer attachment points are framebuffer incomplete.";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            error_log_ += "The framebuffer does not have at least one image attached to it.";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            error_log_ += "The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for "
                          "any color attachment point(s) named by GL_DRAW_BUFFERi";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            error_log_ += "GL_READ_BUFFER is not GL_NONE and the value of "
                          "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color "
                          "attachment point named by GL_READ_BUFFER.";
          break;
          case GL_FRAMEBUFFER_UNSUPPORTED:
            error_log_ += "The combination of internal formats of the attached images "
                          "violates an implementation-dependent set of restrictions";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            error_log_ += "The value of GL_RENDERBUFFER_SAMPLES is not the same for all "
                          "attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the "
                          "not same for all attached textures; or, if the attached images are "
                          "a mix of renderbuffers and textures, the value of "
                          "GL_RENDERBUFFER_SAMPLES does not match the value of "
                          "GL_TEXTURE_SAMPLES.\n\n"
                          "The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for "
                          "all attached textures; or, if the attached images are a mix of "
                          "renderbuffers and textures, the value of "
                          "GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all "
                          "attached textures.";
          break;
          case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            error_log_ += "A framebuffer attachment is layered, and any populated attachment "
                          "is not layered, or if all populated color attachments are not from "
                          "textures of the same target.";
          break;
          default:
            error_log_ += "Unknown error";
          break;
        }
        error_log_ += "\n----------\n";
      }
      return frame_status;
    }
  }
}
