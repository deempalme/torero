#include "torero/point_cloud.h"

namespace torero {
  PointCloud::PointCloud(Shader *shader, const float point_size,
                         const float maximum_intensity_value) :
    shader_(shader),
    buffer_(true),
    point_cloud_(nullptr),
    identity_matrix_(),
    primary_model_(&identity_matrix_),
    secondary_model_(),
    point_size_(point_size),
    maximum_intensity_value_(maximum_intensity_value),
    color_palette_(0),
    color_size_(4.0f),
    has_color_(false),
    has_alpha_(false),
    has_intensity_(false),
    type_size_(0),
    data_size_(0),
    offset_small_(sizeof(float)),
    offset_big_(offset_small_ * 3)
  {
    color_palette_.push_back(algebraica::vec3f(0.2f, 0.5f, 0.7f)); // Turquoise
    color_palette_.push_back(algebraica::vec3f(0.0f, 1.0f, 0.0f)); // Green
    color_palette_.push_back(algebraica::vec3f(1.0f, 1.0f, 0.0f)); // Yellow
    color_palette_.push_back(algebraica::vec3f(1.0f, 0.0f, 0.0f)); // Red

    initialize();
  }

  bool PointCloud::set_color_palette(const std::vector<torero::ColorRGB> &color_palette){
    std::size_t color_size{color_palette.size()};
    const bool bigger{color_size > 10u};

    color_size = (bigger)? 9 : color_size - 1;
    color_size_ = std::round(color_size);

    color_palette_.clear();

    for(std::size_t i = 0; i <= color_size; ++i)
      color_palette_.push_back(algebraica::vec3f(color_palette[i].rgb.red,
                                                 color_palette[i].rgb.green,
                                                 color_palette[i].rgb.blue) / 255.0f);
    return !bigger;
  }

  bool PointCloud::set_color_palette(const torero::ColorRGB color){
    color_size_ = 1.0f;
    color_palette_.clear();
    color_palette_.push_back(algebraica::vec3f(color.rgb.red,
                                               color.rgb.green,
                                               color.rgb.blue) / 255.0f);
    return true;
  }

  void PointCloud::set_color_mode(const torero::ColorMode color_mode){
    switch(static_cast<unsigned int>(color_mode)){
      case static_cast<unsigned int>(torero::ColorMode::ColorMap):
        color_mode_ = 1.0f;
      break;
      case static_cast<unsigned int>(torero::ColorMode::Solid):
        color_mode_ = 2.0f;
      break;
      case static_cast<unsigned int>(torero::ColorMode::Data):
        color_mode_ = 3.0f;
      break;
      default:
        color_mode_ = 0.0f;
      break;
    }
  }

  void PointCloud::set_transformation_matrix(const algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix ? transformation_matrix : &identity_matrix_;
  }

  bool PointCloud::set_point_size(const float point_size){
    const bool bigger_than_zero{point_size > 0.0f};
    point_size_ = bigger_than_zero ? point_size : 1.0f;
    return bigger_than_zero;
  }

  bool PointCloud::set_maximum_intensity_value(const float maximum_intensity_value){
    const bool bigger_than_zero{maximum_intensity_value > 0.0f};
    maximum_intensity_value_ = bigger_than_zero ? maximum_intensity_value : 1.0f;
    return bigger_than_zero;
  }

  void PointCloud::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void PointCloud::translate(const algebraica::vec3f translation){
    secondary_model_.translate(translation.x(), translation.y(), translation.z());
  }

  void PointCloud::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void PointCloud::rotate(const algebraica::vec3f rotation){
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

      data_size_ = size();
      buffer_.allocate_array(data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

      GLint offset{0};
      buffer_.enable(i_position_);
      buffer_.attributte_buffer(i_position_, _3D, 0, type_size_);
      offset += offset_big_;
      if(has_color_){
        buffer_.enable(i_color_);
        buffer_.attributte_buffer(i_color_, _3D, offset, type_size_);
        offset += offset_big_;
      }
      if(has_intensity_){
        buffer_.enable(i_intensity_);
        buffer_.attributte_buffer(i_intensity_, _1D, offset, type_size_);
        offset += offset_small_;
      }
      if(has_alpha_){
        buffer_.enable(i_alpha_);
        buffer_.attributte_buffer(i_alpha_, _1D, offset, type_size_);
      }

      buffer_.vertex_release();
    }
    return no_error;
  }

  bool PointCloud::draw(){
    bool no_error{shader_->use()};

    if(no_error){
      buffer_.vertex_bind();

      shader_->set_value(u_primary_model_, *primary_model_);
      shader_->set_value(u_secondary_model_, secondary_model_);
      shader_->set_value(u_color_mode_, color_mode_);
      shader_->set_value(u_intensity_range_, maximum_intensity_value_);

      shader_->set_values(u_palette_, color_palette_.data(), color_size_);
      shader_->set_value(u_color_size_, color_size_);
      shader_->set_value(u_has_alpha_, has_alpha_);

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
  }

  GLsizei PointCloud::size(){
    switch(static_cast<unsigned int>(cloud_type_)){
      case static_cast<unsigned int>(torero::PointCloudType::XYZ):
      return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZ>*>
                                  (point_cloud_)->size());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZI):
      return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZI>*>
                                  (point_cloud_)->size());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGB):
      return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGB>*>
                                  (point_cloud_)->size());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGBI):
      return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGBI>*>
                                  (point_cloud_)->size());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGBA):
      return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGBA>*>
                                  (point_cloud_)->size());
      break;
      default:
        return GLsizei(0);
      break;
    }
  }

  const GLvoid *PointCloud::data(){
    switch(static_cast<unsigned int>(cloud_type_)){
      case static_cast<unsigned int>(torero::PointCloudType::XYZ):
      return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZ>*>
                                        (point_cloud_)->data());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZI):
      return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZI>*>
                                        (point_cloud_)->data());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGB):
      return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGB>*>
                                        (point_cloud_)->data());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGBI):
      return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGBI>*>
                                        (point_cloud_)->data());
      break;
      case static_cast<unsigned int>(torero::PointCloudType::XYZRGBA):
      return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGBA>*>
                                        (point_cloud_)->data());
      break;
      default:
      return nullptr;
      break;
    }
  }

  void PointCloud::set_cloud(const std::vector<torero::PointXYZ> *point_cloud){
    point_cloud_ = point_cloud;
    cloud_type_ = torero::PointCloudType::XYZ;
    type_size_ = sizeof(torero::PointXYZ);
  }

  void PointCloud::set_cloud(const std::vector<torero::PointXYZI> *point_cloud){
    point_cloud_ = point_cloud;
    cloud_type_ = torero::PointCloudType::XYZI;
    type_size_ = sizeof(torero::PointXYZI);

    has_intensity_ = true;
  }

  void PointCloud::set_cloud(const std::vector<torero::PointXYZRGB> *point_cloud){
    point_cloud_ = point_cloud;
    cloud_type_ = torero::PointCloudType::XYZRGB;
    type_size_ = sizeof(torero::PointXYZRGB);

    has_color_ = true;
  }

  void PointCloud::set_cloud(const std::vector<torero::PointXYZRGBI> *point_cloud){
    point_cloud_ = point_cloud;
    cloud_type_ = torero::PointCloudType::XYZRGBI;
    type_size_ = sizeof(torero::PointXYZRGBI);

    has_color_ = true;
    has_intensity_ = true;
  }

  void PointCloud::set_cloud(const std::vector<torero::PointXYZRGBA> *point_cloud){
    point_cloud_ = point_cloud;
    cloud_type_ = torero::PointCloudType::XYZRGBA;
    type_size_ = sizeof(torero::PointXYZRGBA);

    has_color_ = true;
    has_alpha_ = true;
  }
}
