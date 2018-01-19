#include "torero/text_manager.h"
#include "torero/core.h"

namespace Toreo {
  TextManager::TextManager(Core *core) :
    core_(core),
    shader_(new Shader("resources/shaders/text.vert",
                       "resources/shaders/text.frag",
                       "resources/shaders/text.geom")),
    fonts_(0),
    transformation_matrix_(nullptr)
  {
  }

  TextManager::~TextManager(){
    for(Visualizer::Text &text : texts_)
      if(text.text)
        delete text.text;

    for(FontLoader *font : fonts_)
      if(font)
        delete font;

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();

    if(signal_draw_all_.connected())
      signal_draw_all_.disconnect();

    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();

    if(shader_) delete shader_;
  }

  FTid TextManager::load_font(const std::string font_distance_path,
                              const std::string font_info_path){
    FontLoader *new_font = new FontLoader(font_distance_path, font_info_path, core_);
    fonts_.push_back(new_font);
    return fonts_.size() - 1;
  }

  bool TextManager::delete_font(FTid font){
    if(fonts_.size() > font)
      if(texts_[font].text != nullptr){
        for(Visualizer::Text &text : texts_)
          if(text.font == font){
            delete text.text;
            text.text = nullptr;
            text.name.clear();
            if(text.connection.connected())
              text.connection.disconnect();
          }
        delete texts_[font].text;
        texts_[font].text = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void TextManager::purge_fonts(){
    purge();
    for(FontLoader *font : fonts_)
      if(font)
        delete font;

    fonts_.clear();
  }

  bool TextManager::change_font(TXMid id, FTid new_font){
    if(fonts_.size() > new_font)
      if(fonts_[new_font])
        if(texts_.size() > id)
          if(texts_[id].text)
            texts_[id].font = new_font;
  }

  TXMid TextManager::add(FTid font, const std::vector<Visualizer::TextSimple> *data,
                         const std::string name, const Algebraica::mat4f *transformation_matrix,
                         const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_), font, name, visible };
        if(transformation_matrix != nullptr)
          text.text->set_transformation_matrix(transformation_matrix);

        text.text->set_input(data);

        texts_.push_back(text);
        return texts_.size() - 1;
      }else
        return -1;
  }

  TXMid TextManager::add(FTid font, const std::vector<Visualizer::TextColored> *data,
                         const std::string name, const Algebraica::mat4f *transformation_matrix,
                         const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_), font, name, visible };
        if(transformation_matrix != nullptr)
          text.text->set_transformation_matrix(transformation_matrix);

        text.text->set_input(data);

        texts_.push_back(text);
        return texts_.size() - 1;
      }else
        return -1;
  }

  TXMid TextManager::add(FTid font, const std::vector<Visualizer::TextExtended> *data,
                         const std::string name, const Algebraica::mat4f *transformation_matrix,
                         const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_), font, name, visible };
        if(transformation_matrix != nullptr)
          text.text->set_transformation_matrix(transformation_matrix);

        text.text->set_input(data);

        texts_.push_back(text);
        return texts_.size() - 1;
      }else
        return -1;
  }

  bool TextManager::change_input(TXMid id, const std::vector<Visualizer::TextSimple> *data){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_input(data);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::change_input(TXMid id, const std::vector<Visualizer::TextColored> *data){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_input(data);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::change_input(TXMid id, const std::vector<Visualizer::TextExtended> *data){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_input(data);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_visibility(TXMid id, const bool visible){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].visibility = visible;
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_fog(TXMid id, const bool foggy){

  }

  bool TextManager::set_font_color(TXMid id, const float R, const float G,
                              const float B, const float A){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_color(R/255.0f, G/255.0f, B/255.0f, A/255.0f);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_font_size(TXMid id, const float size){

  }

  bool TextManager::set_font_widthness(TXMid id, const float width){

  }

  bool TextManager::set_edge_softness(TXMid id, const float softness){

  }

  bool TextManager::set_border_width(TXMid id, const float width){

  }

  bool TextManager::set_border_softness(TXMid id, const float softness){

  }

  bool TextManager::set_border_color(TXMid id, const float R, const float G,
                                     const float B, const float A){

  }

  bool TextManager::set_border_offset(TXMid id, const float x, const float y){

  }

  bool TextManager::set_transformation_matrix(TXMid id,
                                              const Algebraica::mat4f *transformation_matrix){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::translate(TXMid id, const float x, const float y, const float z){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::rotate(TXMid id, const float pitch, const float yaw, const float roll){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::rotate_in_x(TXMid id, const float angle){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::rotate_in_y(TXMid id, const float angle){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::rotate_in_z(TXMid id, const float angle){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->rotate_in_y(angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::update(TXMid id){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void TextManager::update_all(){
    for(Visualizer::Text &text : texts_)
      if(text.text && text.visibility)
        text.text->update();
  }

  bool TextManager::draw(TXMid id){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void TextManager::draw_all(){
    for(Visualizer::Text &text : texts_)
      if(text.text && text.visibility)
        text.text->draw();
  }

  bool TextManager::delete_text(TXMid id){
    if(texts_.size() > id)
      if(texts_[id].text){
        if(texts_[id].connection.connected())
          texts_[id].connection.disconnect();
        delete texts_[id].text;
        texts_[id].text = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void TextManager::purge(){
    for(Visualizer::Text &text : texts_)
      if(text.text){
        if(text.connection.connected())
          text.connection.disconnect();
        delete text.text;
      }
    texts_.clear();
  }

  bool TextManager::connect(TXMid id, boost::signals2::signal<void ()> *signal){
    if(texts_.size() > id)
      if(texts_[id].text){
        if(texts_[id].connection.connected())
          texts_[id].connection.disconnect();
        texts_[id].connection =
            signal->connect(boost::bind(&TextManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void TextManager::connect_all(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&TextManager::update_all, this));
  }

  void TextManager::updated_camera(){
    shader_->use();
    shader_->set_value(u_pv_, core_->camera_matrix_perspective_view());
  }
}
