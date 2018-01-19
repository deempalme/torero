#include "torero/ground.h"

namespace Toreo {
  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::Ground2D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(0),
    is_polar_(0),
    ground_position_(),
    ground_2D_(ground),
    ground_3D_(nullptr),
    free_ground_2D_(nullptr),
    free_ground_3D_(nullptr),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::Ground2DShader)),
    data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::Ground3D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(0),
    is_polar_(0),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(ground),
    free_ground_2D_(nullptr),
    free_ground_3D_(nullptr),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::Ground3DShader)),
    data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::FreeGround2D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(1),
    is_polar_(0),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(nullptr),
    free_ground_2D_(ground),
    free_ground_3D_(nullptr),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::FreeGround2D)),
    data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::FreeGround3D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(1),
    is_polar_(0),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(nullptr),
    free_ground_2D_(nullptr),
    free_ground_3D_(ground),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::FreeGround3D)),
    data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::FreePolarGround2D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(1),
    is_polar_(1),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(nullptr),
    free_ground_2D_(nullptr),
    free_ground_3D_(nullptr),
    polar_ground_2D_(ground),
    polar_ground_3D_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::FreePolarGround2D)),
    data_size_(0)
  {
    initialize();
  }

  Ground::Ground(Shader *ground_shader, const std::vector<Visualizer::FreePolarGround3D> *ground) :
    shader_(ground_shader),
    buffer_(true),
    width_(100.0f),
    length_(100.0f),
    element_width_(1.0f),
    element_length_(1.0f),
    quantity_width_(100u),
    quantity_length_(100u),
    fog_visibility_(true),
    is_free_(1),
    is_polar_(1),
    ground_position_(),
    ground_2D_(nullptr),
    ground_3D_(nullptr),
    free_ground_2D_(nullptr),
    free_ground_3D_(nullptr),
    polar_ground_2D_(nullptr),
    polar_ground_3D_(ground),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    type_size_(sizeof(Visualizer::FreePolarGround3D)),
    data_size_(0)
  {
    initialize();
  }

  void Ground::change_input(const std::vector<Visualizer::Ground2D> *ground){
    restart();
    ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::Ground2DShader);
    is_free_ = 0;
    is_polar_ = 0;
  }

  void Ground::change_input(const std::vector<Visualizer::Ground3D> *ground){
    restart();
    ground_3D_ = ground;
    type_size_ = sizeof(Visualizer::Ground3DShader);
    is_free_ = 0;
    is_polar_ = 0;
  }

  void Ground::change_input(const std::vector<Visualizer::FreeGround2D> *ground){
    restart();
    free_ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::FreeGround2D);
    is_free_ = 1;
    is_polar_ = 0;
  }

  void Ground::change_input(const std::vector<Visualizer::FreeGround3D> *ground){
    restart();
    free_ground_3D_ = ground;
    type_size_ = sizeof(Visualizer::FreeGround3D);
    is_free_ = 1;
    is_polar_ = 0;
  }

  void Ground::change_input(const std::vector<Visualizer::FreePolarGround2D> *ground){
    restart();
    polar_ground_2D_ = ground;
    type_size_ = sizeof(Visualizer::FreePolarGround2D);
    is_free_ = 1;
    is_polar_ = 1;
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
    bool no_error{shader_->use()};

    if(no_error){
      Visualizer::Ground2DShader ground_2d;
      Visualizer::Ground3DShader ground_3d;
      int o{0}, u{0};
      float quantity_width{static_cast<float>(quantity_width_)};
      float quantity_length{static_cast<float>(quantity_length_)};

      if(ground_2D_){
        data_size_ = quantity_width_ * quantity_length_;

        if(ground_2D_->size() >= data_size_){
          std::vector<Visualizer::Ground2DShader> ground(data_size_);

          for(float i = 0; i < quantity_width; ++i){
            o = static_cast<int>(i * quantity_length);
            for(float e = 0; e < quantity_length; ++e){
              u = o + static_cast<int>(e);
              ground_2d.position(i, e, 0.0f);
              ground_2d.color((*ground_2D_)[u].r, (*ground_2D_)[u].g,
                              (*ground_2D_)[u].b, (*ground_2D_)[u].alpha);
              ground[u] = ground_2d;
            }
          }
          buffer_.vertex_bind();
          buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          buffer_.enable(i_position_);
          buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

          buffer_.enable(i_color_);
          buffer_.attributte_buffer(i_color_, _4D, sizeof(Algebraica::vec3f), type_size_);

          buffer_.disable(i_dimension_);
          buffer_.disable(i_height_);

          buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_2D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(ground_3D_){
        data_size_ = quantity_width_ * quantity_length_;

        if(ground_3D_->size() >= data_size_){
          std::vector<Visualizer::Ground3DShader> ground(data_size_);

          for(float i = 0; i < quantity_width; ++i){
            o = static_cast<int>(i * quantity_length);
            for(float e = 0; e < quantity_length; ++e){
              u = o + static_cast<int>(e);
              ground_3d.position(i, e, 0.0f);
              ground_3d.color((*ground_3D_)[u].r, (*ground_3D_)[u].g,
                              (*ground_3D_)[u].b, (*ground_3D_)[u].alpha);
              ground_3d.height = (*ground_3D_)[u].height;
              ground[u] = ground_3d;
            }
          }
          buffer_.vertex_bind();
          buffer_.allocate_array(ground.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

          GLint offset{0};
          buffer_.enable(i_position_);
          buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

          offset += sizeof(Algebraica::vec3f);
          buffer_.enable(i_color_);
          buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

          buffer_.disable(i_dimension_);

          offset += sizeof(Algebraica::vec4f);
          buffer_.enable(i_height_);
          buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

          buffer_.vertex_release();
        }else
          std::cout << "Ground size does not matches, is:" << ground_3D_->size()
                    << ". Should be:" << data_size_ << std::endl;
      }else if(free_ground_2D_){
        data_size_ = free_ground_2D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_2D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, offset, type_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += sizeof(Algebraica::vec4f);
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        buffer_.disable(i_height_);

        buffer_.vertex_release();
      }else if(free_ground_3D_){
        data_size_ = free_ground_3D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_3D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += sizeof(Algebraica::vec4f);
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_.enable(i_height_);
        buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

        buffer_.vertex_release();
      }else if(polar_ground_2D_){
        data_size_ = free_ground_2D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(free_ground_2D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _2D, offset, type_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += sizeof(Algebraica::vec4f);
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        buffer_.disable(i_height_);

        buffer_.vertex_release();
      }else if(polar_ground_3D_){
        data_size_ = polar_ground_3D_->size();

        buffer_.vertex_bind();
        buffer_.allocate_array(polar_ground_3D_->data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _2D, 0, type_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

        offset += sizeof(Algebraica::vec4f);
        buffer_.enable(i_dimension_);
        buffer_.attributte_buffer(i_dimension_, _2D, offset, type_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_.enable(i_height_);
        buffer_.attributte_buffer(i_height_, _1D, offset, type_size_);

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

      if(ground_2D_ || free_ground_2D_ || polar_ground_2D_)
        shader_->set_value(u_2D_, 1);
      else
        shader_->set_value(u_2D_, 0);

      shader_->set_value(u_fog_, fog_visibility_);
      shader_->set_value(u_width_, element_width_);
      shader_->set_value(u_length_, element_length_);
      shader_->set_value(u_position_, ground_position_);
      shader_->set_value(u_free_, is_free_);
      shader_->set_value(u_polar_, is_polar_);

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
    // GLSL uniform locations
    u_primary_model_   = shader_->uniform_location("u_primary_model");
    u_secondary_model_ = shader_->uniform_location("u_secondary_model");
    u_fog_             = shader_->uniform_location("u_fog");
    u_width_           = shader_->uniform_location("u_width");
    u_length_          = shader_->uniform_location("u_length");
    u_2D_              = shader_->uniform_location("u_2D");
    u_position_        = shader_->uniform_location("u_position");
    u_free_            = shader_->uniform_location("u_free");
    u_polar_           = shader_->uniform_location("u_polar");
  }

  void Ground::restart(){
    ground_2D_ = nullptr;
    ground_3D_ = nullptr;
    free_ground_2D_ = nullptr;
    free_ground_3D_ = nullptr;
    polar_ground_2D_ = nullptr;
    polar_ground_3D_ = nullptr;
    data_size_ = 0;
  }
}
