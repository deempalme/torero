#include "torero/covariance/covariance.h"

namespace torero {
  namespace covariance {
    Covariances::Covariances(torero::gl::Shader *shader,
                             const std::vector<torero::covariance::Covariance> *matrices,
                             torero::gl::Buffer *plane, const GLsizei plane_data_size) :
      shader_(shader),
      buffer_plane_(plane),
      buffer_data_(false),
      matrices_(matrices),
      pi_pi_twice_(static_cast<float>(4.0 * M_PI * M_PI)),
      primary_model_(nullptr),
      secondary_model_(),
      identity_matrix_(),
      type_size_(sizeof(torero::covariance::Shader)),
      data_size_(0),
      plane_data_size_(plane_data_size)
    {
      initialize();
    }

    void Covariances::change_input(const std::vector<torero::covariance::Covariance> *matrices){
      matrices_ = matrices;
      data_size_ = 0;
      update();
    }

    void Covariances::set_transformation_matrix(const algebraica::mat4f *transformation_matrix){
      primary_model_ = transformation_matrix;
    }

    void Covariances::translate(const float x, const float y, const float z){
      secondary_model_.translate(x, y, z);
    }

    void Covariances::rotate(const float pitch, const float yaw, const float roll){
      secondary_model_.rotate(pitch, yaw, roll);
    }

    void Covariances::rotate_in_x(const float angle){
      secondary_model_.rotate_x(angle);
    }

    void Covariances::rotate_in_y(const float angle){
      secondary_model_.rotate_y(angle);
    }

    void Covariances::rotate_in_z(const float angle){
      secondary_model_.rotate_z(angle);
    }

    bool Covariances::update(){
      const bool no_error{shader_->use()};

      if(no_error){
        data_size_ = matrices_->size();
        std::vector<torero::covariance::Shader> matrix_data(data_size_);

        std::size_t i{0};
        algebraica::mat2f invertible;
        for(const torero::covariance::Covariance &matrix : *matrices_){
          matrix_data[i].mean(-matrix.position.point.y,
                              matrix.position.point.z,
                              -matrix.position.point.x);
          invertible(matrix.matrix.m00, matrix.matrix.m01, matrix.matrix.m10, matrix.matrix.m11);
          matrix_data[i].constant = 1.0f / std::sqrt(pi_pi_twice_ * invertible.determinant());
          matrix_data[i].inverse = invertible.invert();
        }

        buffer_data_.vertex_bind();
        buffer_data_.allocate_array(matrix_data.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_data_.enable(i_mean_);
        buffer_data_.attributte_buffer(i_mean_, _3D, offset, type_size_);
        buffer_data_.divisor(i_mean_, 1);

        offset += sizeof(algebraica::vec3f);
        buffer_data_.enable(i_inverse_covariance_);
        buffer_data_.attributte_buffer(i_inverse_covariance_, _2D, offset, type_size_);
        buffer_data_.divisor(i_inverse_covariance_, 1);

        offset += sizeof(algebraica::vec2f);
        buffer_data_.enable(i_inverse_covariance_ + 1);
        buffer_data_.attributte_buffer(i_inverse_covariance_ + 1, _2D, offset, type_size_);
        buffer_data_.divisor(i_inverse_covariance_ + 1, 1);

        offset += sizeof(algebraica::vec2f);
        buffer_data_.enable(i_equation_constant_);
        buffer_data_.attributte_buffer(i_equation_constant_, _1D, offset, type_size_);
        buffer_data_.divisor(i_equation_constant_, 1);

        buffer_plane_->buffer_bind();
        buffer_plane_->enable(i_position_);
        buffer_plane_->attributte_buffer(i_position_, _2D, 0, plane_data_size_);
        buffer_plane_->buffer_release();

        buffer_data_.vertex_release();
      }
      return no_error;
    }

    bool Covariances::draw(){
      const bool no_error{shader_->use()};

      if(no_error){
        if(primary_model_)
          shader_->set_value(u_primary_model_, *primary_model_);
        else
          shader_->set_value(u_primary_model_, identity_matrix_);

        shader_->set_value(u_secondary_model_, secondary_model_);

        buffer_data_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, type_size_, data_size_);
        buffer_data_.vertex_release();
      }
      return no_error;
    }

    void Covariances::initialize(){
      if(shader_->use()){
        buffer_data_.create();
        // GLSL instanced attributes
        i_position_           = shader_->attribute_location("i_position");
        i_mean_               = shader_->attribute_location("i_mean");
        i_inverse_covariance_ = shader_->attribute_location("i_inverse_covariance");
        i_equation_constant_  = shader_->attribute_location("i_equation_constant");
        // GLSL uniform locations
        u_primary_model_      = shader_->uniform_location("u_primary_model");
        u_secondary_model_    = shader_->uniform_location("u_secondary_model");

        update();
      }
    }
  }
}
