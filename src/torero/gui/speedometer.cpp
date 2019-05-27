#include "torero/core.h"
#include "torero/gui/speedometer.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"

namespace torero {
  namespace gui {
    Speedometer::Speedometer(torero::Core *core, torero::gl::Shader *pbr, torero::gl::Shader *text,
                             const std::vector<torero::text::FontCharacter> *characters,
                             const std::string &folder_path) :
      core_(core),
      folder_path_(folder_path),
      folder_exist_(false),
      is_ready_(false),
      is_loaded_(false),
      pbr_(pbr),

      i_angle_(pbr_->attribute_location("i_angle")),
      i_color_(pbr_->attribute_location("i_color")),

      u_rotation_(pbr_->uniform_location("u_rotation")),
      u_translation_(pbr_->uniform_location("u_translation")),
      u_light_(pbr_->uniform_location("u_light")),
      u_light_color_(pbr_->uniform_location("u_light_color")),

      t_albedo_(nullptr),
      t_normal_(nullptr),
      t_pbr_emission_(nullptr),

      data_stride_size_(sizeof(torero::ComplexShaderData)),
      object_stride_size_(sizeof(torero::gui::PBR_Shader)),

      buffer_background_(true),
      buffer_background_objects_(true),
      background_data_size_(0),
      background_objects_(1, { 0.0f, {{1.0f, 1.0f, 1.0f}} }),

      buffer_marker_(true),
      buffer_marker_objects_(true),
      marker_data_size_(0),
      previous_speed_level_(0),
      previous_rpm_level_(0),
      marker_objects_(30, { 0.0f, {{0.220f, 0.855f, 1.0f}} }),

      buffer_needle_(true),
      buffer_needle_objects_(true),
      needle_data_size_(0),
      needle_objects_(2, { 0.0f, {{0.799f, 0.076f, 0.014f}} }),

      text_(text),
      i_size_(text_->attribute_location("i_size")),
      u_t_rotation_(text_->uniform_location("u_rotation")),
      u_t_translation_(text_->uniform_location("u_translation")),
      buffer_text_(true),
      text_data_size_(0u),
      text_stride_size_(sizeof(torero::gui::TextShader)),
      characters_(characters),

      previous_rotation_(0.0f),
      rotation_(algebraica::quaternionF::euler_to_quaternion(0.0f, to_radians(5.0f), to_radians(18.0f))),
      translation_(3.5f),
      light_color_(0.799f, 0.076f, 0.014f),
      light_(0.0f, 1.0f, 0.0f)
    {
      if(check_folder()){
        folder_exist_ = true;

        for(std::size_t i = 0; i < 22; ++i)
          marker_objects_[i].angle = k_190_degrees - k_10_degrees * i;

        for(std::size_t i = 22; i < 30; ++i){
          marker_objects_[i].angle = -k_42_degrees - k_15_degrees * (i - 22);
          if(i == 22 || i == 23){
            marker_objects_[i].color.rgb.red = 0.965f;
            marker_objects_[i].color.rgb.green = 0.357f;
            marker_objects_[i].color.rgb.blue = 0.463f;
          }
        }

        needle_objects_[1].angle = k_22_degrees;
      }

      core_->multithread_add_process(boost::bind(&gui::Speedometer::run, this),
                                     boost::bind(&gui::Speedometer::ready, this),
                                     &protector_);
    }

    Speedometer::~Speedometer(){
      if(t_albedo_) delete t_albedo_;
      if(t_normal_) delete t_normal_;
      if(t_pbr_emission_) delete t_pbr_emission_;
    }

    void Speedometer::update(float speed, float rpm){
      bool speed_changed{false}, rpm_changed{false};

      update_text(speed, rpm);

      speed = (speed > 210.0f)? 210.0f : speed;
      rpm = (rpm > 7000.0f)? 7000.0f : rpm;

      speed /= 210.0f;
      rpm /= 7000.0f;

      needle_objects_[0].angle = -speed * k_210_degrees;
      needle_objects_[1].angle = k_22_degrees + rpm * k_105_degrees;
      update_buffer(&buffer_needle_, &buffer_needle_objects_, 2, needle_objects_);

      const std::size_t speed_position {static_cast<std::size_t>(std::floor(speed * 21.0f) + 1.0f)};

      if(speed_position != previous_speed_level_){
        for(std::size_t i = 0; i < 22; ++i)
          if(i < speed_position){
            marker_objects_[i].color.rgb.red = 0.6784f;
            marker_objects_[i].color.rgb.green = 0.9569f;
            marker_objects_[i].color.rgb.blue = 0.2588f;
          }else{
            marker_objects_[i].color.rgb.red = 0.220f;
            marker_objects_[i].color.rgb.green = 0.855f;
            marker_objects_[i].color.rgb.blue = 1.0f;
          }
        previous_speed_level_ = speed_position;
        speed_changed = true;
      }

      const std::size_t rpm_position {-static_cast<std::size_t>(std::floor(rpm * 7.0f) + 1.0f) + 30};

      if(rpm_position != previous_rpm_level_){
        for(std::size_t i = 22; i < 30; ++i)
          if(rpm_position <= 23){
            marker_objects_[i].color.rgb.red = 1.0f;
            marker_objects_[i].color.rgb.green = 0.36f;
            marker_objects_[i].color.rgb.blue = 0.36f;
          }else if(i < rpm_position){
            if(i == 22 || i == 23){
              marker_objects_[i].color.rgb.red = 0.965f;
              marker_objects_[i].color.rgb.green = 0.357f;
              marker_objects_[i].color.rgb.blue = 0.463f;
            }else{
              marker_objects_[i].color.rgb.red = 0.220f;
              marker_objects_[i].color.rgb.green = 0.855f;
              marker_objects_[i].color.rgb.blue = 1.0f;
            }
          }else{
            marker_objects_[i].color.rgb.red = 0.9569f;
            marker_objects_[i].color.rgb.green = 0.6196f;
            marker_objects_[i].color.rgb.blue = 0.2588f;
          }
        previous_rpm_level_ = rpm_position;
        rpm_changed = true;
      }

      if(speed_changed || rpm_changed)
        update_buffer(&buffer_marker_, &buffer_marker_objects_, 30, marker_objects_);
    }

    void Speedometer::rotate(const float factor){
      float reduction{0.0f};

      if(factor > 0.7f){
        if(factor > 1.1f)
          reduction = 48.0f;
        else
          reduction = (factor - 0.7f) * 120.0f;
      }

      if(float_differentiation(reduction, previous_rotation_)){
        previous_rotation_ = reduction;

        const float angle{18.0f - reduction};
        rotation_.from_euler(0.0f, to_radians(5.0f), to_radians(angle));

        core_->reload_screen();
      }
    }

    void Speedometer::draw(){
      if(is_loaded_){
        // Loading textures
        if(t_albedo_) t_albedo_->bind();
        if(t_normal_) t_normal_->bind();
        if(t_pbr_emission_) t_pbr_emission_->bind();

        pbr_->set_value(u_rotation_, rotation_);
        pbr_->set_value(u_translation_, translation_);
        pbr_->set_value(u_light_, light_);
        pbr_->set_value(u_light_color_, light_color_);

        // Loading data buffer
        buffer_background_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, background_data_size_, 1);
        buffer_background_objects_.vertex_release();

        // Loading data buffer
        buffer_marker_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, marker_data_size_, 30);
        buffer_marker_objects_.vertex_release();

        // Loading data buffer
        buffer_needle_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, needle_data_size_, 2);
        buffer_needle_objects_.vertex_release();
      }
    }

    void Speedometer::draw_emission(){
    }

    void Speedometer::draw_text(){
      if(is_loaded_){
        text_->set_value(u_t_rotation_, rotation_);
        text_->set_value(u_t_translation_, translation_);

        buffer_text_.vertex_bind();
        glDrawArrays(GL_POINTS, 0, text_data_size_);
        buffer_text_.vertex_release();
      }
    }

    bool Speedometer::check_folder(){
      if(folder_path_.front() != '/') folder_path_ = "/" + folder_path_;
      if(folder_path_.back() != '/') folder_path_ += "/";

      std::string first_path(folder_path_);

      if(!boost::filesystem::exists(boost::filesystem::path(folder_path_))){
        folder_path_ = boost::filesystem::current_path().string() + folder_path_;

        if(!boost::filesystem::exists(boost::filesystem::path(folder_path_))){
          torero::terminal::Printer::error("*** GUI Speedometer loader: ***\n The folder: "
                                           + first_path + " was not found.\n  Neither: "
                                           + folder_path_ + "\n");
          return false;
        }
      }
      return true;
    }

    void Speedometer::run(){
      if(folder_exist_){
        stbi_set_flip_vertically_on_load(true);

        protector_.lock();

        // loading albedo image
        im_albedo_.data = stbi_load(std::string(folder_path_ + "/albedo.png").c_str(),
                                    &im_albedo_.width, &im_albedo_.height,
                                    &im_albedo_.components_size, 0);
        // loading normal image
        im_normal_.data = stbi_load(std::string(folder_path_ + "/normal.png").c_str(),
                                    &im_normal_.width, &im_normal_.height
                                    , &im_normal_.components_size, 0);
        im_pbr_emission_.data = stbi_load(std::string(folder_path_ + "/pbr_emission.png").c_str(),
                                          &im_pbr_emission_.width, &im_pbr_emission_.height,
                                          &im_pbr_emission_.components_size, 0);

        calculate_model(&data_background_, "/background");
        calculate_model(&data_marker_, "/marker");
        calculate_model(&data_needle_, "/needle");

        is_ready_ = true;
        protector_.unlock();
      }
    }

    void Speedometer::ready(){
      if(!is_loaded_ && is_ready_){
        if(im_albedo_.data)
          t_albedo_ = new torero::gl::Texture(im_albedo_, torero::ShaderTexture::Albedo);

        if(im_normal_.data)
          t_normal_ = new torero::gl::Texture(im_normal_, torero::ShaderTexture::Normal);

        if(im_pbr_emission_.data)
          t_pbr_emission_ = new torero::gl::Texture(im_pbr_emission_,
                                                    torero::ShaderTexture::PBREmission);

        stbi_image_free(im_albedo_.data);
        stbi_image_free(im_normal_.data);
        stbi_image_free(im_pbr_emission_.data);

        load_buffer(&buffer_background_, &buffer_background_objects_, 1,
                    background_objects_, &data_background_, &background_data_size_);
        load_buffer(&buffer_marker_, &buffer_marker_objects_, 30,
                    marker_objects_, &data_marker_, &marker_data_size_);
        load_buffer(&buffer_needle_, &buffer_needle_objects_, 2,
                    needle_objects_, &data_needle_, &needle_data_size_);

        is_loaded_ = true;
      }
    }

    bool Speedometer::is_ready(){
      if(is_loaded_){
        return is_ready_;
      }else{
        if(protector_.try_lock()){
          protector_.unlock();
          return is_ready_;
        }else
          return false;
      }
    }

    void Speedometer::calculate_model(std::vector<torero::ComplexShaderData> *buffer_data,
                                      const std::string subfolder){
      std::vector<algebraica::vec3f> position, normal;
      std::vector<algebraica::vec2f> texture;
      algebraica::vec3f tvector;
      algebraica::vec2f ttexture;
      std::vector<unsigned int> vertex_indices, texture_indices, normal_indices;
      unsigned int vertex_index[3], texture_index[3], normal_index[3];
      std::string line;

      std::ifstream file;
      file.open(folder_path_ + subfolder + "/model.obj");

      if(file.is_open()){
        while(std::getline(file, line)){

          if(line == "" || line[0] == '#')
            continue;

          const char *line_c = line.c_str();

          if(line[0] == 'v' && line[1] == ' '){
            std::sscanf(line_c, "%*s %f %f %f", &tvector[0], &tvector[1], &tvector[2]);
            position.push_back(tvector);
          }else if(line[0] == 'v' && line[1] == 't'){
            std::sscanf(line_c, "%*s %f %f", &ttexture[0], &ttexture[1]);
            texture.push_back(ttexture);
          }else if(line[0] == 'v' && line[1] == 'n'){
            std::sscanf(line_c, "%*s %f %f %f", &tvector[0], &tvector[1], &tvector[2]);
            normal.push_back(tvector);
          }else if(line[0] == 'f'){
            std::sscanf(line_c, "%*s %u/%u/%u %u/%u/%u %u/%u/%u",
                        &vertex_index[0], &texture_index[0], &normal_index[0],
                &vertex_index[1], &texture_index[1], &normal_index[1],
                &vertex_index[2], &texture_index[2], &normal_index[2]);
            for(int i = 0; i < 3; i++){
              vertex_indices.push_back(vertex_index[i]);
              texture_indices.push_back(texture_index[i]);
              normal_indices.push_back(normal_index[i]);
            }
          }
        }
        file.close();

        algebraica::vec3f v0, v1, v2, dP1, dP2, tangent;
        algebraica::vec2f uv0, uv1, uv2, dUV1, dUV2;
        int e{0};

        std::size_t total{vertex_indices.size()};
        buffer_data->resize(total);
        // For each vertex of each triangle
        for(std::size_t i = 0; i < total; i++){
          (*buffer_data)[i].position = position[vertex_indices[i] - 1];
          (*buffer_data)[i].normal = normal[normal_indices[i] - 1];
          if(texture_indices[i] > 0)
            (*buffer_data)[i].texture = texture[texture_indices[i] - 1];
          else
            (*buffer_data)[i].texture = algebraica::vec2f();

          v2 = v1;
          v1 = v0;
          v0 = (*buffer_data)[i].position;

          uv2 = uv1;
          uv1 = uv0;
          uv0 = (*buffer_data)[i].texture;

          if(e++ > 1){
            e = 0;
            // Vertex positions
            dP1 = v1 - v0;
            dP2 = v2 - v0;

            // UV delta
            dUV1 = uv1 - uv0;
            dUV2 = uv2 - uv0;

            float r = 1.0f / (dUV1.x() * dUV2.y() - dUV1.y() * dUV2.x());
            tangent = (dP1 * dUV2.y()   - dP2 * dUV1.y()) * r;

            (*buffer_data)[i].tangent = tangent;
            (*buffer_data)[i - 1].tangent = tangent;
            (*buffer_data)[i - 2].tangent = tangent;
          }
        }
      }else
        torero::terminal::Printer::error("File \"model.obj\" not found at:" + folder_path_
                                         + "...\n");
    }

    void Speedometer::update_text(const float speed, const float rpm){
      char speed_str[6];
      char rpm_str[4];

      const float font_size_speed{0.4f}, font_size_rpm{0.2f};
      const float offset_speed{0.3f}, offset_rpm{-0.21f};

      float speed_offset_x{0.0f}, rpm_offset_x{0.0f};
      const std::size_t speed_length =
          static_cast<std::size_t>(std::sprintf(speed_str, "%.1f", speed));
      const std::size_t rpm_length =
          static_cast<std::size_t>(std::sprintf(rpm_str, "%.0f", rpm));

      const std::size_t total{speed_length + rpm_length};
      std::vector<torero::gui::TextShader> characters(total);
      float next_position{0.0f};
      std::size_t i{0u};

      for(/*i*/; i < speed_length; ++i){
        const std::size_t character{static_cast<std::size_t>(speed_str[i])};
        const torero::text::FontCharacter &chr{(*characters_)[character]};

        // Position
        characters[i].x = next_position + chr.position.offset.x * font_size_speed;
        next_position += chr.position.offset.next * font_size_speed * 0.9f;
        characters[i].y = offset_speed - chr.position.offset.y * font_size_speed;
        characters[i].z = 0.110f;

        // Dimension
        characters[i].width = chr.texture.map.width * font_size_speed;
        characters[i].height = chr.texture.map.height * font_size_speed;

        // Texture
        characters[i].u1 = chr.texture.map.u1;
        characters[i].v1 = chr.texture.map.v1;
        characters[i].u2 = chr.texture.map.u2;
        characters[i].v2 = chr.texture.map.v2;
      }
      speed_offset_x = next_position / 2.1f;

      for(std::size_t u = 0u; u < speed_length; ++u)
        characters[u].x -= speed_offset_x;

      next_position = 0.0f;

      for(std::size_t e = 0u; e < rpm_length; ++e, ++i){
        const std::size_t character{static_cast<std::size_t>(rpm_str[e])};
        const torero::text::FontCharacter &chr{(*characters_)[character]};

        // Position
        characters[i].x = next_position + chr.position.offset.x * font_size_rpm;
        next_position += chr.position.offset.next * font_size_rpm;
        characters[i].y = offset_rpm + chr.position.offset.y * font_size_rpm;
        characters[i].z = 0.110f;

        // Dimension
        characters[i].width = chr.texture.map.width * font_size_rpm;
        characters[i].height = chr.texture.map.height * font_size_rpm;

        // Texture
        characters[i].u1 = chr.texture.map.u1;
        characters[i].v1 = chr.texture.map.v1;
        characters[i].u2 = chr.texture.map.u2;
        characters[i].v2 = chr.texture.map.v2;
      }
      rpm_offset_x = next_position / 2.0f;

      for(std::size_t u = speed_length; u < total; ++u)
        characters[u].x -= rpm_offset_x;

      text_data_size_ = total;

      buffer_text_.vertex_bind();
      buffer_text_.allocate_array(characters.data(), text_data_size_ * text_stride_size_,
                                  GL_DYNAMIC_DRAW);

      GLint offset{0};
      buffer_text_.enable(torero::ShaderLocation::Position);
      buffer_text_.attributte_buffer(torero::ShaderLocation::Position, _3D, offset, text_stride_size_);

      offset += sizeof(algebraica::vec3f);
      buffer_text_.enable(i_size_);
      buffer_text_.attributte_buffer(i_size_, _2D, offset, text_stride_size_);

      offset += sizeof(algebraica::vec2f);
      buffer_text_.enable(torero::ShaderLocation::UV);
      buffer_text_.attributte_buffer(torero::ShaderLocation::UV, _4D, offset, text_stride_size_);

      offset += sizeof(algebraica::vec4f);
      buffer_text_.enable(i_color_);
      buffer_text_.attributte_buffer(i_color_, _4D, offset, text_stride_size_);

      buffer_text_.vertex_release();
    }

    void Speedometer::load_buffer(torero::gl::Buffer *buffer, torero::gl::Buffer *objects,
                                  const GLsizei size,
                                  const std::vector<torero::gui::PBR_Shader> &object_data,
                                  std::vector<torero::ComplexShaderData> *buffer_data,
                                  GLsizei *data_size){
      // Data size of main model
      *data_size = static_cast<GLsizei>(buffer_data->size());
      // Sending main model's data to OpenGL buffer
      buffer->vertex_bind();
      buffer->allocate_array(buffer_data->data(), *data_size * data_stride_size_);
      buffer->vertex_release();
      buffer_data->clear();

      update_buffer(buffer, objects, size, object_data);
    }

    void Speedometer::update_buffer(torero::gl::Buffer *buffer, torero::gl::Buffer *objects,
                                    const GLsizei size,
                                    const std::vector<torero::gui::PBR_Shader> &object_data){
      // Sending repetitions data to OpenGL buffer
      objects->vertex_bind();
      objects->allocate_array(object_data.data(), size * object_stride_size_, GL_DYNAMIC_DRAW);

      objects->enable(i_angle_);
      objects->attributte_buffer(i_angle_, _1D, 0, object_stride_size_);
      objects->divisor(i_angle_, 1);

      objects->enable(i_color_);
      objects->attributte_buffer(i_color_, _3D, sizeof(float), object_stride_size_);
      objects->divisor(i_color_, 1);

      // Adding the main model data
      buffer->buffer_bind();
      GLint offset{0};
      buffer->enable(torero::ShaderLocation::Position);
      buffer->attributte_buffer(torero::ShaderLocation::Position, _3D, offset, data_stride_size_);

      offset += sizeof(algebraica::vec3f);
      buffer->enable(torero::ShaderLocation::UV);
      buffer->attributte_buffer(torero::ShaderLocation::UV, _2D, offset, data_stride_size_);

      offset += sizeof(algebraica::vec2f);
      buffer->enable(torero::ShaderLocation::Normal);
      buffer->attributte_buffer(torero::ShaderLocation::Normal, _3D, offset, data_stride_size_);

      offset += sizeof(algebraica::vec3f);
      buffer->enable(torero::ShaderLocation::Tangent);
      buffer->attributte_buffer(torero::ShaderLocation::Tangent, _3D, offset, data_stride_size_);
      buffer->buffer_release();

      objects->vertex_release();
    }
  }
}
