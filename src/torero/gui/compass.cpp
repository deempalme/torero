#include "torero/gui/compass.h"
#include "torero/core.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"

namespace torero {
  namespace gui {
    Compass::Compass(torero::Core *core, torero::gl::Shader *pbr, torero::gl::Shader *text,
                     const std::vector<torero::text::FontCharacter> *characters,
                     const std::string &folder_path) :
      core_(core),
      folder_path_(folder_path),
      file_exists_(false),
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

      buffer_compass_(true),
      buffer_compass_objects_(true),
      compass_data_size_(0),
      previous_compass_angle_(0.0f),
      compass_objects_(1, { 0.0f, {{1.0f, 1.0f, 1.0f}} }),

      buffer_steering_(true),
      buffer_steering_objects_(true),
      steering_data_size_(0),
      previous_steering_angle_(0.0f),
      steering_objects_(1, { 0.0f, {{1.0f, 1.0f, 1.0f}} }),

      buffer_clutch_(true),
      buffer_clutch_objects_(true),
      clutch_data_size_(0),
      previous_cloutch_(0.0f),
      clutch_color_(0.012f, 0.918f, 1.0f),
      clutch_objects_(1, { 0.0f, {{0.0012f, 0.0918f, 0.1f}} }),

      buffer_marker_(true),
      buffer_marker_objects_(true),
      marker_data_size_(0),
      previous_brake_value_(0.0f),
      previous_accelerator_value_(0.0f),
      previous_brake_(0),
      previous_accelerator_(0),
      marker_objects_(26, { 0.0f, {{0.0f, 0.25f, 0.1745f}} }),

      buffer_needle_(true),
      buffer_needle_objects_(true),
      needle_data_size_(0),
      needle_objects_(1, { 0.0f, {{0.799f, 0.076f, 0.014f}} }),

      text_(text),
      i_size_(text_->attribute_location("i_size")),
      u_t_rotation_(text_->uniform_location("u_rotation")),
      u_t_translation_(text_->uniform_location("u_translation")),
      buffer_text_(true),
      text_data_size_(0u),
      text_stride_size_(sizeof(torero::gui::TextShader)),
      characters_(characters),
      previous_gear_("P"),

      data_background_(0),
      data_compass_(0),
      data_steering_(0),
      data_clutch_(0),
      data_marker_(0),
      data_needle_(0),

      previous_rotation_(0.0f),
      rotation_(algebraica::quaternionF::euler_to_quaternion(0.0f, to_radians(5.0f), to_radians(-18.0f))),
      translation_(-3.5f),
      light_color_(0.799f, 0.076f, 0.014f),
      light_(0.0f, 1.0f, 0.0f)
    {
      if(check_folder()){
        file_exists_ = true;

        for(std::size_t i = 0; i < 13; ++i){
          marker_objects_[i].angle = - k_10_degrees * i;
          marker_objects_[i].color.rgb.red = 0.24125f;
          marker_objects_[i].color.rgb.green = 0.08925f;
          marker_objects_[i].color.rgb.blue = 0.11575f;
        }

        for(std::size_t i = 13; i < 26; ++i)
          marker_objects_[i].angle = k_90_degrees + k_10_degrees * (i - 13);
      }

      core_->multithread_add_process(boost::bind(&gui::Compass::run, this),
                                     boost::bind(&gui::Compass::ready, this),
                                     &protector_);
    }

    Compass::~Compass(){
      if(t_albedo_) delete t_albedo_;
      if(t_normal_) delete t_normal_;
      if(t_pbr_emission_) delete t_pbr_emission_;
    }

    void Compass::update_rotation(const float compass_angle, const float steering_angle){
      if(is_loaded_){
        if(float_differentiation(compass_angle, previous_compass_angle_)){
          previous_compass_angle_ = compass_angle;

          compass_objects_[0].angle = -compass_angle;
          update_buffer(&buffer_compass_, &buffer_compass_objects_, 1, compass_objects_);
        }

        if(float_differentiation(steering_angle, previous_steering_angle_)){
          previous_steering_angle_ = steering_angle;

          steering_objects_[0].angle = steering_angle;
          update_buffer(&buffer_steering_, &buffer_steering_objects_, 1, steering_objects_);
          update_text(steering_angle, previous_gear_);
        }
      }
    }

    void Compass::update_pedals(const float brake, const float accelerator,
                                const float clutch, const std::string gear_position){
      if(is_loaded_){
        const std::size_t brake_level{static_cast<std::size_t>(std::round(brake * 13.0f))};
        const std::size_t gas_level{static_cast<std::size_t>(std::round(accelerator * 13.0f))};

        bool update_brakes{false}, update_gas{false};

        if(brake_level != previous_brake_){
          previous_brake_ = brake_level;

          for(std::size_t i = 0; i < 13; ++i){
            if(i < brake_level){
              marker_objects_[i].color.rgb.red = 0.965f;
              marker_objects_[i].color.rgb.green = 0.357f;
              marker_objects_[i].color.rgb.blue = 0.463f;
            }else{
              marker_objects_[i].color.rgb.red = 0.24125f;
              marker_objects_[i].color.rgb.green = 0.08925f;
              marker_objects_[i].color.rgb.blue = 0.11575f;
            }
          }
          update_brakes = true;
        }

        if(gas_level != previous_accelerator_){
          previous_accelerator_ = gas_level;

          for(std::size_t e = 0, i = 13; e < 13; ++e, ++i){
            if(e < gas_level){
              marker_objects_[i].color.rgb.red = 0.0f;
              marker_objects_[i].color.rgb.green = 1.0f;
              marker_objects_[i].color.rgb.blue = 0.698f;
            }else{
              marker_objects_[i].color.rgb.red = 0.0f;
              marker_objects_[i].color.rgb.green = 0.25f;
              marker_objects_[i].color.rgb.blue = 0.1745f;
            }
          }
          update_gas = true;
        }

        if(update_brakes || update_gas)
          update_buffer(&buffer_marker_, &buffer_marker_objects_, 26, marker_objects_);

        if(float_differentiation(clutch, previous_cloutch_)){
          previous_cloutch_ = clutch;
          const float new_clutch{0.1f + clutch * 0.9f};

          clutch_objects_[0].color.rgb.red   = clutch_color_[0] * new_clutch;
          clutch_objects_[0].color.rgb.green = clutch_color_[1] * new_clutch;
          clutch_objects_[0].color.rgb.blue  = clutch_color_[2] * new_clutch;

          update_buffer(&buffer_clutch_, &buffer_clutch_objects_, 1, clutch_objects_);
        }

        if(gear_position != previous_gear_){
          previous_gear_ = gear_position;
          update_text(previous_steering_angle_, gear_position);
        }
      }
    }

    void Compass::rotate(const float factor){
      float reduction{0.0f};

      if(factor < 0.3f){
        if(factor < -0.1f)
          reduction = 48.0f;
        else
          reduction = (0.3f - factor) * 120.0f;
      }

      if(float_differentiation(reduction, previous_rotation_)){
        previous_rotation_ = reduction;

        const float angle{-18.0f + reduction};
        rotation_.from_euler(0.0f, to_radians(5.0f), to_radians(angle));

        core_->reload_screen();
      }
    }

    void Compass::draw(){
      if(is_loaded_){
        // Loading textures
        if(t_albedo_) t_albedo_->bind();
        if(t_normal_) t_normal_->bind();
        if(t_pbr_emission_) t_pbr_emission_->bind();

        pbr_->set_value(u_rotation_, rotation_);
        pbr_->set_value(u_translation_, translation_);
        pbr_->set_value(u_light_, light_);
        pbr_->set_value(u_light_color_, light_color_);

        buffer_background_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, background_data_size_, 1);
        buffer_background_objects_.vertex_release();

        buffer_compass_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, compass_data_size_, 1);
        buffer_compass_objects_.vertex_release();

        buffer_steering_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, steering_data_size_, 1);
        buffer_steering_objects_.vertex_release();

        buffer_clutch_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, clutch_data_size_, 1);
        buffer_clutch_objects_.vertex_release();

        buffer_marker_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, marker_data_size_, 26);
        buffer_marker_objects_.vertex_release();

        buffer_needle_objects_.vertex_bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, needle_data_size_, 1);
        buffer_needle_objects_.vertex_release();
      }
    }

    void Compass::draw_emission(){
    }

    void Compass::draw_text(){
      if(is_loaded_){
        text_->set_value(u_t_rotation_, rotation_);
        text_->set_value(u_t_translation_, translation_);

        buffer_text_.vertex_bind();
        glDrawArrays(GL_POINTS, 0, text_data_size_);
        buffer_text_.vertex_release();
      }
    }

    bool Compass::check_folder(){
      if(folder_path_.front() != '/') folder_path_ = "/" + folder_path_;
      if(folder_path_.back() != '/') folder_path_ += "/";

      std::string first_path(folder_path_);

      if(!boost::filesystem::exists(boost::filesystem::path(folder_path_))){
        folder_path_ = boost::filesystem::current_path().string() + folder_path_;

        if(!boost::filesystem::exists(boost::filesystem::path(folder_path_))){
          torero::terminal::Printer::message("*** GUI Compass loader: ***\n The folder: "
                                             + first_path + " was not found.\n  Neither: "
                                             + folder_path_ + "\n", torero::Message::Error);
          return false;
        }
      }
      return true;
    }

    void Compass::run(){
      if(file_exists_){
        protector_.lock();
        stbi_set_flip_vertically_on_load(true);

        // loading albedo image
        im_albedo_.data = stbi_load(std::string(folder_path_ + "/albedo.png").c_str(),
                                    &im_albedo_.width, &im_albedo_.height,
                                    &im_albedo_.components_size, 0);
        // loading normal image
        im_normal_.data = stbi_load(std::string(folder_path_ + "/normal.png").c_str(),
                                    &im_normal_.width, &im_normal_.height
                                    , &im_normal_.components_size, 0);
        // loading metallic, roughness, cavities and emission image
        im_pbr_emission_.data = stbi_load(std::string(folder_path_ + "/pbr_emission.png").c_str(),
                                          &im_pbr_emission_.width, &im_pbr_emission_.height,
                                          &im_pbr_emission_.components_size, 0);

        calculate_model(&data_background_, "/background");
        calculate_model(&data_compass_, "/compass");
        calculate_model(&data_steering_, "/steering");
        calculate_model(&data_clutch_, "/clutch");
        calculate_model(&data_marker_, "/marker");
        calculate_model(&data_needle_, "/needle");

        is_ready_ = true;
        protector_.unlock();
      }
    }

    void Compass::ready(){
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
        load_buffer(&buffer_compass_, &buffer_compass_objects_, 1,
                    compass_objects_, &data_compass_, &compass_data_size_);
        load_buffer(&buffer_steering_, &buffer_steering_objects_, 1,
                    steering_objects_, &data_steering_, &steering_data_size_);
        load_buffer(&buffer_clutch_, &buffer_clutch_objects_, 1,
                    clutch_objects_, &data_clutch_, &clutch_data_size_);
        load_buffer(&buffer_marker_, &buffer_marker_objects_, 26,
                    marker_objects_, &data_marker_, &marker_data_size_);
        load_buffer(&buffer_needle_, &buffer_needle_objects_, 1,
                    needle_objects_, &data_needle_, &needle_data_size_);

        is_loaded_ = true;
      }
    }

    bool Compass::is_ready(){
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

    void Compass::calculate_model(std::vector<torero::ComplexShaderData> *buffer_data,
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
        torero::terminal::Printer::message("File \"model.obj\" not found at:" + folder_path_
                                           + "...\n", torero::Message::Error);
    }

    void Compass::update_text(const float steering, const std::string gear){
      char steering_str[5];
      const float steering_deg{std::round(to_degrees(steering))};

      const float font_size_steering{0.2f}, font_size_gear{0.2f};
      const float offset_steering{0.15f}, offset_gear{-0.07f};

      float steering_offset_x{0.0f};
      const std::size_t steering_length =
          static_cast<std::size_t>(std::sprintf(steering_str, "%+.0f", steering_deg));

      std::vector<torero::gui::TextShader> characters(steering_length + 1);
      float next_position{0.0f};
      std::size_t i{0u};

      for(; i < steering_length; ++i){
        const std::size_t character{static_cast<std::size_t>(steering_str[i])};
        const torero::text::FontCharacter chr{(*characters_)[character]};

        // Position
        characters[i].x = next_position + chr.position.offset.x * font_size_steering;
        next_position += chr.position.offset.next * font_size_steering * 0.9f;
        characters[i].y = offset_steering - chr.position.offset.y * font_size_steering;
        characters[i].z = 0.110f;

        // Dimension
        characters[i].width  = chr.texture.map.width * font_size_steering;
        characters[i].height = chr.texture.map.height * font_size_steering;

        // Texture
        characters[i].u1 = chr.texture.map.u1;
        characters[i].v1 = chr.texture.map.v1;
        characters[i].u2 = chr.texture.map.u2;
        characters[i].v2 = chr.texture.map.v2;
      }
      steering_offset_x = next_position / 2.1f;

      for(std::size_t u = 0u; u < steering_length; ++u)
        characters[u].x -= steering_offset_x;

      // Gear text description
      const std::size_t character{static_cast<std::size_t>(gear[0])};
      const torero::text::FontCharacter &chr{(*characters_)[character]};

      // Position
      characters[i].x = chr.position.offset.x * font_size_gear
                        - (chr.position.offset.next * font_size_gear) * 0.35f;
      characters[i].y = offset_gear + chr.position.offset.y * font_size_gear;
      characters[i].z = 0.110f;

      // Dimension
      characters[i].width  = chr.texture.map.width * font_size_gear;
      characters[i].height = chr.texture.map.height * font_size_gear;

      // Texture
      characters[i].u1 = chr.texture.map.u1;
      characters[i].v1 = chr.texture.map.v1;
      characters[i].u2 = chr.texture.map.u2;
      characters[i].v2 = chr.texture.map.v2;
      // End gear text description

      text_data_size_ = steering_length + 1;

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

    void Compass::load_buffer(torero::gl::Buffer *buffer, torero::gl::Buffer *objects,
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

    void Compass::update_buffer(torero::gl::Buffer *buffer, torero::gl::Buffer *objects,
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
