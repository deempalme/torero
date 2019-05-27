#include "torero/core.h"
#include "torero/file/manager.h"
#include "torero/model/loader.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"

namespace torero{
  namespace model {
    Loader::Loader(const std::string folder_address, torero::gl::Shader *shader_program,
                   torero::Core *core) :
      folder_address_(folder_address),
      folder_exists_(torero::file::Manager::check_path(&folder_address_)),
      is_ready_(false),
      is_loaded_(false),
      core_(core),
      shader_(shader_program),
      buffer_(new torero::gl::Buffer()),
      data_size_(0),
      buffer_data_(0),
      t_albedo_(nullptr),
      t_normal_(nullptr),
      t_pbr_emission_(nullptr),
      error_(false),
      error_log_("Model not loaded yet...\n----------\n")
    {
      core_->multithread_add_process(
            boost::bind(&model::Loader::run, this),
            boost::bind(&model::Loader::ready, this),
            &protector_);
    }

    Loader::Loader(const torero::model::Models model, torero::gl::Shader *shader_program,
                   torero::Core *core) :
      is_ready_(false),
      is_loaded_(false),
      core_(core),
      shader_(shader_program),
      buffer_(new torero::gl::Buffer()),
      data_size_(0),
      buffer_data_(0),
      t_albedo_(nullptr),
      t_normal_(nullptr),
      t_pbr_emission_(nullptr),
      error_(false),
      error_log_("Model not loaded yet...\n----------\n")
    {
      switch(model){
        case torero::model::DB5_Body:
          folder_address_ = torero::k_db5_body_model;
        break;
        case torero::model::DB5_Windows:
          folder_address_ = torero::k_db5_windows_model;
        break;
        case torero::model::DB5_Accessories:
          folder_address_ = torero::k_db5_accessories_model;
        break;
        case torero::model::DB5_Interior:
          folder_address_ = torero::k_db5_interior_model;
        break;
        case torero::model::Chassis:
          folder_address_ = torero::k_chassis_model;
        break;
        case torero::model::Axis:
          folder_address_ = torero::k_axis_model;
        break;
        case torero::model::Steering:
          folder_address_ = torero::k_steering_model;
        break;
        case torero::model::Tire:
          folder_address_ = torero::k_tire_model;
        break;
        case torero::model::ShuttleBody:
          folder_address_ = torero::k_shuttle_body_model;
        break;
        case torero::model::ShuttleWindows:
          folder_address_ = torero::k_shuttle_windows_model;
        break;
        default:
          folder_address_ = torero::k_coordinates_model;
        break;
      }

      folder_exists_ = torero::file::Manager::check_path(&folder_address_);

      core_->multithread_add_process(
            boost::bind(&model::Loader::run, this),
            boost::bind(&model::Loader::ready, this),
            &protector_);
    }

    Loader::~Loader(){
      delete buffer_;
      if(t_albedo_) delete t_albedo_;
      if(t_pbr_emission_) delete t_pbr_emission_;
      if(t_normal_) delete t_normal_;
    }

    void Loader::pre_drawing(){
      if(is_loaded_){
        // Loading textures
        if(t_albedo_) t_albedo_->bind();
        if(t_normal_) t_normal_->bind();
        if(t_pbr_emission_) t_pbr_emission_->bind();
        // Loading data buffer
        buffer_->vertex_bind();
      }
    }

    void Loader::draw(){
      if(is_loaded_)
        glDrawArrays(GL_TRIANGLES, 0, data_size_);
    }

    void Loader::post_drawing(){
      if(is_loaded_){
        buffer_->vertex_release();
      }
    }

    bool Loader::is_ready(){
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

    void Loader::run(){
      stbi_set_flip_vertically_on_load(true);

      protector_.lock();
      std::vector<algebraica::vec3f> position, normal;
      std::vector<algebraica::vec2f> texture;
      algebraica::vec3f tvector;
      algebraica::vec2f ttexture;
      std::vector<unsigned int> vertex_indices, texture_indices, normal_indices;
      unsigned int vertex_index[3], texture_index[3], normal_index[3];
      std::string line;

      std::ifstream file;
      file.open(folder_address_ + "/model.obj");

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

        algebraica::vec3f v0, v1, v2, dP1, dP2, tangent/*, bitangent*/;
        algebraica::vec2f uv0, uv1, uv2, dUV1, dUV2;
        int e{0};

        std::size_t total{vertex_indices.size()};
        buffer_data_.resize(total);
        // For each vertex of each triangle
        for(std::size_t i = 0; i < total; i++){
          buffer_data_[i].position = position[vertex_indices[i] - 1];
          buffer_data_[i].normal = normal[normal_indices[i] - 1];
          if(texture_indices[i] > 0)
            buffer_data_[i].texture = texture[texture_indices[i] - 1];
          else
            buffer_data_[i].texture = algebraica::vec2f();

          v2 = v1;
          v1 = v0;
          v0 = buffer_data_[i].position;

          uv2 = uv1;
          uv1 = uv0;
          uv0 = buffer_data_[i].texture;

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
            //          bitangent = (dP2 * dUV1.x()   - dP1 * dUV2.x()) * r;

            buffer_data_[i].tangent = tangent;
            buffer_data_[i - 1].tangent = tangent;
            buffer_data_[i - 2].tangent = tangent;
            //          buffer_data_[i].bitangent = bitangent;
            //          buffer_data_[i - 1].bitangent = bitangent;
            //          buffer_data_[i - 2].bitangent = bitangent;
          }
        }

        data_size_ = static_cast<GLsizei>(total * sizeof(torero::ComplexShaderData));

        // loading albedo image
        albedo_.data = stbi_load(std::string(folder_address_ + "/albedo.png").c_str(),
                                 &albedo_.width, &albedo_.height, &albedo_.components_size, 0);
        // loading normal map image
        normal_.data = stbi_load(std::string(folder_address_ + "/normal.png").c_str(),
                                 &normal_.width, &normal_.height, &normal_.components_size, 0);
        // loading emission image
        pbr_emission_.data = stbi_load(std::string(folder_address_ + "/pbr_emission.png").c_str(),
                                       &pbr_emission_.width, &pbr_emission_.height,
                                       &pbr_emission_.components_size, 0);
        is_ready_ = true;
        error_ = false;
        error_log_.clear();
      }else{
        error_ = true;
        error_log_ = "File \"model.obj\" not found at:" + folder_address_ + "...\n----------\n";
        is_ready_ = false;
      }
      protector_.unlock();
    }

    void Loader::ready(){
      if(!error_ && !is_loaded_ && is_ready_){
        shader_->use();

        GLsizei stride_size{sizeof(torero::ComplexShaderData)};

        buffer_->create();
        buffer_->vertex_bind();
        buffer_->allocate_array(buffer_data_.data(), data_size_, GL_STATIC_DRAW);
        buffer_data_.clear();

        GLint offset{0};
        buffer_->enable(torero::ShaderLocation::Position);
        buffer_->attributte_buffer(torero::ShaderLocation::Position, _3D, offset, stride_size);

        offset += sizeof(algebraica::vec3f);
        buffer_->enable(torero::ShaderLocation::Normal);
        buffer_->attributte_buffer(torero::ShaderLocation::Normal, _3D, offset, stride_size);

        offset += sizeof(algebraica::vec3f);
        buffer_->enable(torero::ShaderLocation::UV);
        buffer_->attributte_buffer(torero::ShaderLocation::UV, _2D, offset, stride_size);

        offset += sizeof(algebraica::vec2f);
        buffer_->enable(torero::ShaderLocation::Tangent);
        buffer_->attributte_buffer(torero::ShaderLocation::Tangent, _3D, offset, stride_size);
        /*
      offset += sizeof(algebraica::vec3f);
      buffer_->enable(torero::ShaderLocation::Bitangent);
      buffer_->attributte_buffer(torero::ShaderLocation::Bitangent, _3D, offset, stride_size);
*/
        buffer_->vertex_release();

        if(albedo_.data)
          t_albedo_ = new torero::gl::Texture(albedo_, torero::ShaderTexture::Albedo);

        if(normal_.data)
          t_normal_ = new torero::gl::Texture(normal_, torero::ShaderTexture::Normal);

        if(pbr_emission_.data)
          t_pbr_emission_ = new torero::gl::Texture(pbr_emission_,
                                                    torero::ShaderTexture::PBREmission);

        stbi_image_free(albedo_.data);
        stbi_image_free(normal_.data);
        stbi_image_free(pbr_emission_.data);

        is_loaded_ = true;
      }else if(error_)
        torero::terminal::Printer::error(error_log_);
    }
  }
}
