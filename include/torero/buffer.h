#ifndef TORERO_BUFFER_H
#define TORERO_BUFFER_H

#include "glad/glad.h"

namespace Toreo {
  class Buffer
  {
  public:
    // construct this Buffer object and creates a new GL_VERTEX_ARRAY if create is true
    Buffer(const bool create = false):
      vertex_array_(0),
      array_buffer_(0),
      element_buffer_(0),
      is_created_(false),
      has_array_buffer_(false),
      has_element_buffer_(false)
    {
      if(create)
        this->create();
    }
    // frees the memory of its GL_VERTEX_ARRAY and GL_ARRAY_BUFFER and GL_ELEMENT_BUFFER
    ~Buffer(){
      if(has_array_buffer_)
        glDeleteBuffers(1, &array_buffer_);
      if(has_element_buffer_)
        glDeleteBuffers(1, &element_buffer_);
      glDeleteVertexArrays(1, &vertex_array_);
    }
    // creates a new GL_VERTEX_ARRAY that could contain a GL_ARRAY_BUFFER and GL_ELEMENT_BUFFER
    // it also binds this GL_VERTEX_ARRAY
    void create(){
      if(!is_created_){
        glGenVertexArrays(1, &vertex_array_);
        is_created_ = true;
      }
    }
    // binds this GL_VERTEX_ARRAY_OBJECT or create and bind it if has not been created yet
    void vertex_bind(){
      if(is_created_)
        glBindVertexArray(vertex_array_);
      else
        create();
    }
    // Releases this GL_VERTEX_ARRAY_OBJECT
    void vertex_release(){
      glBindVertexArray(0);
    }
    // binds this GL_VERTEX_BUFFER_OBJECT
    void buffer_bind(){
      if(is_created_){
        if(has_array_buffer_)
          glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        if(has_element_buffer_)
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
      }
    }
    // Releases this GL_VERTEX_BUFFER_OBJECT
    void buffer_release(){
      if(has_array_buffer_)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
      if(has_element_buffer_)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void divisor(const GLuint attribute_id, const GLuint divisor){
      glVertexAttribDivisor(attribute_id, divisor);
    }
    // creates a new GL_VERTEX_ARRAY if is not yet created
    // also creates a new GL_ARRAY_BUFFER if has not been created yet
    // and allocates its buffered data
    // it also binds this GL_VERTEX_ARRAY and GL_ARRAY_BUFFER
    void allocate_array(const GLvoid *data, const GLsizei size_in_bytes,
                        const GLenum ussage = GL_STATIC_DRAW){
      if(!has_array_buffer_){
        glGenBuffers(1, &array_buffer_);
        has_array_buffer_ = true;
      }
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
      glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, ussage);
    }
    // Allocates a new buffer inside an already created array buffer
    // Returns false if the array buffer has not been created yet using allocate_array()
    bool allocate_range_array(const GLvoid *data, const GLint offset, const GLsizei size_in_bytes){
      if(has_array_buffer_){
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size_in_bytes, data);
        return true;
      }else
        return false;
    }
    // creates a new GL_VERTEX_ARRAY if is not yet created
    // also creates a new GL_ELEMENT_BUFFER if has not been created yet
    // and allocates its buffered data
    // it also binds this GL_VERTEX_ARRAY and GL_ELEMENT_BUFFER
    void allocate_element(const GLvoid *data, const GLsizei size_in_bytes,
                          const GLenum ussage = GL_STATIC_DRAW){
      if(!has_element_buffer_){
        glGenBuffers(1, &element_buffer_);
        has_element_buffer_ = true;
      }
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, ussage);
    }
    // Allocates a new buffer inside an already created element buffer
    // Returns false if the element array buffer has not been created yet using allocate_element()
    bool allocate_range_element(const GLvoid *data, const GLint offset,
                                const GLsizei size_in_bytes){
      if(has_element_buffer_){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size_in_bytes, data);
        return true;
      }else
        return false;
    }
    // Declares the attribute's buffer format
    void attributte_buffer(const GLuint index, const GLint size, const GLint offset,
                           const GLsizei stride, const GLenum type = GL_FLOAT,
                           const GLboolean normalized = GL_FALSE){
      glVertexAttribPointer(index, size, type, normalized, stride,
                            reinterpret_cast<const GLvoid*>(offset));
    }
    // Enables the vertex array's attribute with id = attribute_id
    void enable(const GLuint attribute_id){
      glEnableVertexAttribArray(attribute_id);
    }
    // Disables the vertex array's attribute with id = attribute_id
    void disable(const GLuint attribute_id){
      glDisableVertexAttribArray(attribute_id);
    }
    // Returns the size in bytes of the array buffer
    GLint size_array(){
      GLint size{0};
      if(has_array_buffer_){
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      }
      return size;
    }
    // Returns the size in bytes of the element array buffer
    GLint size_element(){
      GLint size{0};
      if(has_array_buffer_){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      }
      return size;
    }

  private:
    GLuint vertex_array_, array_buffer_, element_buffer_;
    bool is_created_, has_array_buffer_, has_element_buffer_;
  };
  }

#endif // TORERO_BUFFER_H
