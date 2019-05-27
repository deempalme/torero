#include "torero/gl/buffer.h"

namespace torero {
  namespace gl {
    Buffer::Buffer(const bool create):
      array_buffer_(0),
      element_buffer_(0),
      vertex_array_(0)
    {
      if(create)
        this->create();
    }

    Buffer::~Buffer(){
      delete_buffer();
    }

    bool Buffer::delete_buffer(){
      if(vertex_array_ == 0 && array_buffer_ == 0 && element_buffer_ == 0) return false;

      if(array_buffer_ != 0){
        glDeleteBuffers(1, &array_buffer_);
        array_buffer_ = 0;
      }
      if(element_buffer_ != 0){
        glDeleteBuffers(1, &element_buffer_);
        element_buffer_ = 0;
      }
      glDeleteVertexArrays(1, &vertex_array_);
      vertex_array_ = 0;
      return true;
    }

    bool Buffer::create(){
      if(vertex_array_ != 0) return false;

      glGenVertexArrays(1, &vertex_array_);
      return true;
    }

    bool Buffer::vertex_bind(){
      if(vertex_array_ == 0) return false;

      glBindVertexArray(vertex_array_);
      return false;
    }

    void Buffer::vertex_release(){
      glBindVertexArray(0);
    }

    void Buffer::buffer_bind(){
      if(array_buffer_ != 0)
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
      if(element_buffer_ != 0)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    }

    void Buffer::buffer_release(){
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Buffer::divisor(const GLuint attribute_id, const GLuint divisor){
      glVertexAttribDivisor(attribute_id, divisor);
    }

    bool Buffer::allocate_array(const GLvoid *data, const GLsizei size_in_bytes,
                                const GLenum ussage){
      if(array_buffer_ == 0 && vertex_array_ != 0) glGenBuffers(1, &array_buffer_);
      if(array_buffer_ == 0) return false;

      glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
      glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, ussage);
      return true;
    }

    bool Buffer::allocate_section_array(const GLvoid *data, const GLint offset,
                                        const GLsizei size_in_bytes){
      if(array_buffer_ != 0){
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size_in_bytes, data);
        return true;
      }else
        return false;
    }

    bool Buffer::allocate_element(const GLvoid *data, const GLsizei size_in_bytes,
                                  const GLenum ussage){
      if(element_buffer_ == 0 && vertex_array_ != 0) glGenBuffers(1, &element_buffer_);
      if(element_buffer_ == 0) return false;

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, ussage);
      return true;
    }

    bool Buffer::allocate_section_element(const GLvoid *data, const GLint offset,
                                          const GLsizei size_in_bytes){
      if(element_buffer_ != 0){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size_in_bytes, data);
        return true;
      }else
        return false;
    }

    void Buffer::attributte_buffer(const GLuint index, const GLint size, const GLint offset,
                                   const GLsizei stride, const GLenum type,
                                   const GLboolean normalized){
      glVertexAttribPointer(index, size, type, normalized, stride,
                            reinterpret_cast<const GLvoid*>(offset));
    }

    void Buffer::enable(const GLuint attribute_id){
      glEnableVertexAttribArray(attribute_id);
    }

    GLuint Buffer::id_array(){
      return array_buffer_;
    }

    GLuint Buffer::id_element(){
      return element_buffer_;
    }

    GLuint Buffer::id_vertex(){
      return vertex_array_;
    }

    void Buffer::disable(const GLuint attribute_id){
      glDisableVertexAttribArray(attribute_id);
    }

    GLint Buffer::size_array(){
      GLint size{0};
      if(array_buffer_ != 0){
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      }
      return size;
    }

    GLint Buffer::size_element(){
      GLint size{0};
      if(element_buffer_ != 0){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      }
      return size;
    }
  }
}
