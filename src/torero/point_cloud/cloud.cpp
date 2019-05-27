#include "torero/point_cloud/cloud.h"

namespace torero {
  namespace point_cloud {
    Cloud::Cloud(torero::gl::Shader *shader, const float point_size,
                 const float maximum_intensity_value) :
      shader_(shader),
      buffer_(true),
      buffer_id_(0u),
      buffer_size_(nullptr),
      point_cloud_(nullptr),
      initial_primary_model_{},
      initial_secondary_model_{},
      primary_model_(&initial_primary_model_),
      secondary_model_(&initial_secondary_model_),
      point_size_(point_size),
      maximum_intensity_value_(maximum_intensity_value),
      color_palette_(0),
      color_size_(4.0f),
      has_color_(false),
      has_alpha_(false),
      has_intensity_(false),
      has_buffer_(false),
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

    bool Cloud::set_color_palette(const std::vector<torero::ColorRGB> &color_palette){
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

    bool Cloud::set_color_palette(const torero::ColorRGB color){
      color_size_ = 1.0f;
      color_palette_.clear();
      color_palette_.push_back(algebraica::vec3f(color.rgb.red,
                                                 color.rgb.green,
                                                 color.rgb.blue) / 255.0f);
      return true;
    }

    void Cloud::set_color_mode(const torero::point_cloud::ColorMode color_mode){
      color_mode_ = color_mode >= torero::point_cloud::ColorMode::ColorMap
                    && color_mode <= torero::point_cloud::ColorMode::Data
                    ? static_cast<float>(color_mode) : 0.0f;
    }

    void Cloud::set_coordinate_frame(const torero::Pose3D *coordinate_frame){
      primary_model_ = coordinate_frame ? coordinate_frame : &initial_primary_model_;
    }

    void Cloud::set_pose(torero::Pose3D *pose){
      secondary_model_ = pose;
    }

    bool Cloud::set_point_size(const float point_size){
      const bool bigger_than_zero{point_size > 0.0f};
      point_size_ = bigger_than_zero ? point_size : 1.0f;
      return bigger_than_zero;
    }

    bool Cloud::set_maximum_intensity_value(const float maximum_intensity_value){
      const bool bigger_than_zero{maximum_intensity_value > 0.0f};
      maximum_intensity_value_ = bigger_than_zero ? maximum_intensity_value : 1.0f;
      return bigger_than_zero;
    }

    void Cloud::translate(const float x, const float y, const float z){
      secondary_model_->position.point.x += x;
      secondary_model_->position.point.y += y;
      secondary_model_->position.point.z += z;
    }

    void Cloud::translate(const algebraica::vec3f &translation){
      secondary_model_->position.point.x += translation.x;
      secondary_model_->position.point.y += translation.y;
      secondary_model_->position.point.z += translation.z;
    }

    void Cloud::rotate(const float pitch, const float yaw, const float roll){
      const algebraica::quaternionF rotation {
        algebraica::quaternionF::euler_to_quaternion(pitch, yaw, roll)
            * algebraica::quaternionF(secondary_model_->orientation.axes.x,
                                      secondary_model_->orientation.axes.y,
                                      secondary_model_->orientation.axes.z,
                                      secondary_model_->orientation.axes.w)};
      secondary_model_->orientation.axes.x = rotation.x;
      secondary_model_->orientation.axes.y = rotation.y;
      secondary_model_->orientation.axes.z = rotation.z;
      secondary_model_->orientation.axes.w = rotation.w;
    }

    void Cloud::rotate(const algebraica::quaternionF &rotation){
      const algebraica::quaternionF rotate {
        rotation * algebraica::quaternionF(secondary_model_->orientation.axes.x,
                                           secondary_model_->orientation.axes.y,
                                           secondary_model_->orientation.axes.z,
                                           secondary_model_->orientation.axes.w)};
      secondary_model_->orientation.axes.x = rotate.x;
      secondary_model_->orientation.axes.y = rotate.y;
      secondary_model_->orientation.axes.z = rotate.z;
      secondary_model_->orientation.axes.w = rotate.w;
    }

    void Cloud::rotate_in_x(const float angle){
      algebraica::quaternionF rotation(secondary_model_->orientation.axes.x,
                                       secondary_model_->orientation.axes.y,
                                       secondary_model_->orientation.axes.z,
                                       secondary_model_->orientation.axes.w);
      rotation.rotate(1, 0, 0, angle);

      secondary_model_->orientation.axes.x = rotation.x;
      secondary_model_->orientation.axes.y = rotation.y;
      secondary_model_->orientation.axes.z = rotation.z;
      secondary_model_->orientation.axes.w = rotation.w;
    }

    void Cloud::rotate_in_y(const float angle){
      algebraica::quaternionF rotation(secondary_model_->orientation.axes.x,
                                       secondary_model_->orientation.axes.y,
                                       secondary_model_->orientation.axes.z,
                                       secondary_model_->orientation.axes.w);
      rotation.rotate(0, 1, 0, angle);

      secondary_model_->orientation.axes.x = rotation.x;
      secondary_model_->orientation.axes.y = rotation.y;
      secondary_model_->orientation.axes.z = rotation.z;
      secondary_model_->orientation.axes.w = rotation.w;
    }

    void Cloud::rotate_in_z(const float angle){
      algebraica::quaternionF rotation(secondary_model_->orientation.axes.x,
                                       secondary_model_->orientation.axes.y,
                                       secondary_model_->orientation.axes.z,
                                       secondary_model_->orientation.axes.w);
      rotation.rotate(0, 0, 1, angle);

      secondary_model_->orientation.axes.x = rotation.x;
      secondary_model_->orientation.axes.y = rotation.y;
      secondary_model_->orientation.axes.z = rotation.z;
      secondary_model_->orientation.axes.w = rotation.w;
    }

    bool Cloud::update(){
      if(shader_->use() && !has_buffer_){
        buffer_.vertex_bind();

        data_size_ = size();
        buffer_.allocate_array(data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(torero::ShaderLocation::Position);
        buffer_.attributte_buffer(torero::ShaderLocation::Position, _3D, 0, type_size_);
        offset += offset_big_;
        if(has_color_){
          buffer_.enable(torero::ShaderLocation::Color);
          buffer_.attributte_buffer(torero::ShaderLocation::Color, _3D, offset, type_size_);
          offset += offset_big_;
        }
        if(has_intensity_){
          buffer_.enable(torero::ShaderLocation::Intensity);
          buffer_.attributte_buffer(torero::ShaderLocation::Intensity, _1D, offset, type_size_);
          offset += offset_small_;
        }
        if(has_alpha_){
          buffer_.enable(torero::ShaderLocation::Alpha);
          buffer_.attributte_buffer(torero::ShaderLocation::Alpha, _1D, offset, type_size_);
        }

        buffer_.vertex_release();
        return true;
      }
      return false;
    }

    bool Cloud::draw(){
      if(shader_->use() && (*buffer_size_ > 0 || data_size_ > 0)){
        shader_->set_value(u_primary_translation_, u_primary_orientation_, *primary_model_);
        shader_->set_value(u_secondary_translation_, u_secondary_orientation_, *secondary_model_);
        shader_->set_value(u_color_mode_, color_mode_);
        shader_->set_value(u_intensity_range_, maximum_intensity_value_);
        shader_->set_values(u_palette_, color_palette_.data(), color_size_);
        shader_->set_value(u_color_size_, color_size_);
        shader_->set_value(u_has_alpha_, has_alpha_);

        glPointSize(point_size_);
        if(has_buffer_){
          glBindVertexArray(buffer_id_);
          glDrawArrays(GL_POINTS, 0, *buffer_size_);
          glBindVertexArray(0);
        }else{
          buffer_.vertex_bind();
          glDrawArrays(GL_POINTS, 0, data_size_);
          buffer_.vertex_release();
        }
        glPointSize(1.0f);

        return true;
      }
      return false;
    }

    void Cloud::initialize(){
      shader_->use();
      // GLSL uniform locations
      u_primary_translation_   = shader_->uniform_location("u_primary_translation");
      u_primary_orientation_   = shader_->uniform_location("u_primary_orientation");
      u_secondary_translation_ = shader_->uniform_location("u_secondary_translation");
      u_secondary_orientation_ = shader_->uniform_location("u_secondary_orientation");
      u_palette_          = shader_->uniform_location("u_palette");
      u_color_size_       = shader_->uniform_location("u_color_size");
      u_color_mode_       = shader_->uniform_location("u_color_mode");
      u_intensity_range_  = shader_->uniform_location("u_intensity_range");
      u_has_alpha_        = shader_->uniform_location("u_has_alpha");
    }

    GLsizei Cloud::size(){
      switch(static_cast<unsigned int>(cloud_type_)){
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZ):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZ>*>
                                    (point_cloud_)->size());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZI):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZI>*>
                                    (point_cloud_)->size());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGB):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGB>*>
                                    (point_cloud_)->size());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGBI):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGBI>*>
                                    (point_cloud_)->size());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGBA):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZRGBA>*>
                                    (point_cloud_)->size());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZIAD):
        return static_cast<GLsizei>(static_cast<const std::vector<torero::PointXYZIAD>*>
                                    (point_cloud_)->size());
        default:
        return GLsizei(0);
      }
    }

    const GLvoid *Cloud::data(){
      switch(static_cast<unsigned int>(cloud_type_)){
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZ):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZ>*>
                                          (point_cloud_)->data());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZI):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZI>*>
                                          (point_cloud_)->data());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGB):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGB>*>
                                          (point_cloud_)->data());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGBI):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGBI>*>
                                          (point_cloud_)->data());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZRGBA):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZRGBA>*>
                                          (point_cloud_)->data());
        case static_cast<unsigned int>(torero::point_cloud::Type::XYZIAD):
        return static_cast<const GLvoid*>(static_cast<const std::vector<torero::PointXYZIAD>*>
                                          (point_cloud_)->data());
        default:
        return nullptr;
      }
    }

    void Cloud::set_cloud(const std::vector<torero::PointXYZ> *point_cloud){
      point_cloud_ = static_cast<const void*>(point_cloud);
      cloud_type_ = torero::point_cloud::Type::XYZ;
      type_size_ = sizeof(torero::PointXYZ);

      has_color_ = false;
      has_alpha_ = false;
      has_intensity_ = false;
      has_buffer_ = false;
    }

    void Cloud::set_cloud(const std::vector<torero::PointXYZI> *point_cloud){
      point_cloud_ = static_cast<const void*>(point_cloud);
      cloud_type_ = torero::point_cloud::Type::XYZI;
      type_size_ = sizeof(torero::PointXYZI);

      has_color_ = false;
      has_alpha_ = false;
      has_intensity_ = true;
      has_buffer_ = false;
    }

    void Cloud::set_cloud(const std::vector<torero::PointXYZRGB> *point_cloud){
      point_cloud_ = static_cast<const void*>(point_cloud);
      cloud_type_ = torero::point_cloud::Type::XYZRGB;
      type_size_ = sizeof(torero::PointXYZRGB);

      has_color_ = true;
      has_alpha_ = false;
      has_intensity_ = false;
      has_buffer_ = false;
    }

    void Cloud::set_cloud(const std::vector<torero::PointXYZRGBI> *point_cloud){
      point_cloud_ = static_cast<const void*>(point_cloud);
      cloud_type_ = torero::point_cloud::Type::XYZRGBI;
      type_size_ = sizeof(torero::PointXYZRGBI);

      has_color_ = true;
      has_alpha_ = false;
      has_intensity_ = true;
      has_buffer_ = false;
    }

    void Cloud::set_cloud(const std::vector<torero::PointXYZRGBA> *point_cloud){
      point_cloud_ = static_cast<const void*>(point_cloud);
      cloud_type_ = torero::point_cloud::Type::XYZRGBA;
      type_size_ = sizeof(torero::PointXYZRGBA);

      has_color_ = true;
      has_alpha_ = true;
      has_intensity_ = false;
      has_buffer_ = false;
    }

    void Cloud::set_cloud_buffer(const GLuint vertex_array_object_id,
                                 const GLsizei *cloud_size){
      cloud_type_ = torero::point_cloud::Type::XYZIAD;
      type_size_ = sizeof(torero::PointXYZIAD);
      buffer_size_ = cloud_size;
      buffer_id_ = vertex_array_object_id;

      has_color_ = false;
      has_alpha_ = false;
      has_intensity_ = true;
      has_buffer_ = true;
    }
  }
}
