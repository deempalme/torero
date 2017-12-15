#include "include/ground.h"

namespace Toreo {
  Ground::Ground(Shader *ground_shader, Shader *lines_shader,
                 const std::vector<Visualizer::Ground2D> *ground) :
    ground_shader_(ground_shader),
    lines_shader_(lines_shader),
    ground_buffer_(true),
    lines_buffer_(true),
    width_(0.0f),
    length_(0.0f),
    element_width_(0.0f),
    element_length_(0.0f),
    quantity_width_(0u),
    quantity_length_(0u),
    line_visibility_(true),
    fog_visibility_(true),
    line_color_(255.0f, 255.0f, 255.0f, 255.0f),
    ground_position_(),
    ground_2D_(ground),
    ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::Ground2DShader)),
    line_type_size_(sizeof(Algebraica::vec3f)),
    data_size_(0),
    line_data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, Shader *lines_shader,
                 const std::vector<Visualizer::Ground3D> *ground) :
    ground_shader_(ground_shader),
    lines_shader_(lines_shader),
    ground_buffer_(true),
    lines_buffer_(true),
    width_(0.0f),
    length_(0.0f),
    element_width_(0.0f),
    element_length_(0.0f),
    quantity_width_(0u),
    quantity_length_(0u),
    line_visibility_(true),
    fog_visibility_(true),
    line_color_(255.0f, 255.0f, 255.0f, 255.0f),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(ground),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::Ground3DShader)),
    line_type_size_(sizeof(Algebraica::vec3f)),
    data_size_(0),
    line_data_size_(0)
  {
    initialize();
  }

  void Ground::change_input(const std::vector<Visualizer::Ground2D> *ground){
    ground_2D_ = ground;
    if(ground_3D_){
      ground_3D_ = nullptr;
      type_size_ = sizeof(Visualizer::Ground2DShader);
    }
  }

  void Ground::change_input(const std::vector<Visualizer::Ground3D> *ground){
    ground_3D_ = ground;
    if(ground_2D_){
      ground_2D_ = nullptr;
      type_size_ = sizeof(Visualizer::Ground3DShader);
    }
  }

  void Ground::set_transformation_matrix(const Algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void Ground::set_ground_size(const float width, const float length,
                               const unsigned int number_of_elements_through_width,
                               const unsigned int number_of_elements_through_length){
    width_ = width;
    length_ = length;
    element_width_ = width / number_of_elements_through_width;
    element_length_ = length / number_of_elements_through_length;
    quantity_width_ = number_of_elements_through_width;
    quantity_length_ = number_of_elements_through_length;
    data_size_ = quantity_width_ * quantity_length_;
    ground_position_(length_/2.0f - element_length_/2.0f,
                     width_/2.0f - element_width_/2.0f, 0.0f);
  }

  void Ground::set_lines(const unsigned int line_quantity_in_width,
                         const unsigned int line_quantity_in_length){
    lines_shader_->use();

    float half_width{width_/2.0f};
    float half_length{length_/2.0f};

    float width_separation{width_/line_quantity_in_width};
    float length_separation{length_/line_quantity_in_length};

    line_data_size_ = (line_quantity_in_width + line_quantity_in_length + 2) * 2;
    Algebraica::vec3f line_points[line_data_size_];

    int e{-1};
    for(int i = 0; i <= line_quantity_in_width; ++i){
      line_points[++e](i * width_separation - half_width, 0, -half_length);
      line_points[++e](i * width_separation - half_width, 0,  half_length);
    }

    for(int i = 0; i <= line_quantity_in_length; ++i){
      line_points[++e](-half_width, 0, i * length_separation - half_length);
      line_points[++e]( half_width, 0, i * length_separation - half_length);
    }

    lines_buffer_.vertex_bind();
    lines_buffer_.allocate_array(line_points, line_data_size_ * line_type_size_);

    lines_buffer_.enable(i_position_lines_);
    lines_buffer_.attributte_buffer(i_position_, _3D, 0, line_type_size_);
    lines_buffer_.vertex_release();
    line_visibility_ = true;
  }

  void Ground::line_color(const float r, const float g, const float b, const float alpha){
    line_color_(r, g, b, alpha);
  }

  void Ground::line_visibility(const bool visible){
    line_visibility_ = visible;
  }

  void Ground::fog_visibility(const bool visible){
    fog_visibility_ = visible;
  }

  void Ground::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void Ground::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void Ground::rotate(const Algebraica::vec3f rotation){
    secondary_model_.rotate(rotation.x(), rotation.y(), rotation.z());
  }

  void Ground::rotate_in_x(const float angle){
    secondary_model_.rotate_z(angle);
  }

  void Ground::rotate_in_y(const float angle){
    secondary_model_.rotate_x(angle);
  }

  void Ground::rotate_in_z(const float angle){
    secondary_model_.rotate_y(angle);
  }

  bool Ground::update(){
    bool no_error{ground_shader_->use()};

    if(no_error && data_size_ > 0){
      Visualizer::Ground2DShader ground_2d;
      Visualizer::Ground3DShader ground_3d;
      int o{0}, u{0};
      float quantity_width{static_cast<float>(quantity_width_)};
      float quantity_length{static_cast<float>(quantity_length_)};

      if(ground_2D_){
        if(ground_2D_->size() >= data_size_){
          std::vector<Visualizer::Ground2DShader> ground(data_size_);

          for(float i = 0; i < quantity_width; ++i){
            o = static_cast<int>(i * quantity_length);
            for(float e = 0; e < quantity_length; ++e){
              u = o + static_cast<int>(e);
              ground_2d.position(i, e);
              ground_2d.color(ground_2D_->at(u).r, ground_2D_->at(u).g,
                              ground_2D_->at(u).b, ground_2D_->at(u).alpha);
              ground.at(u) = ground_2d;
            }
          }
          ground_buffer_.vertex_bind();
          ground_buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          ground_buffer_.enable(i_position_);
          ground_buffer_.attributte_buffer(i_position_, _2D, 0, type_size_);

          ground_buffer_.enable(i_color_);
          ground_buffer_.attributte_buffer(i_color_, _4D, sizeof(Algebraica::vec2f), type_size_);

          ground_buffer_.disable(i_height_);

          ground_buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_2D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(ground_3D_){
        if(ground_3D_->size() >= data_size_){
          std::vector<Visualizer::Ground3DShader> ground(data_size_);

          for(float i = 0; i < quantity_width; ++i){
            o = static_cast<int>(i * quantity_length);
            for(float e = 0; e < quantity_length; ++e){
              u = o + static_cast<int>(e);
              ground_3d.position(i, e);
              ground_3d.color(ground_3D_->at(u).r, ground_3D_->at(u).g,
                              ground_3D_->at(u).b, ground_3D_->at(u).alpha);
              ground_3d.height = ground_3D_->at(u).height;
              ground.at(u) = ground_3d;
            }
          }
          ground_buffer_.vertex_bind();
          ground_buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          GLint offset{0};
          ground_buffer_.enable(i_position_);
          ground_buffer_.attributte_buffer(i_position_, _2D, 0, type_size_);

          offset += sizeof(Algebraica::vec2f);
          ground_buffer_.enable(i_color_);
          ground_buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

          offset += sizeof(Algebraica::vec4f);
          ground_buffer_.enable(i_height_);
          ground_buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

          ground_buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_3D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }
    }else{
      std::cout << "You have not yet defined the ground size." << std::endl;
      no_error = false;
    }
    return no_error;
  }

  bool Ground::draw(){
    bool no_error{lines_shader_->use()};

    if(no_error && line_visibility_){
      if(primary_model_)
        lines_shader_->set_value(u_primary_model_lines_, *primary_model_);
      else
        lines_shader_->set_value(u_primary_model_lines_, identity_matrix_);

      lines_shader_->set_value(u_secondary_model_lines_, secondary_model_);
      lines_shader_->set_value(u_color_lines_, line_color_);
      lines_shader_->set_value(u_fog_lines_, fog_visibility_);

      lines_buffer_.vertex_bind();
      glDrawArrays(GL_LINES, 0, line_data_size_);
      lines_buffer_.vertex_release();
    }

    no_error &= ground_shader_->use();

    if(no_error){
      if(primary_model_)
        ground_shader_->set_value(u_primary_model_, *primary_model_);
      else
        ground_shader_->set_value(u_primary_model_, identity_matrix_);
      ground_shader_->set_value(u_secondary_model_, secondary_model_);

      if(ground_2D_)
        ground_shader_->set_value(u_2D_, 1);
      else
        ground_shader_->set_value(u_2D_, 0);

      ground_shader_->set_value(u_fog_, fog_visibility_);
      ground_shader_->set_value(u_width_, element_width_);
      ground_shader_->set_value(u_length_, element_length_);
      ground_shader_->set_value(u_position_, ground_position_);

      ground_buffer_.vertex_bind();
      glDrawArrays(GL_POINTS, 0, data_size_);
      ground_buffer_.vertex_release();
    }

    return no_error;
  }

  void Ground::initialize(){
    ground_shader_->use();
    // GLSL attribute locations
    i_position_        = ground_shader_->attribute_location("i_position");
    i_color_           = ground_shader_->attribute_location("i_color");
    i_height_          = ground_shader_->attribute_location("i_height");
    // GLSL uniform locations
    u_primary_model_   = ground_shader_->uniform_location("u_primary_model");
    u_secondary_model_ = ground_shader_->uniform_location("u_secondary_model");
    u_fog_             = ground_shader_->uniform_location("u_fog");
    u_width_           = ground_shader_->uniform_location("u_width");
    u_length_          = ground_shader_->uniform_location("u_length");
    u_2D_              = ground_shader_->uniform_location("u_2D");
    u_position_        = ground_shader_->uniform_location("u_position");

    lines_shader_->use();
    // GLSL attribute locations
    i_position_lines_        = lines_shader_->attribute_location("i_position");
    // GLSL uniform locations
    u_color_lines_           = lines_shader_->uniform_location("u_color");
    u_primary_model_lines_   = lines_shader_->uniform_location("u_primary_model");
    u_secondary_model_lines_ = lines_shader_->uniform_location("u_secondary_model");
    u_fog_lines_             = lines_shader_->uniform_location("u_fog");

  }
}
