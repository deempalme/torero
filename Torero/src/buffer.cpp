#include "includes/buffer.h"

Buffer::Buffer() :
  vertex_array_(0),
  array_buffer_(0),
  element_buffer_(0),
  is_created_(false),
  has_array_buffer_(false),
  has_element_buffer_(false)
{}

Buffer::Buffer(const bool create) :
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

Buffer::~Buffer(){
  use();
  if(has_array_buffer_)
    glDeleteBuffers(1, &array_buffer_);
  if(has_element_buffer_)
    glDeleteBuffers(1, &element_buffer_);
  glDeleteVertexArrays(1, &vertex_array_);
}

void Buffer::create(){
  if(!is_created_){
    glGenVertexArrays(1, &vertex_array_);
    is_created_ = true;
  }
  use();
}

void Buffer::use(){
  if(is_created_)
    glBindVertexArray(vertex_array_);
  else
    create();
}

void Buffer::allocate_array(const GLvoid *data, GLsizei size_in_bytes, GLenum ussage){
  use();
  if(!has_array_buffer_){
    glGenBuffers(1, &array_buffer_);
    has_array_buffer_ = true;
  }
  glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
  glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, ussage);
}

void Buffer::allocate_element(const GLvoid *data, GLsizei size_in_bytes, GLenum ussage){
  use();
  if(!has_element_buffer_){
    glGenBuffers(1, &element_buffer_);
    has_element_buffer_ = true;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, data, ussage);
}
