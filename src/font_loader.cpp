#include "torero/font_loader.h"
#include "torero/core.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  FontLoader::FontLoader(const std::string font_distance_path,
                         const std::string font_info_path,
                         Core *core) :
    font_distance_path_(font_distance_path),
    font_info_path_(font_info_path),
    is_ready_(false),
    is_loaded_(false),
    core_(core),
    t_distance_(nullptr),
    characters_(0),
    padding_left_(0),
    padding_top_(0),
    padding_right_(0),
    padding_bottom_(0),
    space_x_(0),
    space_y_(0),
    scale_w_(0),
    scale_h_(0),
    error_(false),
    error_log_("Font not loaded yet...\n----------\n")
  {
    if(check_path(&font_distance_path_) &&
       check_path(&font_info_path_)){
      runner_ = boost::thread(boost::bind(&FontLoader::run, this));
      runner_.detach();
    }
  }

  FontLoader::~FontLoader(){
    if(t_distance_) delete t_distance_;
  }

  void FontLoader::use(){
    if(is_loaded_){
      if(t_distance_) t_distance_->use();
    }else
      ready();
  }

  const std::vector<Visualizer::FontCharacter> *FontLoader::characters() const{
    return &characters_;
  }

  bool FontLoader::check_path(std::string *path){
    if(path->front() != '/') *path = "/" + *path;

    std::string first_path(*path);

    if(!boost::filesystem::exists(boost::filesystem::path(*path))){
      *path = boost::filesystem::current_path().string() + *path;

      if(!boost::filesystem::exists(boost::filesystem::path(*path))){
        core_->message_handler("*** Font loader: ***\n The file: " + first_path +
                               " was not found.\n  Neither: " + *path + "\n",
                               Visualizer::ERROR);
        return false;
      }
    }
    return true;
  }

  void FontLoader::run(){
    protector_.lock();
    std::ifstream file;
    std::string line;
    std::size_t position(0u);
    std::size_t i{0u};
    int maximum{0};
    Visualizer::FontCharacter letter;
    file.open(font_info_path_);

    std::vector<Visualizer::FontCharacter> temporal_data(0);

    if(file.is_open()){
      while(std::getline(file, line)){
        if(position == 0u){
          std::size_t column{line.find("padding") + 8u};
          std::string sub_line(line.substr(column));
          const char *padding = sub_line.c_str();

          std::sscanf(padding, "%d,%d,%d,%d %*8s%d,%d",
                      &padding_top_, &padding_right_, &padding_bottom_, &padding_left_,
                      &space_x_, &space_y_);
          ++position;
        }else if(position == 1u){
          std::size_t column{line.find("scaleW") + 7u};
          std::string sub_line(line.substr(column));
          const char *scale = sub_line.c_str();

          std::sscanf(scale, "%d %*7s%d", &scale_w_, &scale_h_);
          ++position;
        }else if(position == 3u){
          int data_size{0};
          const char *counting = line.c_str();

          std::sscanf(counting, "%*s %*6s%i", &data_size);
          temporal_data.resize(data_size);
          ++position;
        }else if(position > 4u){
          if(line[0] != 'c') continue;

          const char *line_c = line.c_str();
          float x{-2.0f}, y{-2.0f};
          std::sscanf(line_c, "%*s %*3s%d %*2s%f %*2s%f %*6s%f %*7s%f %*8s%f %*8s%f %*9s%f",
                      &letter.ascii, &x, &y, &letter.width, &letter.height,
                      &letter.x, &letter.y, &letter.next);

          maximum = (letter.ascii > maximum)? letter.ascii : maximum;

          letter.u = x / scale_w_;
          letter.v = y / scale_h_;
          temporal_data[i++] = letter;
        }else
          ++position;
      }

      std::size_t max_data{temporal_data.size()};
      characters_.resize(static_cast<std::size_t>(maximum) + 1u);

      for(std::size_t e = 0u; e < max_data; ++e)
        characters_[static_cast<std::size_t>(temporal_data[e].ascii)] = temporal_data[e];

      file.close();
      stbi_set_flip_vertically_on_load(true);

      // loading font distance image
      distance_.data = stbi_load(std::string(font_distance_path_).c_str(),
                                 &distance_.width, &distance_.height,
                                 &distance_.components_size, 0);
      protector_.unlock();

      protector_.lock();
      is_ready_ = true;
      error_ = false;
      error_log_.clear();
      protector_.unlock();
    }else{
      protector_.lock();
      error_ = true;
      error_log_ = "File not found:" + font_info_path_ + "...\n----------\n";
      is_ready_ = false;
      protector_.unlock();
    }
  }

  void FontLoader::ready(){
    if(protector_.try_lock()){
      protector_.unlock();
      if(!error_ && is_ready_){

        if(distance_.data)
          t_distance_ = new Texture(4, core_->max_anisotropic_filtering(), &distance_);

        stbi_image_free(distance_.data);

        is_loaded_ = true;
      }else
        core_->message_handler(error_log_, Visualizer::ERROR);
    }
  }
}
