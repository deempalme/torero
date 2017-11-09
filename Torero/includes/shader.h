#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "Algebraica.h"

#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

class Shader
{
public:
  Shader();
  // construct this object and creates this shader program
  Shader(const char *vertex_path, const char *fragment_path);
  ~Shader();

  // creates the shader program
  bool create(const char *vertex_path, const char *fragment_path);
  // returns if the shader program was properly created
  bool is_created();
  // if create() or is_created() are false, this will return the error's description of the failure
  const std::string error_log();
  // activate the shader
  bool use();
  // return the program id
  const GLuint id();
  // get uniform and attribute ids
  const GLint uniform_location(const GLchar *name);
  const GLint attribute_location(const GLchar *name);

  // utility uniform functions
  // bool ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const bool value);
  // integer ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const int value);
  // float ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const float value);
  // vector 2D ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const Algebraica::vec2f &vector);
  bool set_value(const GLint uniform_location, const float x, const float y);
  // vector 3D ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const Algebraica::vec3f &vector);
  bool set_value(const GLint uniform_location, const float x, const float y, const float z);
  bool set_values(const GLint uniform_location, const Algebraica::vec3f *pointer_to_vector_array,
                        const GLsizei vector_array_size);
  // vector 4D ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const Algebraica::vec4f &vector);
  bool set_value(const GLint uniform_location,
                 const float x, const float y, const float z, const float w);
  // matrix 3D ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const Algebraica::mat3f &matrix);
  // matrix 4D ------------------------------------------------------------------------
  bool set_value(const GLint uniform_location, const Algebraica::mat4f &matrix);

private:
  GLuint id_;
  bool is_created_;
  std::string error_log_;
};

#endif // SHADER_H
