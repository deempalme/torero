#include "include/line_grid.h"

namespace Toreo {
  LineGrid::LineGrid(Shader *line_shader, const float width, const float length,
                     const unsigned int quantity_width, const unsigned int quantity_length) :
    shader_(line_shader),
    buffer_(true),
    color_(255.0f, 255.0f, 255.0f, 255.0f),
    primary_model_(nullptr),
    identity_matrix_(),
    secondary_model_(),
    data_size_(0),
    i_position_(shader_->attribute_location("i_position")),
    u_primary_model_(shader_->uniform_location("u_primary_model")),
    u_secondary_model_(shader_->uniform_location("u_secondary_model")),
    u_color_(shader_->uniform_location("u_color")),
    u_fog_(shader_->uniform_location("u_fog")),
    fog_(1)
  {
    properties(width, length, quantity_width, quantity_length);
  }

  void LineGrid::fog(const bool foggy){
    fog_ = static_cast<int>(foggy);
  }

  void LineGrid::color(const float r, const float g, const float b, const float alpha){
    color_(r, g, b, alpha);
  }

  void LineGrid::properties(const float width, const float length,
                            const unsigned int line_quantity_through_width,
                            const unsigned int line_quantity_through_length){
    if(shader_->use()){
      GLsizei type_size{sizeof(algebraica::vec3f)};
      float half_width{width/2.0f};
      float half_length{length/2.0f};

      float width_separation{width/line_quantity_through_width};
      float length_separation{length/line_quantity_through_length};

      data_size_ = (line_quantity_through_width + line_quantity_through_length + 2) * 2;
      algebraica::vec3f line_points[data_size_];

      int e{-1};
      for(int i = 0; i <= line_quantity_through_width; ++i){
        line_points[++e](i * width_separation - half_width, 0, -half_length);
        line_points[++e](i * width_separation - half_width, 0,  half_length);
      }

      for(int i = 0; i <= line_quantity_through_length; ++i){
        line_points[++e](-half_width, 0, i * length_separation - half_length);
        line_points[++e]( half_width, 0, i * length_separation - half_length);
      }

      buffer_.vertex_bind();
      buffer_.allocate_array(line_points, data_size_ * type_size);

      buffer_.enable(i_position_);
      buffer_.attributte_buffer(i_position_, _3D, 0, type_size);
      buffer_.vertex_release();
    }
  }

  void LineGrid::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void LineGrid::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void LineGrid::transformation_matrix(const algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void LineGrid::draw(){
    if(shader_->use()){
      if(primary_model_)
        shader_->set_value(u_primary_model_, *primary_model_);
      else
        shader_->set_value(u_primary_model_, identity_matrix_);

      shader_->set_value(u_secondary_model_, secondary_model_);
      shader_->set_value(u_color_, color_);
      shader_->set_value(u_fog_, fog_);

      buffer_.vertex_bind();
      glDrawArrays(GL_LINES, 0, data_size_);
      buffer_.vertex_release();
    }
  }
}
