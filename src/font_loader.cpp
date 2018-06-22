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
    file_exists_(true),
    is_ready_(false),
    is_loaded_(false),
    core_(core),
    t_distance_(nullptr),
    characters_(0),
    error_(false),
    error_log_("Font not loaded yet...\n----------\n")
  {
    if(check_path(&font_distance_path_) &&
       check_path(&font_info_path_)){
      file_exists_ = true;
      load_font();
    }
  }

  FontLoader::~FontLoader(){
    if(t_distance_) delete t_distance_;
  }

  void FontLoader::use(){
    if(is_loaded_)
      if(t_distance_) t_distance_->use();
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
        error_ = true;
        error_log_ = "*** Font loader: ***\n The file: " + first_path +
                     " was not found.\n  Neither: " + *path + "\n";
        core_->message_handler(error_log_, Visualizer::Message::ERROR);
        return false;
      }
    }
    return true;
  }

  void FontLoader::load_font(){
    std::ifstream file;
    std::string line;
    std::size_t position(0u);
    std::size_t i{0u};
    unsigned int maximum{0u};
    Visualizer::FontCharacter letter;
    float scale_w, scale_h, base, font_size;
    float padding_top, padding_right, padding_bottom, padding_left;
    float space_x, space_y;
    file.open(font_info_path_);

    std::vector<Visualizer::FontCharacter> temporal_data(0);

    if(file.is_open()){
      while(std::getline(file, line)){
        if(position == 0u){
          std::size_t column{line.find("padding") + 8u};
          std::string sub_line(line.substr(column));
          const char *padding = sub_line.c_str();

          std::sscanf(padding, "%f,%f,%f,%f %*8s%f,%f",
                      &padding_top, &padding_right, &padding_bottom, &padding_left,
                      &space_x, &space_y);

          const char *description = line.c_str();
          std::sscanf(description, "%*s %*s %*5s%f", &font_size);

          ++position;
        }else if(position == 1u){
          std::size_t column{line.find("base") + 5u};
          std::string sub_line(line.substr(column));
          const char *scale = sub_line.c_str();

          std::sscanf(scale, "%f %*7s%f %*7s%f", &base, &scale_w, &scale_h);
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
          std::sscanf(line_c, "%*s %*3s%u %*2s%f %*2s%f %*6s%f %*7s%f %*8s%f %*8s%f %*9s%f",
                      &letter.ascii, &x, &y, &letter.width, &letter.height,
                      &letter.x, &letter.y, &letter.next);

          maximum = (letter.ascii > maximum)? letter.ascii : maximum;

          letter.u1 = x / scale_w;
          letter.v1 = y / scale_h;
          letter.u2 = (x + letter.width) / scale_w;
          letter.v2 = (y + letter.height) / scale_h;

          letter.width /= font_size;
          letter.height /= font_size;
          letter.next = (letter.next - padding_left - padding_right) / font_size;
          letter.x = (letter.x - padding_left) / font_size;
          letter.y = (letter.y - padding_top) / font_size;

          temporal_data[i++] = letter;
        }else
          ++position;
      }

      characters_.resize(maximum + 1u);
      for(const Visualizer::FontCharacter &character : temporal_data)
        characters_[character.ascii] = character;

      file.close();
    }
  }

  void FontLoader::run(){
    if(file_exists_){
      protector_.lock();
      stbi_set_flip_vertically_on_load(false);
      // loading font distance image
      distance_.data = stbi_load(font_distance_path_.c_str(),
                                 &distance_.width, &distance_.height,
                                 &distance_.components_size, 0);
      if(distance_.data){
        is_ready_ = true;
        error_ = false;
        error_log_.clear();
      }else{
        error_ = true;
        error_log_ = "File not porperly read:" + font_distance_path_ +
                     " (check format)...\n----------\n";
        is_ready_ = false;
      }
      protector_.unlock();
    }
  }

  void FontLoader::ready(){
    if(!error_ && is_ready_ && !is_loaded_){
      if(distance_.data)
        t_distance_ = new Texture(9, core_->screen_max_anisotropic_filtering(), &distance_);

      stbi_image_free(distance_.data);

      is_loaded_ = true;
    }else if(error_)
      core_->message_handler(error_log_, Visualizer::Message::ERROR);
  }

  const bool FontLoader::is_ready(){
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
}
