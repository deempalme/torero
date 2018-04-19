#include "torero/point_cloud.h"

namespace Toreo {
  PointCloud::PointCloud(Shader *shader_program,
                         const std::vector<Visualizer::pointXYZ> *point_cloud,
                         const Algebraica::vec3f rgb_color, const float point_size,
                         const float maximum_intensity_value) :
    shader_(shader_program),
    buffer_(true),
    point_cloud_xyz_(point_cloud),
    point_cloud_xyzi_(nullptr),
    point_cloud_rgb_(nullptr),
    point_cloud_rgba_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    color_mode_(Visualizer::MONOCHROME),
    point_size_(point_size),
    maximum_intensity_value_(maximum_intensity_value),
    color_palette_{rgb_color},
    type_(POINT_XYZ),
    color_size_(1),
    type_size_(sizeof(Visualizer::pointXYZ)),
    data_size_(0),
    offset_(sizeof(Algebraica::vec3f)),
    offset_x2_(offset_ + offset_)
  {
    initialize();
  }

  PointCloud::PointCloud(Shader *shader_program,
                         const std::vector<Visualizer::pointXYZI> *point_cloud,
                         const Visualizer::ColorMode color_mode, const float point_size,
                         const float maximum_intensity_value) :
    shader_(shader_program),
    buffer_(true),
    point_cloud_xyz_(nullptr),
    point_cloud_xyzi_(point_cloud),
    point_cloud_rgb_(nullptr),
    point_cloud_rgba_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    color_mode_(color_mode),
    point_size_(point_size),
    maximum_intensity_value_(maximum_intensity_value),
    color_palette_{Algebraica::vec3f(0.2, 0.5, 0.7), Algebraica::vec3f(0, 1, 0),
                   Algebraica::vec3f(1, 1, 0), Algebraica::vec3f(1, 0, 0)},
    type_(POINT_XYZI),
    color_size_(4),
    type_size_(sizeof(Visualizer::pointXYZI)),
    data_size_(0),
    offset_(sizeof(Algebraica::vec3f)),
    offset_x2_(offset_ + offset_)
  {
    initialize();
  }

  PointCloud::PointCloud(Shader *shader_program,
                         const std::vector<Visualizer::pointXYZRGB> *point_cloud,
                         const float point_size, const float maximum_intensity_value) :
    shader_(shader_program),
    buffer_(true),
    point_cloud_xyz_(nullptr),
    point_cloud_xyzi_(nullptr),
    point_cloud_rgb_(point_cloud),
    point_cloud_rgba_(nullptr),
    primary_model_(nullptr),
    secondary_model_(),
    color_mode_(Visualizer::NONE),
    point_size_(point_size),
    maximum_intensity_value_(maximum_intensity_value),
    color_palette_{},
    type_(POINT_XYZRGB),
    color_size_(0),
    type_size_(sizeof(Visualizer::pointXYZRGB)),
    data_size_(0),
    offset_(sizeof(Algebraica::vec3f)),
    offset_x2_(offset_ + offset_)
  {
    initialize();
  }

  PointCloud::PointCloud(Shader *shader_program,
                         const std::vector<Visualizer::pointXYZRGBA> *point_cloud,
                         const float point_size, const float maximum_intensity_value) :
    shader_(shader_program),
    buffer_(true),
    point_cloud_xyz_(nullptr),
    point_cloud_xyzi_(nullptr),
    point_cloud_rgb_(nullptr),
    point_cloud_rgba_(point_cloud),
    primary_model_(nullptr),
    secondary_model_(),
    color_mode_(Visualizer::NONE),
    point_size_(point_size),
    maximum_intensity_value_(maximum_intensity_value),
    color_palette_{},
    type_(POINT_XYZRGBA),
    color_size_(0),
    type_size_(sizeof(Visualizer::pointXYZRGBA)),
    data_size_(0),
    offset_(sizeof(Algebraica::vec3f)),
    offset_x2_(offset_ + offset_)
  {
    initialize();
  }

  void PointCloud::change_input(const std::vector<Visualizer::pointXYZ> *point_cloud){
    point_cloud_xyz_ = point_cloud;
    if(type_ != POINT_XYZ){
      point_cloud_xyzi_ = nullptr;
      point_cloud_rgb_ = nullptr;
      point_cloud_rgba_ = nullptr;
      color_mode_ = Visualizer::MONOCHROME;
      type_ = POINT_XYZ;
      color_size_ = 1;
      type_size_ = sizeof(Visualizer::pointXYZ);
    }
    data_size_ = 0;
    restart();
  }

  void PointCloud::change_input(const std::vector<Visualizer::pointXYZI> *point_cloud){
    point_cloud_xyzi_ = point_cloud;
    if(type_ != POINT_XYZI){
      point_cloud_xyz_ = nullptr;
      point_cloud_rgb_ = nullptr;
      point_cloud_rgba_ = nullptr;
      color_mode_ = Visualizer::VARIABLE;
      color_palette_[0] = Algebraica::vec3f(0.2, 0.5, 0.7); //grayish blue
      color_palette_[1] = Algebraica::vec3f(0, 1, 0);       //green
      color_palette_[2] = Algebraica::vec3f(1, 1, 0);       //yellow
      color_palette_[3] = Algebraica::vec3f(1, 0, 0);       //red
      type_ = POINT_XYZI;
      color_size_ = 4;
      type_size_ = sizeof(Visualizer::pointXYZI);
    }
    data_size_ = 0;
    restart();
  }

  void PointCloud::change_input(const std::vector<Visualizer::pointXYZRGB> *point_cloud){
    point_cloud_rgb_ = point_cloud;
    if(type_ != POINT_XYZRGB){
      point_cloud_xyz_ = nullptr;
      point_cloud_xyzi_ = nullptr;
      point_cloud_rgba_ = nullptr;
      color_mode_ = Visualizer::NONE;
      type_ = POINT_XYZRGB;
      color_size_ = 0;
      type_size_ = sizeof(Visualizer::pointXYZRGB);
    }
    data_size_ = 0;
    restart();
  }

  void PointCloud::change_input(const std::vector<Visualizer::pointXYZRGBA> *point_cloud){
    point_cloud_rgba_ = point_cloud;
    if(type_ != POINT_XYZRGBA){
      point_cloud_xyz_ = nullptr;
      point_cloud_xyzi_ = nullptr;
      point_cloud_rgb_ = nullptr;
      color_mode_ = Visualizer::NONE;
      type_ = POINT_XYZRGBA;
      color_size_ = 0;
      type_size_ = sizeof(Visualizer::pointXYZRGBA);
    }
    data_size_ = 0;
    restart();
  }

  void PointCloud::set_colormap(const Algebraica::vec3f *colors, const unsigned int quantity){
    color_size_ = (quantity > 10)? 9 : quantity - 1;

    for(uint i = 0; i < 10; i++)
      if(i < quantity)
        color_palette_[i] = Algebraica::vec3f(*(colors + i)) / 255.0f;
      else
        color_palette_[i] = Algebraica::vec3f();
  }

  void PointCloud::set_color_mode(const Visualizer::ColorMode color_mode){
    color_mode_ = color_mode;
  }

  void PointCloud::set_transformation_matrix(const Algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void PointCloud::set_point_size(const float point_size){
    point_size_ = point_size;
  }

  void PointCloud::set_maximum_intensity_value(const float maximum_intensity_value){
    maximum_intensity_value_ = maximum_intensity_value;
  }

  void PointCloud::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void PointCloud::translate(const Algebraica::vec3f translation){
    secondary_model_.translate(translation.x(), translation.y(), translation.z());
  }

  void PointCloud::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void PointCloud::rotate(const Algebraica::vec3f rotation){
    secondary_model_.rotate(rotation.x(), rotation.y(), rotation.z());
  }

  void PointCloud::rotate_in_x(const float angle){
    secondary_model_.rotate_x(angle);
  }

  void PointCloud::rotate_in_y(const float angle){
    secondary_model_.rotate_y(angle);
  }

  void PointCloud::rotate_in_z(const float angle){
    secondary_model_.rotate_z(angle);
  }

  bool PointCloud::update(){
    bool no_error{shader_->use()};

    if(no_error){
      buffer_.vertex_bind();
      switch(type_){
      case POINT_XYZ:
        data_size_ = point_cloud_xyz_->size();
        buffer_.allocate_array(point_cloud_xyz_->data(),
                               data_size_ * type_size_, GL_DYNAMIC_DRAW);
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
        break;
      case POINT_XYZRGB:
        data_size_ = point_cloud_rgb_->size();
        buffer_.allocate_array(point_cloud_rgb_->data(),
                               data_size_ * type_size_, GL_DYNAMIC_DRAW);
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _3D, offset_, type_size_);
        buffer_.enable(i_alpha_);
        break;
      case POINT_XYZRGBA:
        data_size_ = point_cloud_rgba_->size();
        buffer_.allocate_array(point_cloud_rgba_->data(),
                               data_size_ * type_size_, GL_DYNAMIC_DRAW);
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _3D, offset_, type_size_);
        buffer_.enable(i_alpha_);
        buffer_.attributte_buffer(i_alpha_, _1D, offset_x2_, type_size_);
        break;
      default:
        data_size_ = point_cloud_xyzi_->size();
        buffer_.allocate_array(point_cloud_xyzi_->data(),
                               data_size_ * type_size_, GL_DYNAMIC_DRAW);
        buffer_.enable(i_position_);
        buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
        buffer_.enable(i_intensity_);
        buffer_.attributte_buffer(i_intensity_, _1D, offset_, type_size_);
        break;
      }
      buffer_.vertex_release();
    }
    return no_error;
  }

  bool PointCloud::draw(){
    bool no_error{shader_->use()};

    if(no_error){
      buffer_.vertex_bind();

      if(primary_model_)
        shader_->set_value(u_primary_model_, *primary_model_);
      else
        shader_->set_value(u_primary_model_, identity_matrix_);

      shader_->set_value(u_secondary_model_, secondary_model_);
      shader_->set_value(u_color_mode_, static_cast<float>(color_mode_));
      shader_->set_value(u_intensity_range_, maximum_intensity_value_);

      if(type_ == POINT_XYZ){
        shader_->set_values(u_palette_, &color_palette_[0], color_size_);
        shader_->set_value(u_color_size_, static_cast<float>(color_size_));
      }else if(type_ == POINT_XYZI){
        shader_->set_values(u_palette_, &color_palette_[0], color_size_);
        shader_->set_value(u_color_size_, static_cast<float>(color_size_));
      }else if(type_ == POINT_XYZRGB)
        shader_->set_value(u_has_alpha_, false);
      else if(type_ == POINT_XYZRGBA)
        shader_->set_value(u_has_alpha_, true);

      glPointSize(point_size_);
      glDrawArrays(GL_POINTS, 0, data_size_);
      glPointSize(1.0f);

      buffer_.vertex_release();
    }
    return no_error;
  }

  void PointCloud::initialize(){
    shader_->use();
    // GLSL attribute locations
    i_position_         = shader_->attribute_location("i_position");
    i_intensity_        = shader_->attribute_location("i_intensity");
    i_color_            = shader_->attribute_location("i_color");
    i_alpha_            = shader_->attribute_location("i_alpha");
    // GLSL uniform locations
    u_primary_model_    = shader_->uniform_location("u_primary_model");
    u_secondary_model_  = shader_->uniform_location("u_secondary_model");
    u_palette_          = shader_->uniform_location("u_palette");
    u_color_size_       = shader_->uniform_location("u_color_size");
    u_color_mode_       = shader_->uniform_location("u_color_mode");
    u_intensity_range_  = shader_->uniform_location("u_intensity_range");
    u_has_alpha_        = shader_->uniform_location("u_has_alpha");

    update();
  }

  void PointCloud::restart(){
    shader_->use();
    buffer_.vertex_bind();
    buffer_.disable(i_position_);
    buffer_.disable(i_color_);
    buffer_.disable(i_intensity_);
    buffer_.disable(i_alpha_);
    buffer_.vertex_release();
  }
}
