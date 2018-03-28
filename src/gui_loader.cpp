#include "torero/gui_loader.h"
#include "torero/core.h"

namespace Toreo {
  GUILoader::GUILoader(const std::string folder_address, Shader *shader, Core *core) :
    folder_address_(folder_address),
    is_ready_(false),
    is_loaded_(false),
    core_(core),
    shader_(shader),
    buffer_(new Buffer()),
    data_size_(0),
    buffer_data_(0),
    error_(true),
    error_log_("GUI element not loaded yet")
  {

    if(check_folder()){
      runner_ = boost::thread(boost::bind(&GUILoader::run, this));
      runner_.detach();
    }
  }

  GUILoader::~GUILoader(){
    delete buffer_;
  }

  void GUILoader::draw(){
    if(is_loaded_){
      // Loading data buffer
      buffer_->vertex_bind();
      glDrawArrays(GL_TRIANGLES, 0, data_size_);
      buffer_->vertex_release();
    }else
      model_ready();
  }

  bool GUILoader::check_folder(){
    if(folder_address_.front() != '/') folder_address_ = "/" + folder_address_;
    if(folder_address_.back() != '/') folder_address_ += "/";

    std::string first_path(folder_address_);

    if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
      folder_address_ = boost::filesystem::current_path().string() + folder_address_;

      if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
        core_->message_handler("*** Model loader: ***\n The file: " + first_path +
                               " was not found.\n  Neither: " + folder_address_ + "\n",
                               Visualizer::Message::ERROR);
        return false;
      }
    }
    return true;
  }

  void GUILoader::run(){
    std::vector<Algebraica::vec3f> position, normal;
    std::vector<Algebraica::vec2f> texture;
    Algebraica::vec3f tvector;
    Algebraica::vec2f ttexture;
    std::vector<unsigned int> vertex_indices, texture_indices, normal_indices;
    unsigned int vertex_index[3], texture_index[3], normal_index[3];
    std::string line;

    protector_.lock();
    std::ifstream file;
    file.open(folder_address_ + "model.obj");
    protector_.unlock();

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

      Algebraica::vec3f v0, v1, v2, dP1, dP2, tangent, bitangent;
      Algebraica::vec2f uv0, uv1, uv2, dUV1, dUV2;
      int e{0};

      protector_.lock();
      int total{static_cast<int>(vertex_indices.size())};
      buffer_data_.resize(total);
      // For each vertex of each triangle
      for(int i = 0; i < total; i++){
        buffer_data_[i].position = position[vertex_indices[i] - 1];
        buffer_data_[i].normal = normal[normal_indices[i] - 1];
        if(texture_indices[i] > 0)
          buffer_data_[i].texture = texture[texture_indices[i] - 1];
        else
          buffer_data_[i].texture = Algebraica::vec2f();

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
      }

      data_size_ = static_cast<GLsizei>(total * sizeof(Visualizer::ComplexShaderData));

      protector_.unlock();

      protector_.lock();
      is_ready_ = true;
      error_ = false;
      error_log_.clear();
      protector_.unlock();
    }else{
      protector_.lock();
      error_ = true;
      error_log_ = "File not found:" + folder_address_ + "...\n----------\n";
      is_ready_ = false;
      protector_.unlock();
    }
  }

  void GUILoader::model_ready(){
    if(protector_.try_lock()){
      protector_.unlock();
      if(!error_ && is_ready_){
        shader_->use();

        i_position_  = shader_->attribute_location("i_position");
        i_normal_    = shader_->attribute_location("i_normal");
        i_tangent_   = shader_->attribute_location("i_tangent");
        i_bitangent_ = shader_->attribute_location("i_bitangent");
        i_uv_        = shader_->attribute_location("i_uv");

        GLsizei stride_size{sizeof(Visualizer::ComplexShaderData)};

        buffer_->create();
        buffer_->vertex_bind();
        buffer_->allocate_array(buffer_data_.data(), data_size_, GL_STATIC_DRAW);
        buffer_data_.clear();

        GLint offset{0};
        buffer_->enable(i_position_);
        buffer_->attributte_buffer(i_position_, _3D, offset, stride_size);

        offset += sizeof(Algebraica::vec3f);
        buffer_->enable(i_normal_);
        buffer_->attributte_buffer(i_normal_, _3D, offset, stride_size);

        offset += sizeof(Algebraica::vec3f);
        buffer_->enable(i_tangent_);
        buffer_->attributte_buffer(i_tangent_, _3D, offset, stride_size);

        offset += sizeof(Algebraica::vec3f);
        buffer_->enable(i_bitangent_);
        buffer_->attributte_buffer(i_bitangent_, _3D, offset, stride_size);

        offset += sizeof(Algebraica::vec3f);
        buffer_->enable(i_uv_);
        buffer_->attributte_buffer(i_uv_, _2D, offset, stride_size);

        buffer_->vertex_release();

        is_loaded_ = true;
      }else
        core_->message_handler(error_log_, Visualizer::Message::ERROR);
    }
  }
}
