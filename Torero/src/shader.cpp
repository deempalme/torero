#include "includes/shader.h"

Shader::Shader() :
  id_(0),
  is_created_(false),
  error_log_("Shader program not created yet...\n----------\n")
{}

Shader::Shader(const char *vertex_path, const char *fragment_path) :
  id_(0),
  is_created_(false),
  error_log_()
{
  create(vertex_path, fragment_path);
}

Shader::~Shader(){
  if(is_created_)
    glDeleteProgram(id_);
}

bool Shader::create(const char *vertex_path, const char *fragment_path){
  if(!is_created_){
    error_log_.clear();
    is_created_ = true;
    std::ifstream vertex_file;
    std::ifstream fragment_file;

    std::string vertex_absolute_path(vertex_path);
    if(vertex_absolute_path[0] != '/')
      vertex_absolute_path = "/" + vertex_absolute_path;

    std::string fragment_absolute_path(fragment_path);
    if(fragment_absolute_path[0] != '/')
      fragment_absolute_path = "/" + fragment_absolute_path;

    vertex_absolute_path = boost::filesystem::current_path().string() + vertex_absolute_path;
    fragment_absolute_path = boost::filesystem::current_path().string() + fragment_absolute_path;

    // open files
    vertex_file.open(vertex_absolute_path.c_str());
    fragment_file.open(fragment_absolute_path.c_str());

    if(vertex_file.is_open() && fragment_file.is_open()){
      std::stringstream vertex_stream, fragment_stream;

      // read file's buffer contents into streams
      vertex_stream << vertex_file.rdbuf();
      fragment_stream << fragment_file.rdbuf();

      // close file handlers
      vertex_file.close();
      fragment_file.close();

      std::string vertex_text(vertex_stream.str());
      std::string fragment_text(fragment_stream.str());

      // convert stream into const char*
      const char *vertex_code{vertex_text.c_str()};
      const char *fragment_code{fragment_text.c_str()};

      // compile shaders
      GLint success;
      char info_log[512];
      GLsizei info_size;

      // vertex shader
      GLuint vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
      glShaderSource(vertex_shader, 1, &vertex_code, NULL);
      glCompileShader(vertex_shader);
      // check for shader compile errors
      glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
      if(!success){
        glGetShaderInfoLog(vertex_shader, 512, &info_size, info_log);
        error_log_ += std::string(vertex_path) + " vertex shader compilation failed...\n" +
            std::string(info_log, info_size) + "\n----------\n";
        is_created_ = false;
      }

      // fragment Shader
      GLuint fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
      glShaderSource(fragment_shader, 1, &fragment_code, NULL);
      glCompileShader(fragment_shader);
      // check for shader compile errors
      glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
      if(!success){
        glGetShaderInfoLog(fragment_shader, 512, &info_size, info_log);
        error_log_ += std::string(fragment_path) + " fragment shader compilation failed...\n" +
            std::string(info_log, info_size) + "\n----------\n";
        is_created_ = false;
      }

      // shader Program
      id_ = glCreateProgram();
      glAttachShader(id_, vertex_shader);
      glAttachShader(id_, fragment_shader);
      glLinkProgram(id_);
      // check for linking errors
      glGetProgramiv(id_, GL_LINK_STATUS, &success);
      if(!success){
        glGetProgramInfoLog(id_, 512, &info_size, info_log);
        error_log_ += "Shader program linking failed...\n" + std::string(info_log, info_size) +
            "\n----------\n";
        is_created_ = false;
      }
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);

      use();
    }else{
      if(!vertex_file.is_open())
        error_log_ += "The vertex shader: " + std::string(vertex_absolute_path) +
            " was not found.\n----------\n";
      if(!fragment_file.is_open())
        error_log_ += "The fragment shader: " + std::string(fragment_absolute_path) +
            " was not found.\n----------\n";
      is_created_ = false;
    }
  }
  return is_created_;
}

bool Shader::is_created(){
  return is_created_;
}

// activate the shader
bool Shader::use(){
  if(is_created_)
    glUseProgram(id_);
  return is_created_;
}

// return the program id
const GLuint Shader::id(){
  return id_;
}

// get uniform and attribute ids
const GLint Shader::uniform_location(const GLchar *name){
  if(is_created_)
    return glGetUniformLocation(id_, name);
  else
    return -1;
}

const GLint Shader::attribute_location(const GLchar *name){
  if(is_created_)
    return glGetAttribLocation(id_, name);
  else
    return -1;
}

const std::string Shader::error_log(){
  return error_log_;
}

// utility uniform functions
// bool ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const bool value){
  if(is_created_)
    glUniform1i(uniform_location, static_cast<int>(value));
  return is_created_;
}
// integer ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const int value){
  if(is_created_)
    glUniform1i(uniform_location, value);
  return is_created_;
}
// float ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const float value){
  if(is_created_)
    glUniform1f(uniform_location, value);
  return is_created_;
}
// vector 2D ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const Algebraica::vec2f &vector){
  if(is_created_)
    glUniform2fv(uniform_location, 1, vector.data());
  return is_created_;
}

bool Shader::set_value(const GLint uniform_location, const float x, const float y){
  if(is_created_)
    glUniform2f(uniform_location, x, y);
  return is_created_;
}
// vector 3D ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const Algebraica::vec3f &vector){
  if(is_created_)
    glUniform3fv(uniform_location, 1, vector.data());
  return is_created_;
}

bool Shader::set_value(const GLint uniform_location,
                             const float x, const float y, const float z){
  if(is_created_)
    glUniform3f(uniform_location, x, y, z);
  return is_created_;
}

bool Shader::set_values(const GLint uniform_location,
                              const Algebraica::vec3f *pointer_to_vector_array,
                              const GLsizei vector_array_size){
  if(is_created_)
    glUniform3fv(uniform_location, vector_array_size, pointer_to_vector_array->data());
  return is_created_;
}
// vector 4D ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const Algebraica::vec4f &vector){
  if(is_created_)
    glUniform4fv(uniform_location, 1, vector.data());
  return is_created_;
}

bool Shader::set_value(const GLint uniform_location,
                             const float x, const float y, const float z, const float w){
  if(is_created_)
    glUniform4f(uniform_location, x, y, z, w);
  return is_created_;
}
// matrix 3D ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const Algebraica::mat3f &matrix){
  if(is_created_)
    glUniformMatrix3fv(uniform_location, 1, GL_FALSE, matrix.data());
  return is_created_;
}
// matrix 4D ------------------------------------------------------------------------
bool Shader::set_value(const GLint uniform_location, const Algebraica::mat4f &matrix){
  if(is_created_)
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, matrix.data());
  return is_created_;
}
