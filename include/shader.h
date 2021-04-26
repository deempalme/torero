#ifndef TORERO_SHADER_H
#define TORERO_SHADER_H

#include "glad/glad.h"

#include "algebraica/algebraica.h"

#include <string>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

namespace Toreo {
  class Shader
  {
  public:
    // Object constructor, the shader will not yet be created
    Shader() :
      id_(0),
      is_created_(false),
      error_log_("Shader program not created yet...\n----------\n")
    {}
    // Construct this object and creates this shader program
    Shader(const std::string vertex_path,
           const std::string fragment_path,
           const std::string geometry_path = "") :
      id_(0),
      is_created_(false),
      error_log_()
    {
      create(vertex_path, fragment_path, geometry_path);
    }
    // Deletes the shader program from OpenGL memory
    ~Shader(){
      if(is_created_)
        glDeleteProgram(id_);
    }
    // Creates the shader program if is not yet created
    bool operator()(const std::string vertex_path,
                    const std::string fragment_path,
                    const std::string geometry_path = ""){
      return create(vertex_path, fragment_path, geometry_path);
    }
    // Creates the shader program
    bool create(const std::string vertex_path,
                const std::string fragment_path,
                const std::string geometry_path = ""){
      if(!is_created_){
        error_log_.clear();

        std::ifstream vertex_file;
        std::string vertex_absolute_path(vertex_path);
        if(vertex_absolute_path.front() != '/')
          vertex_absolute_path = "/" + vertex_absolute_path;

        // Verifying the existence of the vertex shader path
        if(!boost::filesystem::exists(boost::filesystem::path(vertex_absolute_path))){
          vertex_absolute_path = boost::filesystem::current_path().string() +
                                 vertex_absolute_path;

          if(!boost::filesystem::exists(boost::filesystem::path(vertex_absolute_path))){
            error_log_ = "The file: " + vertex_path + " was not found.\n" +
                         "  Neither: " + vertex_absolute_path + "\n";
            return false;
          }
        }

        std::ifstream fragment_file;
        std::string fragment_absolute_path(fragment_path);
        if(fragment_absolute_path.front() != '/')
          fragment_absolute_path = "/" + fragment_absolute_path;

        // Verifying the existence of the fragment shader path
        if(!boost::filesystem::exists(boost::filesystem::path(fragment_absolute_path))){
          fragment_absolute_path = boost::filesystem::current_path().string() +
                                   fragment_absolute_path;

          if(!boost::filesystem::exists(boost::filesystem::path(fragment_absolute_path))){
            error_log_ = "The file: " + fragment_path + " was not found.\n" +
                         "  Neither: " + fragment_absolute_path + "\n";
            return false;
          }
        }

        std::ifstream geometry_file;
        std::string geometry_absolute_path(geometry_path);
        bool geometry{geometry_path != ""};

        if(geometry){
          if(geometry_absolute_path.front() != '/')
            geometry_absolute_path = "/" + geometry_absolute_path;

          // Verifying the existence of the fragment shader path
          if(!boost::filesystem::exists(boost::filesystem::path(geometry_absolute_path))){
            geometry_absolute_path = boost::filesystem::current_path().string() +
                                     geometry_absolute_path;

            if(!boost::filesystem::exists(boost::filesystem::path(geometry_absolute_path))){
              error_log_ = "The file: " + geometry_path + " was not found.\n" +
                           "  Neither: " + geometry_absolute_path + "\n";
              return false;
            }
          }
        }

        is_created_ = true;

        // open files
        vertex_file.open(vertex_absolute_path);
        fragment_file.open(fragment_absolute_path);
        if(geometry){
          geometry_file.open(geometry_absolute_path);
          geometry = geometry && geometry_file.is_open();
          if(!geometry){
            is_created_ = false;
            error_log_ += "The geometry shader: " + std::string(geometry_absolute_path) +
                          " was not opened.\n----------\n";
          }
        }

        if(vertex_file.is_open() && fragment_file.is_open()){
          std::stringstream vertex_stream, fragment_stream, geometry_stream;

          // read file's buffer contents into streams
          vertex_stream << vertex_file.rdbuf();
          fragment_stream << fragment_file.rdbuf();
          if(geometry) geometry_stream << geometry_file.rdbuf();

          // close file handlers
          vertex_file.close();
          fragment_file.close();
          if(geometry) geometry_file.close();

          std::string vertex_text(vertex_stream.str());
          std::string fragment_text(fragment_stream.str());
          std::string geometry_text;
          if(geometry) geometry_text = geometry_stream.str();

          // convert stream into const char*
          const char *vertex_code{vertex_text.c_str()};
          const char *fragment_code{fragment_text.c_str()};
          const char *geometry_code{geometry_text.c_str()};

          // compile shaders
          GLint has_succed;
          char info_log[512];
          GLsizei info_size;

          // vertex shader
          GLuint vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
          glShaderSource(vertex_shader, 1, &vertex_code, NULL);
          glCompileShader(vertex_shader);
          // check for shader compile errors
          glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &has_succed);
          if(has_succed == GL_FALSE){
            glGetShaderInfoLog(vertex_shader, 512, &info_size, info_log);
            error_log_ += "\n### Vertex shader compilation failed...\n\n" +
                          std::string(info_log, info_size) + "\n----------\n";
            is_created_ = false;
          }

          // fragment Shader
          GLuint fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
          glShaderSource(fragment_shader, 1, &fragment_code, NULL);
          glCompileShader(fragment_shader);
          // check for shader compile errors
          glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &has_succed);
          if(has_succed == GL_FALSE){
            glGetShaderInfoLog(fragment_shader, 512, &info_size, info_log);
            error_log_ += "\n### Fragment shader compilation failed...\n\n" +
                          std::string(info_log, info_size) + "\n----------\n";
            is_created_ = false;
          }

          // Geometry Shader
          GLuint geometry_shader;
          if(geometry){
            geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry_shader, 1, &geometry_code, NULL);
            glCompileShader(geometry_shader);
            // check for shader compile errors
            glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &has_succed);
            if(has_succed == GL_FALSE){
              glGetShaderInfoLog(geometry_shader, 512, &info_size, info_log);
              error_log_ += "\n### Geometry shader compilation failed...\n\n" +
                            std::string(info_log, info_size) + "\n----------\n";
              is_created_ = false;
            }
          }

          // shader Program
          id_ = glCreateProgram();
          glAttachShader(id_, vertex_shader);
          glAttachShader(id_, fragment_shader);
          if(geometry) glAttachShader(id_, geometry_shader);
          glLinkProgram(id_);
          // check for linking errors
          glGetProgramiv(id_, GL_LINK_STATUS, &has_succed);
          if(has_succed == GL_FALSE){
            glGetProgramInfoLog(id_, 512, &info_size, info_log);
            error_log_ += "\n### Shader program linking failed...\n\n" +
                          std::string(info_log, info_size) +
                          "\n----------\n";
            is_created_ = false;
          }
          glDetachShader(id_, vertex_shader);
          glDetachShader(id_, fragment_shader);

          glDeleteShader(vertex_shader);
          glDeleteShader(fragment_shader);

          if(geometry){
            glDetachShader(id_, geometry_shader);
            glDeleteShader(geometry_shader);
          }

          use();
        }else{
          if(!vertex_file.is_open())
            error_log_ += "The vertex shader: " + std::string(vertex_absolute_path) +
                          " was not opened.\n----------\n";
          if(!fragment_file.is_open())
            error_log_ += "The fragment shader: " + std::string(fragment_absolute_path) +
                          " was not opened.\n----------\n";
          is_created_ = false;
        }
        return is_created_;
      }else{
        error_log_ += "The shader was previously created.\n----------\n";
        return false;
      }
    }
    // Returns true if the shader program was properly created
    bool is_created(){
      return is_created_;
    }
    // If create() or is_created() are false, this will return the error's description of the failure
    const std::string error_log(){
      return error_log_;
    }
    // activate the shader
    bool use(){
      glUseProgram(id_);
      return is_created_;
    }
    // returns the program id
    const GLuint id(){
      return id_;
    }
    // get uniform and attribute ids
    const GLint uniform_location(const std::string name){
      return glGetUniformLocation(id_, name.c_str());
    }
    const GLint attribute_location(const std::string name){
      return glGetAttribLocation(id_, name.c_str());
    }
    // utility uniform functions
    // bool -----------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const bool value){
      glUniform1i(uniform_location, static_cast<int>(value));
    }
    // integer --------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const int value){
      glUniform1i(uniform_location, value);
    }
    // GLuint ---------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const GLuint value){
      glUniform1ui(uniform_location, value);
    }
    // float ----------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const float value){
      glUniform1f(uniform_location, value);
    }
    // vector 2D ------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const algebraica::vec2f &vector){
      glUniform2fv(uniform_location, 1, vector.data());
    }
    void set_value(const GLint uniform_location, const float x, const float y){
      glUniform2f(uniform_location, x, y);
    }
    // vector 3D ------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const algebraica::vec3f &vector){
      glUniform3fv(uniform_location, 1, vector.data());
    }
    void set_value(const GLint uniform_location, const float x, const float y, const float z){
      glUniform3f(uniform_location, x, y, z);
    }
    void set_values(const GLint uniform_location, const algebraica::vec3f *pointer_to_vector_array,
                    const GLsizei vector_array_size){
      glUniform3fv(uniform_location, vector_array_size, pointer_to_vector_array->data());
    }
    // vector 4D ------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const algebraica::vec4f &vector){
      glUniform4fv(uniform_location, 1, vector.data());
    }
    void set_value(const GLint uniform_location,
                   const float x, const float y, const float z, const float w){
      glUniform4f(uniform_location, x, y, z, w);
    }
    // matrix 3D ------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const algebraica::mat3f &matrix){
      glUniformMatrix3fv(uniform_location, 1, GL_FALSE, matrix.data());
    }
    // matrix 4D ------------------------------------------------------------------------
    void set_value(const GLint uniform_location, const algebraica::mat4f &matrix){
      glUniformMatrix4fv(uniform_location, 1, GL_FALSE, matrix.data());
    }

  private:
    GLuint id_;
    bool is_created_;
    std::string error_log_;
  };
  }
#endif // TORERO_SHADER_H
