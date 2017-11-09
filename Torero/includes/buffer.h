#ifndef BUFFER_H
#define BUFFER_H

#include "glad/glad.h"

class Buffer
{
public:
  Buffer();
  // construct this Buffer object and creates a new GL_VERTEX_ARRAY if create is true
  Buffer(const bool create);
  // frees the memory of its GL_VERTEX_ARRAY and GL_ARRAY_BUFFER and GL_ELEMENT_BUFFER
  ~Buffer();

  // creates a new GL_VERTEX_ARRAY that could contain a GL_ARRAY_BUFFER and GL_ELEMENT_BUFFER
  // it also binds this GL_VERTEX_ARRAY
  void create();
  // binds this GL_VERTEX_ARRAY or create and bind it if has not been created yet
  void use();
  // creates a new GL_VERTEX_ARRAY if is not yet created
  // also creates a new GL_ARRAY_BUFFER if has not been created yet
  // and allocates its buffered data
  // it also binds this GL_VERTEX_ARRAY and GL_ARRAY_BUFFER
  void allocate_array(const GLvoid *data, GLsizei size_in_bytes, GLenum ussage);
  // creates a new GL_VERTEX_ARRAY if is not yet created
  // also creates a new GL_ELEMENT_BUFFER if has not been created yet
  // and allocates its buffered data
  // it also binds this GL_VERTEX_ARRAY and GL_ELEMENT_BUFFER
  void allocate_element(const GLvoid *data, GLsizei size_in_bytes, GLenum ussage);

private:
  GLuint vertex_array_;
  GLuint array_buffer_, element_buffer_;

  bool is_created_, has_array_buffer_, has_element_buffer_;
};

#endif // BUFFER_H
