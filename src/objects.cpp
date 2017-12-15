#include "include/objects.h"

namespace Toreo {
  Objects::Objects(Shader *shader_program, const std::vector<Visualizer::Object> *objects,
                   Buffer *hollow, Texture *texture, Buffer *solid, Buffer *arrow,
                   Texture *arrow_ao, const Visualizer::Shape type) :
    shader_(shader_program),
    buffer_hollow_data_(hollow),
    buffer_hollow_(true),
    buffer_solid_data_(solid),
    buffer_solid_(true),
    buffer_arrow_data_(arrow),
    buffer_arrow_(true),
    ao_(texture),
    ao_arrow_(arrow_ao),
    object_(objects),
    type_(type),
    type_size_arrow_(66),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    hollow_data_size_(0),
    solid_data_size_(0),
    arrow_data_size_(0),
    hollow_type_size_(sizeof(Visualizer::ObjectShaderHollow)),
    solid_type_size_(sizeof(Visualizer::ObjectShaderSolid)),
    buffer_size_(sizeof(Visualizer::ObjectBuffer))
  {
    initialize();
  }

  void Objects::change_input(const std::vector<Visualizer::Object> *objects){
    object_ = objects;
    hollow_data_size_ = 0;
    solid_data_size_ = 0;
    arrow_data_size_ = 0;
    update();
  }

  void Objects::set_transformation_matrix(const Algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void Objects::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void Objects::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void Objects::rotate_in_x(const float angle){
    secondary_model_.rotate_x(angle);
  }

  void Objects::rotate_in_y(const float angle){
    secondary_model_.rotate_y(angle);
  }

  void Objects::rotate_in_z(const float angle){
    secondary_model_.rotate_z(angle);
  }

  bool Objects::update(){
    bool no_error{shader_->use()};

    if(no_error){
      std::vector<Visualizer::ObjectShaderSolid> solid_data(0);
      std::vector<Visualizer::ObjectShaderHollow> hollow_data(0);
      std::vector<Visualizer::ObjectShaderSolid> arrow_data(0);

      for(Visualizer::Object object : *object_){
        if(object.solid){
          Visualizer::ObjectShaderSolid datum;
          datum.position(-object.y, object.z, -object.x);
          datum.rotation(object.pitch, object.yaw, object.roll);
          datum.color(object.r, object.g, object.b, object.alpha);
          datum.scale(object.width, object.height, object.length);
          if(type_ == Visualizer::SQUARE || type_ == Visualizer::CIRCLE)
            datum.scale[1] = 1.0f;
          solid_data.push_back(datum);
        }else{
          Visualizer::ObjectShaderHollow datum;
          datum.position(-object.y, object.z, -object.x);
          datum.rotation(object.pitch, object.yaw, object.roll);
          datum.color(object.r, object.g, object.b, object.alpha);
          datum.scale(object.width, object.height, object.length);
          if(type_ == Visualizer::SQUARE || type_ == Visualizer::CIRCLE)
            datum.scale[1] = 1.0f;
          datum.line_width = object.line_width;
          hollow_data.push_back(datum);
        }

        if(object.arrow){
          Visualizer::ObjectShaderSolid datum;
          datum.position(-object.y, object.z, -object.x);
          datum.rotation(object.arrow_pitch, object.arrow_yaw, object.arrow_roll);
          datum.color(object.r, object.g, object.b, object.alpha);
          datum.scale(1.0f, 1.0f, object.arrow_length);
          arrow_data.push_back(datum);
        }
      }

      hollow_data_size_ = hollow_data.size();

      if(hollow_data_size_ > 0){
        buffer_hollow_.vertex_bind();
        buffer_hollow_.allocate_array(hollow_data.data(), hollow_data_size_ * hollow_type_size_,
                                      GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_hollow_.enable(i_translation_);
        buffer_hollow_.attributte_buffer(i_translation_, _3D, offset, hollow_type_size_);
        buffer_hollow_.divisor(i_translation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_hollow_.enable(i_rotation_);
        buffer_hollow_.attributte_buffer(i_rotation_, _3D, offset, hollow_type_size_);
        buffer_hollow_.divisor(i_rotation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_hollow_.enable(i_color_);
        buffer_hollow_.attributte_buffer(i_color_, _4D, offset, hollow_type_size_);
        buffer_hollow_.divisor(i_color_, 1);

        offset += sizeof(Algebraica::vec4f);
        buffer_hollow_.enable(i_scale_);
        buffer_hollow_.attributte_buffer(i_scale_, _3D, offset, hollow_type_size_);
        buffer_hollow_.divisor(i_scale_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_hollow_.enable(i_line_width_);
        buffer_hollow_.attributte_buffer(i_line_width_, _1D, offset, hollow_type_size_);
        buffer_hollow_.divisor(i_line_width_, 1);

        buffer_hollow_data_->buffer_bind();
        offset = 0;
        buffer_hollow_data_->enable(i_position_);
        buffer_hollow_data_->attributte_buffer(i_position_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_hollow_data_->enable(i_normal_);
        buffer_hollow_data_->attributte_buffer(i_normal_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_hollow_data_->enable(i_uv_);
        buffer_hollow_data_->attributte_buffer(i_uv_, _2D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_hollow_data_->enable(i_scales_);
        buffer_hollow_data_->attributte_buffer(i_scales_, _3D, offset, buffer_size_);
        buffer_hollow_data_->buffer_release();

        buffer_hollow_.vertex_release();
      }

      solid_data_size_ = solid_data.size();

      if(solid_data_size_ > 0){
        buffer_solid_.vertex_bind();
        buffer_solid_.allocate_array(solid_data.data(), solid_data_size_ * solid_type_size_,
                                     GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_solid_.enable(i_translation_);
        buffer_solid_.attributte_buffer(i_translation_, _3D, offset, solid_type_size_);
        buffer_solid_.divisor(i_translation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_solid_.enable(i_rotation_);
        buffer_solid_.attributte_buffer(i_rotation_, _3D, offset, solid_type_size_);
        buffer_solid_.divisor(i_rotation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_solid_.enable(i_color_);
        buffer_solid_.attributte_buffer(i_color_, _4D, offset, solid_type_size_);
        buffer_solid_.divisor(i_color_, 1);

        offset += sizeof(Algebraica::vec4f);
        buffer_solid_.enable(i_scale_);
        buffer_solid_.attributte_buffer(i_scale_, _3D, offset, solid_type_size_);
        buffer_solid_.divisor(i_scale_, 1);

        buffer_solid_data_->buffer_bind();
        offset = 0;
        buffer_solid_data_->enable(i_position_);
        buffer_solid_data_->attributte_buffer(i_position_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_solid_data_->enable(i_normal_);
        buffer_solid_data_->attributte_buffer(i_normal_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_solid_data_->enable(i_uv_);
        buffer_solid_data_->attributte_buffer(i_uv_, _2D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_solid_data_->enable(i_scales_);
        buffer_solid_data_->attributte_buffer(i_scales_, _3D, offset, buffer_size_);
        buffer_solid_data_->buffer_release();

        buffer_solid_.vertex_release();
      }

      arrow_data_size_ = arrow_data.size();

      if(arrow_data_size_ > 0){
        buffer_arrow_.vertex_bind();
        buffer_arrow_.allocate_array(arrow_data.data(), arrow_data_size_ * solid_type_size_,
                                     GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_arrow_.enable(i_translation_);
        buffer_arrow_.attributte_buffer(i_translation_, _3D, offset, solid_type_size_);
        buffer_arrow_.divisor(i_translation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_arrow_.enable(i_rotation_);
        buffer_arrow_.attributte_buffer(i_rotation_, _3D, offset, solid_type_size_);
        buffer_arrow_.divisor(i_rotation_, 1);

        offset += sizeof(Algebraica::vec3f);
        buffer_arrow_.enable(i_color_);
        buffer_arrow_.attributte_buffer(i_color_, _4D, offset, solid_type_size_);
        buffer_arrow_.divisor(i_color_, 1);

        offset += sizeof(Algebraica::vec4f);
        buffer_arrow_.enable(i_scale_);
        buffer_arrow_.attributte_buffer(i_scale_, _3D, offset, solid_type_size_);
        buffer_arrow_.divisor(i_scale_, 1);

        buffer_arrow_data_->buffer_bind();
        offset = 0;
        buffer_arrow_data_->enable(i_position_);
        buffer_arrow_data_->attributte_buffer(i_position_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_arrow_data_->enable(i_normal_);
        buffer_arrow_data_->attributte_buffer(i_normal_, _3D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec3f);
        buffer_arrow_data_->enable(i_uv_);
        buffer_arrow_data_->attributte_buffer(i_uv_, _2D, offset, buffer_size_);

        offset += sizeof(Algebraica::vec2f);
        buffer_arrow_data_->enable(i_scales_);
        buffer_arrow_data_->attributte_buffer(i_scales_, _3D, offset, buffer_size_);
        buffer_arrow_data_->buffer_release();

        buffer_arrow_.vertex_release();
      }
    }
    return no_error;
  }

  bool Objects::draw(){
    const bool no_error{shader_->use()};

    if(no_error){
      if(primary_model_)
        shader_->set_value(u_primary_model_, *primary_model_);
      else
        shader_->set_value(u_primary_model_, identity_matrix_);

      shader_->set_value(u_secondary_model_, secondary_model_);
      shader_->set_value(u_solid_, 0.0f);

      if(hollow_data_size_ > 0){
        ao_->use();
        buffer_hollow_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, type_size_hollow_, hollow_data_size_);
        buffer_hollow_.vertex_release();
      }

      if(arrow_data_size_ > 0){
        ao_arrow_->use();
        buffer_arrow_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, type_size_arrow_, arrow_data_size_);
        buffer_arrow_.vertex_release();
      }

      shader_->set_value(u_solid_, 1.0f);

      if(solid_data_size_ > 0){
        buffer_solid_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, type_size_solid_, solid_data_size_);
        buffer_solid_.vertex_release();
      }
    }
    return no_error;
  }

  void Objects::initialize(){
    shader_->use();
    // GLSL instanced attributes
    i_position_        = shader_->attribute_location("i_position");
    i_normal_          = shader_->attribute_location("i_normal");
    i_uv_              = shader_->attribute_location("i_uv");
    i_scales_          = shader_->attribute_location("i_scales");
    // GLSL attribute locations
    i_translation_     = shader_->attribute_location("i_translation");
    i_rotation_        = shader_->attribute_location("i_rotation");
    i_color_           = shader_->attribute_location("i_color");
    i_scale_           = shader_->attribute_location("i_scale");
    i_line_width_      = shader_->attribute_location("i_line_width");
    // GLSL uniform locations
    u_primary_model_   = shader_->uniform_location("u_primary_model");
    u_secondary_model_ = shader_->uniform_location("u_secondary_model");
    u_solid_           = shader_->uniform_location("u_solid");

    switch(type_){
    case Visualizer::CYLINDER:
      type_size_hollow_ = 672;
      type_size_solid_ = 144;
      break;
    case Visualizer::BOX:
      type_size_hollow_ = 288;
      type_size_solid_ = 36;
      break;
    case Visualizer::SQUARE:
      type_size_hollow_ = 96;
      type_size_solid_ = 6;
      break;
    case Visualizer::CIRCLE:
      type_size_hollow_ = 336;
      type_size_solid_ = 42;
      break;
    }

    update();
  }
}
