#include "torero/ground.h"

namespace Toreo {
  Ground::Ground(Shader *ground_shader) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    maximum_height_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    calculate_height_(false),
    fog_visibility_(1),
    is_free_(0),
    is_polar_(0),
    is_grid_(0),
    has_height_(0),
    is_2D_(0),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(nullptr),
    ground_grid_(nullptr),
    free_ground_2D_(nullptr),
    free_ground_3D_(nullptr),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(0),
    data_size_(0),
    size_1_(sizeof(float)),
    size_2_(size_1_ * 2),
    size_3_(size_1_ * 3),
    size_4_(size_1_ * 4)
  {
    initialize();
  }

  void Ground::change_input(const std::vector<Visualizer::Ground2D> *ground){
    restart();
    ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::Ground2DShader);
    is_2D_ = 1;
  }

  void Ground::change_input(const std::vector<Visualizer::Ground3D> *ground){
    restart();
    ground_3D_ = ground;
    type_size_ = sizeof(Visualizer::Ground3DShader);
  }

  void Ground::change_input(const std::vector<Visualizer::GroundGrid> *ground){
    restart();
    ground_grid_ = ground;
    type_size_ = sizeof(Visualizer::GroundGridShader);
    is_grid_ = 1;
  }

  void Ground::change_input(const std::vector<Visualizer::FreeGround2D> *ground){
    restart();
    free_ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::FreeGround2D);
    is_free_ = 1;
    is_2D_ = 1;
  }

  void Ground::change_input(const std::vector<Visualizer::FreeGround3D> *ground){
    restart();
    free_ground_3D_ = ground;
    type_size_ = sizeof(Visualizer::FreeGround3D);
    is_free_ = 1;
  }

  void Ground::change_input(const std::vector<Visualizer::FreePolarGround2D> *ground){
    restart();
    polar_ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::FreePolarGround2D);
    is_free_ = 1;
    is_polar_ = 1;
    is_2D_ = 1;
  }

  void Ground::change_input(const std::vector<Visualizer::FreePolarGround3D> *ground){
    restart();
    polar_ground_3D_ = ground;
    type_size_ = sizeof(Visualizer::FreePolarGround3D);
    is_free_ = 1;
    is_polar_ = 1;
  }

  void Ground::set_transformation_matrix(const Algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void Ground::calculate_height(const bool calculate, const float maximum_height){
    calculate_height_ = calculate;
    has_height_ = (calculate)? 1 : 0;
    is_2D_ = (calculate)? 0 : 1;
    maximum_height_ = maximum_height;
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
    ground_position_(length_/2.0f - element_length_/2.0f,
                     width_/2.0f - element_width_/2.0f, 0.0f);
  }

  void Ground::fog_visibility(const bool visible){
    fog_visibility_ = (visible)? 1 : 0;
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
    bool no_error{shader_->use()};

    if(no_error){
      unsigned int o{0u}, u{0u}, r{0u}, c{0u};
      float quantity_width{static_cast<float>(quantity_width_)};
      float quantity_length{static_cast<float>(quantity_length_)};

      if(ground_2D_){
        data_size_ = quantity_width_ * quantity_length_;

        if(ground_2D_->size() >= data_size_){
          std::vector<Visualizer::Ground2DShader> ground(data_size_);

          r = 0u;
          for(float row = 0.0f; row < quantity_length; ++row, ++r){
            o = r * quantity_width_;
            c = 0u;
            for(float column = 0.0f; column < quantity_width; ++column, ++c){
              u = o + c;
              ground[u].position(column, row, 0.0f);
              ground[u].color((*ground_2D_)[u].r, (*ground_2D_)[u].g,
                              (*ground_2D_)[u].b, (*ground_2D_)[u].alpha);
            }
          }
          buffer_.vertex_bind();
          buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          buffer_.enable(i_position_);
          buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

          buffer_.enable(i_color_);
          buffer_.attributte_buffer(i_color_, _4D, size_3_, type_size_);

          buffer_.disable(i_dimension_);
          buffer_.disable(i_height_);
          buffer_.disable(i_probability_);

          buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_2D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(ground_3D_){
        data_size_ = quantity_width_ * quantity_length_;

        if(ground_3D_->size() >= data_size_){
          std::vector<Visualizer::Ground3DShader> ground(data_size_);

          r = 0u;
          for(float row = 0.0f; row < quantity_length; ++row, ++r){
            o = r * quantity_width_;
            c = 0u;
            for(float column = 0.0f; column < quantity_width; ++column, ++c){
              u = o + c;
              ground[u].position(column, row, 0.0f);
              ground[u].color((*ground_3D_)[u].r, (*ground_3D_)[u].g,
                              (*ground_3D_)[u].b, (*ground_3D_)[u].alpha);
              ground[u].height = (*ground_3D_)[u].height;
            }
          }
          buffer_.vertex_bind();
          buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          GLint offset{0};
          buffer_.enable(i_position_);
          buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

          offset += size_3_;
          buffer_.enable(i_color_);
          buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

          offset += size_4_;
          buffer_.enable(i_height_);
          buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

          buffer_.disable(i_dimension_);
          buffer_.disable(i_probability_);

          buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_3D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(ground_grid_){
        data_size_ = quantity_width_ * quantity_length_;

        if(ground_grid_->size() >= data_size_){
          std::vector<Visualizer::GroundGridShader> ground(data_size_);

          r = 0u;
          for(float row = 0.0f; row < quantity_length; ++row, ++r){
            o = r * quantity_width_;
            c = 0u;
            for(float column = 0.0f; column < quantity_width; ++column, ++c){
              u = o + c;
              ground[u].position(column, row, 0.0f);
              ground[u].probability = (*ground_grid_)[u].probability;
            }
          }
          buffer_.vertex_bind();
          buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          buffer_.enable(i_position_);
          buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

          buffer_.enable(i_probability_);
          buffer_.attributte_buffer(i_probability_, _1D, size_3_, type_size_);

          buffer_.disable(i_color_);
          buffer_.disable(i_dimension_);
          buffer_.disable(i_height_);

          buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_grid_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(free_ground_2D_){
        data_size_ = free_ground_2D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_2D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, offset, type_size_);

        offset += size_3_;
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += size_4_;
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        buffer_.disable(i_height_);
        buffer_.disable(i_probability_);

        buffer_.vertex_release();
      }else if(free_ground_3D_){
        data_size_ = free_ground_3D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_3D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

        offset += size_3_;
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += size_4_;
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        offset += size_2_;
        buffer_.enable(i_height_);
        buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

        buffer_.disable(i_probability_);

        buffer_.vertex_release();
      }else if(polar_ground_2D_){
        data_size_ = free_ground_2D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_2D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _2D, offset, type_size_);

        offset += size_2_;
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += size_4_;
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        buffer_.disable(i_height_);
        buffer_.disable(i_probability_);

        buffer_.vertex_release();
      }else if(polar_ground_3D_){
        data_size_ = polar_ground_3D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(polar_ground_3D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _2D, 0, type_size_);

        offset += size_2_;
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += size_4_;
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        offset += size_2_;
        buffer_.enable(i_height_);
        buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

        buffer_.disable(i_probability_);

        buffer_.vertex_release();
      }
    }else{
      std::cout << "You have not yet defined the ground size." << std::endl;
      no_error = false;
    }
    return no_error;
  }

  bool Ground::draw(){
    bool no_error{shader_->use()};

    if(no_error){
      if(primary_model_)
        shader_->set_value(u_primary_model_, *primary_model_);
      else
        shader_->set_value(u_primary_model_, identity_matrix_);
      shader_->set_value(u_secondary_model_, secondary_model_);

      shader_->set_value(u_2D_, is_2D_);
      shader_->set_value(u_fog_, fog_visibility_);
      shader_->set_value(u_width_, element_width_);
      shader_->set_value(u_length_, element_length_);
      shader_->set_value(u_position_, ground_position_);
      shader_->set_value(u_free_, is_free_);
      shader_->set_value(u_polar_, is_polar_);
      shader_->set_value(u_grid_, is_grid_);

      shader_->set_value(u_height_, has_height_);
      if(calculate_height_)
        shader_->set_value(u_max_height_, maximum_height_);

      buffer_.vertex_bind();
      glDrawArrays(GL_POINTS, 0, data_size_);
      buffer_.vertex_release();
    }

    return no_error;
  }

  void Ground::initialize(){
    shader_->use();
    // GLSL attribute locations
    i_position_        = shader_->attribute_location("i_position");
    i_color_           = shader_->attribute_location("i_color");
    i_dimension_       = shader_->attribute_location("i_dimension");
    i_height_          = shader_->attribute_location("i_height");
    i_probability_     = shader_->attribute_location("i_probability");
    // GLSL uniform locations
    u_primary_model_   = shader_->uniform_location("u_primary_model");
    u_secondary_model_ = shader_->uniform_location("u_secondary_model");
    u_fog_             = shader_->uniform_location("u_fog");
    u_width_           = shader_->uniform_location("u_width");
    u_length_          = shader_->uniform_location("u_length");
    u_height_          = shader_->uniform_location("u_height");
    u_max_height_      = shader_->uniform_location("u_max_height");
    u_2D_              = shader_->uniform_location("u_2D");
    u_position_        = shader_->uniform_location("u_position");
    u_free_            = shader_->uniform_location("u_free");
    u_polar_           = shader_->uniform_location("u_polar");
    u_grid_           = shader_->uniform_location("u_grid");
  }

  void Ground::restart(){
    ground_2D_ = nullptr;
    ground_3D_ = nullptr;
    ground_grid_ = nullptr;
    free_ground_2D_ = nullptr;
    free_ground_3D_ = nullptr;
    polar_ground_2D_ = nullptr;
    polar_ground_3D_ = nullptr;
    data_size_ = 0;
    is_free_ = 0;
    is_polar_ = 0;
    is_grid_ = 0;
    is_2D_ = 0;
  }
}
