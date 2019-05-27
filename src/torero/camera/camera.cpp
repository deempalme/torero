#include "torero/camera/camera.h"

namespace torero {
  namespace camera {
    Camera::Camera(torero::gl::Shader *shader) :
      shader_(shader),
      buffer_(true),
      buffer_plane_(true),
      camera_(nullptr),
      dimensionality_(torero::camera::Dimensionality::ThreeDimensional),
      identity_matrix_(),
      primary_model_(&identity_matrix_),
      secondary_model_(),
      curvature_(1.0f),
      dimensions_(10.0f, 5.625f),
      type_size_(0),
      data_size_(0)
    {
      shader_->use();
      // GLSL attribute locations
      i_position_         = shader_->attribute_location("i_position");
      i_intensity_        = shader_->attribute_location("i_intensity");
      i_color_            = shader_->attribute_location("i_color");
      i_alpha_            = shader_->attribute_location("i_alpha");
      // GLSL uniform locations
      u_primary_model_    = shader_->uniform_location("u_primary_model");
      u_secondary_model_  = shader_->uniform_location("u_secondary_model");
    }

    void Camera::set_camera(const torero::image::File *video_input){
      camera_ = video_input;
    }

    void Camera::set_curvature(const float curvature){
      curvature_ = curvature;
    }

    void Camera::set_dimensions(const torero::camera::Dimensionality number_of_dimensions){
      dimensionality_ = number_of_dimensions;
    }

    void Camera::set_transformation_matrix(const algebraica::mat4f *transformation_matrix){
      primary_model_ = transformation_matrix ? transformation_matrix : &identity_matrix_;
    }

    void Camera::translate(const float x, const float y, const float z){
      secondary_model_.translate(x, y, z);
    }

    void Camera::translate(const algebraica::vec3f translation){
      secondary_model_.translate(translation.x, translation.y, translation.z);
    }

    void Camera::resize(const float width, const float height){
      if(width > 0)
        dimensions_[0] = width;
      if(height > 0)
        dimensions_[1] = height;
    }

    void Camera::rotate(const float pitch, const float yaw, const float roll){
      secondary_model_.rotate(pitch, yaw, roll);
    }

    void Camera::rotate(const algebraica::vec3f rotation){
      secondary_model_.rotate(rotation.x, rotation.y, rotation.z);
    }

    void Camera::rotate_in_x(const float angle){
      secondary_model_.rotate_x(angle);
    }

    void Camera::rotate_in_y(const float angle){
      secondary_model_.rotate_y(angle);
    }

    void Camera::rotate_in_z(const float angle){
      secondary_model_.rotate_z(angle);
    }

    bool Camera::update(){
      bool no_error{shader_->use()};

      if(no_error){/*
      buffer_.vertex_bind();

      data_size_ = type_size_;
      buffer_.allocate_array(data_, data_size_ * type_size_, GL_DYNAMIC_DRAW);

      GLint offset{0};
      buffer_.enable(i_position_);
      buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
      offset += offset_big_;
      buffer_.enable(i_color_);
      buffer_.attributte_buffer(i_color_, _3D, offset, type_size_);
      offset += offset_big_;
      buffer_.enable(i_intensity_);
      buffer_.attributte_buffer(i_intensity_, _1D, offset, type_size_);
      offset += offset_small_;
      buffer_.enable(i_alpha_);
      buffer_.attributte_buffer(i_alpha_, _1D, offset, type_size_);

      buffer_.vertex_release();*/
      }
      return no_error;
    }

    bool Camera::draw(){
      bool no_error{shader_->use()};

      if(no_error){
        buffer_.vertex_bind();

        shader_->set_value(u_primary_model_, *primary_model_);
        shader_->set_value(u_secondary_model_, secondary_model_);

        glDrawArrays(GL_POINTS, 0, data_size_);

        buffer_.vertex_release();
      }
      return no_error;
    }

    void Camera::prepare_plane(){
      float vertices[] = {
        // Plane in XY
        -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, // bottom-left
        -0.5f, +0.5f, +0.0f, +0.0f, +1.0f, // top-left
        +0.5f, -0.5f, +0.0f, +1.0f, +0.0f, // bottom-right
        +0.5f, +0.5f, +0.0f, +1.0f, +1.0f  // top-right
      };

      buffer_plane_.vertex_bind();
      buffer_plane_.allocate_array(&vertices, sizeof(vertices));

      GLsizei stride_size{sizeof(GLfloat) * 8};
      GLsizei offset_3d{sizeof(GLfloat) * 3};

      buffer_plane_.enable(0);
      buffer_plane_.attributte_buffer(0, _3D, 0, stride_size);

      buffer_plane_.enable(1);
      buffer_plane_.attributte_buffer(1, _2D, offset_3d, stride_size);

      buffer_plane_.vertex_release();
    }
  }
}
