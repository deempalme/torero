#include "torero/three_dimensional_model_loader.h"
#include "torero/core.h"
// Image loader
#include "stb_image.h"

namespace torero{
  ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const std::string folder_address,
                                                           Shader *shader_program,
                                                           Core *core) :
    folder_address_(folder_address),
    folder_exists_(check_folder()),
    is_ready_(false),
    is_loaded_(false),
    core_(core),
    shader_(shader_program),
    buffer_(new Buffer()),
    data_size_(0),
    buffer_data_(0),
    t_albedo_(nullptr),
    t_normal_(nullptr),
    t_metallic_(nullptr),
    t_roughness_(nullptr),
    t_ao_(nullptr),
    t_emission_(nullptr),
    error_(false),
    error_log_("Model not loaded yet...\n----------\n")
  {
  }

  ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const torero::Models model,
                                                           Shader *shader_program,
                                                           Core *core) :
    is_ready_(false),
    is_loaded_(false),
    core_(core),
    shader_(shader_program),
    buffer_(new Buffer()),
    data_size_(0),
    buffer_data_(0),
    t_albedo_(nullptr),
    t_normal_(nullptr),
    t_metallic_(nullptr),
    t_roughness_(nullptr),
    t_ao_(nullptr),
    t_emission_(nullptr),
    error_(false),
    error_log_("Model not loaded yet...\n----------\n")
  {
    switch(model){
    case torero::DB5_Body:
      folder_address_ = "/resources/models3D/db5_body";
      break;
    case torero::DB5_Windows:
      folder_address_ = "/resources/models3D/db5_windows";
      break;
    case torero::DB5_Accessories:
      folder_address_ = "/resources/models3D/db5_accessories";
      break;
    case torero::DB5_Interior:
      folder_address_ = "/resources/models3D/db5_interior";
      break;
    case torero::Chassis:
      folder_address_ = "/resources/models3D/chassis";
      break;
    case torero::Axis:
      folder_address_ = "/resources/models3D/axis";
      break;
    case torero::Steering:
      folder_address_ = "/resources/models3D/steering";
      break;
    case torero::Tire:
      folder_address_ = "/resources/models3D/tire";
      break;
    case torero::ShuttleBody:
      folder_address_ = "/resources/models3D/shuttle_body";
      break;
    case torero::ShuttleWindows:
      folder_address_ = "/resources/models3D/shuttle_windows";
      break;
    default:
      folder_address_ = "/resources/models3D/coordinate_system";
      break;
    }

    folder_exists_ = check_folder();
  }

  ThreeDimensionalModelLoader::~ThreeDimensionalModelLoader(){
    delete buffer_;
    if(t_albedo_) delete t_albedo_;
    if(t_ao_) delete t_ao_;
    if(t_emission_) delete t_emission_;
    if(t_normal_) delete t_normal_;
    if(t_metallic_) delete t_metallic_;
    if(t_roughness_) delete t_roughness_;
  }

  void ThreeDimensionalModelLoader::pre_drawing(){
    if(is_loaded_){
      // Loading textures
      if(t_albedo_) t_albedo_->use();
      if(t_normal_) t_normal_->use();
      if(t_metallic_) t_metallic_->use();
      if(t_roughness_) t_roughness_->use();
      if(t_ao_) t_ao_->use();
      if(t_emission_) t_emission_->use();
      // Loading data buffer
      buffer_->vertex_bind();
    }
  }

  void ThreeDimensionalModelLoader::draw(){
    if(is_loaded_)
      glDrawArrays(GL_TRIANGLES, 0, data_size_);
  }

  void ThreeDimensionalModelLoader::post_drawing(){
    if(is_loaded_){
      buffer_->vertex_release();
    }
  }

  bool ThreeDimensionalModelLoader::is_ready(){
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

  bool ThreeDimensionalModelLoader::check_folder(){
    if(folder_address_.front() != '/') folder_address_ = "/" + folder_address_;
    if(folder_address_.back() != '/') folder_address_ += "/";

    std::string first_path(folder_address_);

    if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
      folder_address_ = boost::filesystem::current_path().string() + folder_address_;

      if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
        error_ = true;
        error_log_ = "*** Model loader: ***\n The folder: " + first_path +
                     " was not found.\n  Neither: " + folder_address_ + "\n";
        core_->message_handler(error_log_, torero::Message::Error);
        return false;
      }
    }
    return true;
  }

  void ThreeDimensionalModelLoader::run(){
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
//          std::cout << std::setprecision(4) << tvector << std::endl;
          for(int i = 0; i < 3; i++){
            vertex_indices.push_back(vertex_index[i]);
            texture_indices.push_back(texture_index[i]);
            normal_indices.push_back(normal_index[i]);
          }
        }
      }
      file.close();

      stbi_set_flip_vertically_on_load(true);

      algebraica::vec3f v0, v1, v2, dP1, dP2, tangent, bitangent;
      algebraica::vec2f uv0, uv1, uv2, dUV1, dUV2;
      int e{0};

      int total{static_cast<int>(vertex_indices.size())};
      buffer_data_.resize(total);
      // For each vertex of each triangle
      for(int i = 0; i < total; i++){
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
          bitangent = (dP2 * dUV1.x()   - dP1 * dUV2.x()) * r;

          buffer_data_[i].tangent = tangent;
          buffer_data_[i - 1].tangent = tangent;
          buffer_data_[i - 2].tangent = tangent;
          buffer_data_[i].bitangent = bitangent;
          buffer_data_[i - 1].bitangent = bitangent;
          buffer_data_[i - 2].bitangent = bitangent;
        }

//        std::cout << std::setprecision(6) << std::fixed << std::showpos
//                  << "{ "
//                  << buffer_data_[i].position.x() << ", "
//                  << buffer_data_[i].position.y() << ", "
//                  << buffer_data_[i].position.z() << ", "
//                  << buffer_data_[i].normal.x() << ", "
//                  << buffer_data_[i].normal.y() << ", "
//                  << buffer_data_[i].normal.z() << ", " << std::noshowpos
//                  << buffer_data_[i].texture.x() << ", "
//                  << buffer_data_[i].texture.y() << ", ";
//        int x{0}, y{0}, z{0};

//        // for 3D models
//        if(vertex_indices[i] > 64){
//          x = 1;
//          z = 1;
//        }
//        if(buffer_data_[i].position.y() > -0.45f && buffer_data_[i].position.y() < 0.45f)
//          y = 1;

//        // for squares
//        if(buffer_data_[i].position.x() > -0.45f && buffer_data_[i].position.x() < 0.45f)
//          x = 1;
//        if(buffer_data_[i].position.z() > -0.45f && buffer_data_[i].position.z() < 0.45f)
//          z = 1;

//        // for circles
//        if(vertex_indices[i] > 28){
//          x = 1;
//          z = 1;
//        }

//        std::cout<< x << ", " << y << ", " << z << " }," << std::endl;
      }

//      std::cout << total << std::endl;
      data_size_ = static_cast<GLsizei>(total * sizeof(torero::ComplexShaderData));

      // loading albedo image
      albedo_.data = stbi_load(std::string(folder_address_ + "/albedo.png").c_str(),
                               &albedo_.width, &albedo_.height, &albedo_.components_size, 0);
      // loading ambient occlusion image
      ao_.data = stbi_load(std::string(folder_address_ + "/ao.png").c_str(),
                           &ao_.width, &ao_.height, &ao_.components_size, 0);
      // loading emission image
      emission_.data = stbi_load(std::string(folder_address_ + "/emission.png").c_str(),
                           &emission_.width, &emission_.height, &emission_.components_size, 0);
      // loading normal map image
      normal_.data = stbi_load(std::string(folder_address_ + "/normal.png").c_str(),
                               &normal_.width, &normal_.height, &normal_.components_size, 0);
      // loading metallic image
      metallic_.data = stbi_load(std::string(folder_address_ + "/metallic.png").c_str(),
                                 &metallic_.width, &metallic_.height, &metallic_.components_size, 0);
      // loading roughness image
      roughness_.data = stbi_load(std::string(folder_address_ + "/roughness.png").c_str(),
                                  &roughness_.width, &roughness_.height, &roughness_.components_size, 0);

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

  void ThreeDimensionalModelLoader::ready(){
    if(!error_ && !is_loaded_ && is_ready_){
      shader_->use();

      i_position_  = shader_->attribute_location("i_position");
      i_normal_    = shader_->attribute_location("i_normal");
      i_tangent_   = shader_->attribute_location("i_tangent");
      i_bitangent_ = shader_->attribute_location("i_bitangent");
      i_uv_        = shader_->attribute_location("i_uv");

      GLsizei stride_size{sizeof(torero::ComplexShaderData)};

      buffer_->create();
      buffer_->vertex_bind();
      buffer_->allocate_array(buffer_data_.data(), data_size_, GL_STATIC_DRAW);
      buffer_data_.clear();

      GLint offset{0};
      buffer_->enable(i_position_);
      buffer_->attributte_buffer(i_position_, _3D, offset, stride_size);

      offset += sizeof(algebraica::vec3f);
      buffer_->enable(i_normal_);
      buffer_->attributte_buffer(i_normal_, _3D, offset, stride_size);

      offset += sizeof(algebraica::vec3f);
      buffer_->enable(i_tangent_);
      buffer_->attributte_buffer(i_tangent_, _3D, offset, stride_size);

      offset += sizeof(algebraica::vec3f);
      buffer_->enable(i_bitangent_);
      buffer_->attributte_buffer(i_bitangent_, _3D, offset, stride_size);

      offset += sizeof(algebraica::vec3f);
      buffer_->enable(i_uv_);
      buffer_->attributte_buffer(i_uv_, _2D, offset, stride_size);

      buffer_->vertex_release();

      if(albedo_.data)
        t_albedo_ = new Texture(3, core_->screen_max_anisotropic_filtering(), &albedo_);

      if(normal_.data)
        t_normal_ = new Texture(4, core_->screen_max_anisotropic_filtering(), &normal_);

      if(metallic_.data)
        t_metallic_ = new Texture(5, core_->screen_max_anisotropic_filtering(), &metallic_);

      if(roughness_.data)
        t_roughness_ = new Texture(6, core_->screen_max_anisotropic_filtering(), &roughness_);

      if(ao_.data)
        t_ao_ = new Texture(7, core_->screen_max_anisotropic_filtering(), &ao_);

      if(emission_.data)
        t_emission_ = new Texture(8, core_->screen_max_anisotropic_filtering(), &emission_);

      stbi_image_free(albedo_.data);
      stbi_image_free(normal_.data);
      stbi_image_free(metallic_.data);
      stbi_image_free(roughness_.data);
      stbi_image_free(ao_.data);
      stbi_image_free(emission_.data);

      is_loaded_ = true;
    }else if(error_)
      core_->message_handler(error_log_, torero::Message::Error);
  }
}
