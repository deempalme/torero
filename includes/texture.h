#ifndef TORERO_TEXTURE_H
#define TORERO_TEXTURE_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "includes/definitions.h"
#include "includes/types.h"

#include <string>

namespace Toreo{
  class Texture
  {
  public:
    Texture(const GLuint active_texture, const GLfloat max_anisotropic_filtering,
            Visualizer::ImageFile *texture = nullptr) :
      id_(0),
      active_texture_(active_texture),
      max_filtering_(max_anisotropic_filtering),
      is_created_(false),
      error_log_("Texture not created yet...\n----------\n")
    {
      if(texture)
        create(texture);
    }
    ~Texture(){
      if(is_created_)
        glDeleteTextures(1, &id_);
    }

    // Creates the texture object and loads the data. Requires the location's id from shader program
    bool create(Visualizer::ImageFile *texture){
      error_log_.clear();
      if(texture->data && !is_created_){
        glActiveTexture(GL_TEXTURE0 + active_texture_);
        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_filtering_);

        write_data_to_opengl(texture);

        is_created_ = true;
        return true;
      }else{
        if(!texture->data)
          error_log_ += "Texture data does not exist...\n----------\n";
        if(is_created_)
          error_log_ += "Texture already created...\n----------\n";
        return false;
      }
    }
    // returns true if the shader program was properly created
    bool is_created(){
      return is_created_;
    }
    // if create() or is_created() are false, this will return the error's description of the failure
    const std::string error_log(){
      return error_log_;
    }
    // activate this texture
    bool use(){
      if(is_created_){
        glActiveTexture(GL_TEXTURE0 + active_texture_);
        glBindTexture(GL_TEXTURE_2D, id_);
      }
      return is_created_;
    }
    // Updates the actual texture for a new one
    bool update(Visualizer::ImageFile *new_texture){
      if(is_created_){
        use();
        write_data_to_opengl(new_texture);
        return is_created_;
      }else{
        return create(new_texture);
      }
    }
    // returns the program id
    const GLuint id(){
      return id_;
    }
    // returns the program texture unit
    const GLuint texture_unit(){
      return active_texture_;
    }

  private:
    void write_data_to_opengl(Visualizer::ImageFile *texture){
      switch(texture->components_size){
      case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture->width,
                     texture->height, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
        break;
      case 2:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, texture->width,
                     texture->height, 0, GL_RG, GL_UNSIGNED_BYTE, texture->data);
        break;
      case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width,
                     texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
        break;
      default:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width,
                     texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
        break;
      }
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    GLuint id_;
    GLuint active_texture_;
    GLfloat max_filtering_;
    bool is_created_;
    std::string error_log_;
  };
}

#endif // TORERO_TEXTURE_H
