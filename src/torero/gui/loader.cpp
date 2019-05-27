#include "torero/core.h"
#include "torero/gui/loader.h"
#include "torero/terminal/printer.h"

namespace torero {
  namespace gui {
    Loader::Loader(const std::string folder_address, torero::gl::Shader *shader,
                   torero::Core *core) :
      folder_address_(folder_address),
      is_ready_(false),
      is_loaded_(false),
      core_(core),
      shader_(shader),
      buffer_(new torero::gl::Buffer()),
      data_size_(0),
      buffer_data_(0),
      error_(true),
      error_log_("GUI element not loaded yet")
    {

      if(check_folder()){
        runner_ = boost::thread(boost::bind(&gui::Loader::run, this));
        runner_.detach();
      }
    }

    Loader::~Loader(){
      delete buffer_;
    }

    void Loader::draw(){
      if(is_loaded_){
        // Loading data buffer
        buffer_->vertex_bind();
        glDrawArrays(GL_TRIANGLES, 0, data_size_);
        buffer_->vertex_release();
      }else
        model_ready();
    }

    bool Loader::check_folder(){
      if(folder_address_.front() != '/') folder_address_ = "/" + folder_address_;
      if(folder_address_.back() != '/') folder_address_ += "/";

      std::string first_path(folder_address_);

      if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
        folder_address_ = boost::filesystem::current_path().string() + folder_address_;

        if(!boost::filesystem::exists(boost::filesystem::path(folder_address_))){
          torero::terminal::Printer::error("*** Model loader: ***\n The file: " + first_path +
                                           " was not found.\n  Neither: " + folder_address_ + "\n");
          return false;
        }
      }
      return true;
    }

    void Loader::run(){
      std::vector<algebraica::vec3f> position, normal;
      std::vector<algebraica::vec2f> texture;
      algebraica::vec3f tvector;
      algebraica::vec2f ttexture;
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

        algebraica::vec3f v0, v1, v2, dP1, dP2, tangent;
        algebraica::vec2f uv0, uv1, uv2, dUV1, dUV2;
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

            buffer_data_[i].tangent = tangent;
            buffer_data_[i - 1].tangent = tangent;
            buffer_data_[i - 2].tangent = tangent;
          }
        }

        data_size_ = static_cast<GLsizei>(total * sizeof(torero::ComplexShaderData));

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

    void Loader::model_ready(){
      if(protector_.try_lock()){
        protector_.unlock();
        if(!error_ && is_ready_){
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
          buffer_->enable(torero::ShaderLocation::UV);
          buffer_->attributte_buffer(torero::ShaderLocation::UV, _2D, offset, stride_size);

          offset += sizeof(algebraica::vec2f);
          buffer_->enable(torero::ShaderLocation::Normal);
          buffer_->attributte_buffer(torero::ShaderLocation::Normal, _3D, offset, stride_size);

          offset += sizeof(algebraica::vec3f);
          buffer_->enable(torero::ShaderLocation::Tangent);
          buffer_->attributte_buffer(torero::ShaderLocation::Tangent, _3D, offset, stride_size);

          buffer_->vertex_release();

          is_loaded_ = true;
        }else
          torero::terminal::Printer::error(error_log_);
      }
    }
  }
}
