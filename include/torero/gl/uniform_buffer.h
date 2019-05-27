#ifndef TORERO_UNIFORM_BUFFER_H
#define TORERO_UNIFORM_BUFFER_H

#include "glad/glad.h"

namespace torero {
  namespace gl {
    class UniformBuffer
    {
    public:
      // construct this Buffer object and creates a new GL_VERTEX_ARRAY if create is true
      UniformBuffer(const bool create = false):
        buffer_(0),
        is_created_(false)
      {
        if(create)
          this->create();
      }
      // frees the memory of its GL_UNIFORM_BUFFER
      ~UniformBuffer(){
        if(is_created_){
          glDeleteBuffers(1, &buffer_);
        }
      }
      // creates a new buffer
      void create(){
        if(!is_created_){
          glGenBuffers(1, &buffer_);
          is_created_ = true;
        }
      }
      // binds this GL_UNIFORM_BUFFER
      void bind(){
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_);
      }
      // Releases this GL_UNIFORM_BUFFER
      void release(){
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
      }
      // Allocates the buffer data
      void allocate(const GLvoid *data, const GLsizei size_in_bytes,
                    const GLenum ussage = GL_STATIC_DRAW){
        glBufferData(GL_UNIFORM_BUFFER, size_in_bytes, data, ussage);
      }
      // Allocates a section into the buffer data
      void allocate_section(const GLvoid *data, const GLsizeiptr size_in_bytes,
                            const GLintptr offset_in_bytes = 0){
        glBufferSubData(GL_UNIFORM_BUFFER, offset_in_bytes, size_in_bytes, data);
      }
      // Links a specific region of a buffer to a binding point
      void range(const GLsizeiptr size_in_bytes, const GLuint index = 0,
                 const GLintptr offset_in_bytes = 0){
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer_, offset_in_bytes, size_in_bytes);
      }
      void bind_base(const GLuint index){
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer_);
      }
      void bind_range(const GLuint index, const GLintptr offset_in_bytes,
                      const GLsizeiptr size_in_bytes){
        glBindBufferRange(GL_UNIFORM_BUFFER, index, buffer_, offset_in_bytes, size_in_bytes);
      }
      // Returns the size in bytes of the array buffer
      GLint size(){
        GLint size{0};
        if(is_created_){
          glBindBuffer(GL_UNIFORM_BUFFER, buffer_);
          glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &size);
        }
        return size;
      }

    private:
      GLuint buffer_;
      bool is_created_;
    };
  }
}
#endif // TORERO_UNIFORM_BUFFER_H
