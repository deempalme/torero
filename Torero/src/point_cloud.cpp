#include "includes/point_cloud.h"

PointCloud::PointCloud(Shader *shader_program,
                       const std::vector<Visualizer::pointXYZ> *point_cloud,
                       const Algebraica::vec3f rgb_color, const float point_size,
                       const float maximum_intensity_value) :
  shader_program_(shader_program),
  vertex_array_(),
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
  offset_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f))),
  offset_x2_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f) * 2))
{
  initialize();
}

PointCloud::PointCloud(Shader *shader_program,
                       const std::vector<Visualizer::pointXYZI> *point_cloud,
                       const Visualizer::ColorMode color_mode, const float point_size,
                       const float maximum_intensity_value) :
  shader_program_(shader_program),
  vertex_array_(),
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
  offset_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f))),
  offset_x2_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f) * 2))
{
  initialize();
}

PointCloud::PointCloud(Shader *shader_program,
                       const std::vector<Visualizer::pointXYZRGB> *point_cloud,
                       const float point_size, const float maximum_intensity_value) :
  shader_program_(shader_program),
  vertex_array_(),
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
  offset_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f))),
  offset_x2_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f) * 2))
{
  initialize();
}

PointCloud::PointCloud(Shader *shader_program,
                       const std::vector<Visualizer::pointXYZRGBA> *point_cloud,
                       const float point_size, const float maximum_intensity_value) :
  shader_program_(shader_program),
  vertex_array_(),
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
  offset_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f))),
  offset_x2_(reinterpret_cast<GLvoid*>(sizeof(Algebraica::vec3f) * 2))
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
  secondary_model_.translate(-y, z, -x);
}

void PointCloud::translate(const Algebraica::vec3f translation){
  secondary_model_.translate(-translation.y(), translation.z(), -translation.x());
}

void PointCloud::rotate(const float pitch, const float yaw, const float roll){
  secondary_model_.rotate(pitch, yaw, roll);
}

void PointCloud::rotate(const Algebraica::vec3f rotation){
  secondary_model_.rotate(rotation.x(), rotation.y(), rotation.z());
}

void PointCloud::rotate_in_x(const float x){
  secondary_model_.rotate_x(x);
}

void PointCloud::rotate_in_y(const float y){
  secondary_model_.rotate_y(y);
}

void PointCloud::rotate_in_z(const float z){
  secondary_model_.rotate_z(z);
}

const bool PointCloud::update(){
  const bool no_error{shader_program_->use()};

  if(no_error){
    switch(type_){
    case POINT_XYZ:
      data_size_ = point_cloud_xyz_->size();
      vertex_array_.allocate_array(point_cloud_xyz_->data(),
                                   data_size_ * type_size_, GL_DYNAMIC_DRAW);
      break;
    case POINT_XYZRGB:
      data_size_ = point_cloud_rgb_->size();
      vertex_array_.allocate_array(point_cloud_rgb_->data(),
                                   data_size_ * type_size_, GL_DYNAMIC_DRAW);
      break;
    case POINT_XYZRGBA:
      data_size_ = point_cloud_rgba_->size();
      vertex_array_.allocate_array(point_cloud_rgba_->data(),
                                   data_size_ * type_size_, GL_DYNAMIC_DRAW);
      break;
    default:
      data_size_ = point_cloud_xyzi_->size();
      vertex_array_.allocate_array(point_cloud_xyzi_->data(),
                                   data_size_ * type_size_, GL_DYNAMIC_DRAW);
      break;
    }
  }
  return no_error;
}

const bool PointCloud::draw(){
  const bool no_error{shader_program_->use()};

  if(no_error){
    vertex_array_.use();

    if(primary_model_)
      shader_program_->set_value(u_primary_model_, *primary_model_);
    else
      shader_program_->set_value(u_primary_model_, identity_matrix_);

    shader_program_->set_value(u_secondary_model_, secondary_model_);
    shader_program_->set_value(u_color_mode_, static_cast<float>(color_mode_));
    shader_program_->set_value(u_intensity_range_, maximum_intensity_value_);

    if(type_ == POINT_XYZ){
      shader_program_->set_values(u_palette_, &color_palette_[0], color_size_);
      shader_program_->set_value(u_color_size_, static_cast<float>(color_size_));

      glVertexAttribPointer(i_position_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, 0);
      glEnableVertexAttribArray(i_position_);

    }else if(type_ == POINT_XYZI){
      shader_program_->set_values(u_palette_, &color_palette_[0], color_size_);
      shader_program_->set_value(u_color_size_, static_cast<float>(color_size_));

      glVertexAttribPointer(i_position_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, 0);
      glVertexAttribPointer(i_intensity_, ONE_DIMENSION, GL_FLOAT, GL_FALSE, type_size_, offset_);

      glEnableVertexAttribArray(i_position_);
      glEnableVertexAttribArray(i_intensity_);

    }else if(type_ == POINT_XYZRGB){
      shader_program_->set_value(u_has_alpha_, false);

      glVertexAttribPointer(i_position_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, 0);
      glVertexAttribPointer(i_color_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, offset_);

      glEnableVertexAttribArray(i_position_);
      glEnableVertexAttribArray(i_color_);

    }else if(type_ == POINT_XYZRGBA){
      shader_program_->set_value(u_has_alpha_, true);

      glVertexAttribPointer(i_position_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, 0);
      glVertexAttribPointer(i_color_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE, type_size_, offset_);
      glVertexAttribPointer(i_alpha_, ONE_DIMENSION, GL_FLOAT, GL_FALSE, type_size_, offset_x2_);

      glEnableVertexAttribArray(i_position_);
      glEnableVertexAttribArray(i_color_);
      glEnableVertexAttribArray(i_alpha_);

    }

    glPointSize(point_size_);
    glDrawArrays(GL_POINTS, 0, data_size_);
    glPointSize(1.0f);

    glDisableVertexAttribArray(i_position_);
    if(type_ == POINT_XYZI){
      glDisableVertexAttribArray(i_intensity_);
    }else if(type_ == POINT_XYZRGB){
      glDisableVertexAttribArray(i_color_);
    }else if(type_ == POINT_XYZRGBA){
      glDisableVertexAttribArray(i_color_);
      glDisableVertexAttribArray(i_alpha_);
    }
  }
  return no_error;
}

void PointCloud::initialize(){
  // GLSL attribute locations
  i_position_         = shader_program_->attribute_location("i_position");
  i_intensity_        = shader_program_->attribute_location("i_intensity");
  i_color_            = shader_program_->attribute_location("i_color");
  i_alpha_            = shader_program_->attribute_location("i_alpha");
  // GLSL uniform locations
  u_primary_model_    = shader_program_->uniform_location("u_primary_model");
  u_secondary_model_  = shader_program_->uniform_location("u_secondary_model");
  u_palette_          = shader_program_->uniform_location("u_palette");
  u_color_size_       = shader_program_->uniform_location("u_color_size");
  u_color_mode_       = shader_program_->uniform_location("u_color_mode");
  u_intensity_range_  = shader_program_->uniform_location("u_intensity_range");
  u_has_alpha_        = shader_program_->uniform_location("u_has_alpha");

  update();
}
