#include "torero/text_manager.h"
#include "torero/core.h"

namespace Toreo {
  TextManager::TextManager(Core *core) :
    core_(core),
    shader_(new Shader("resources/shaders/text.vert",
                       "resources/shaders/text.frag",
                       "resources/shaders/text.geom")),
    u_projection_(shader_->uniform_location("u_projection")),
    u_view_(shader_->uniform_location("u_view")),
    u_window_(shader_->uniform_location("u_window")),
    width_(core->width()),
    height_(core->height()),
    fonts_(0),
    text_indices_(),
    texts_(0),
    transformation_matrix_(nullptr),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&TextManager::updated_camera, this))),
    signal_draw_all_(core->syncronize(Visualizer::TEXT)->
                     connect(boost::bind(&TextManager::draw_all, this)))
  {
    initialize();
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

  FTid TextManager::font_add(const std::string font_distance_path,
                              const std::string font_info_path){
    FontLoader *new_font = new FontLoader(font_distance_path, font_info_path, core_);
    core_->multithread_add_process(boost::bind(&FontLoader::run, new_font),
                                   boost::bind(&FontLoader::ready, new_font),
                                   boost::bind(&FontLoader::is_ready, new_font));
    fonts_.push_back(new_font);
    return fonts_.size() - 1;
  }

  bool TextManager::font_use(const FTid font){
    if(fonts_.size() > font)
      if(fonts_[font]){
        fonts_[font]->use();
        return true;
      }else
        return false;
    else
      return false;
  }

  const std::vector<Visualizer::FontCharacter> *TextManager::font_characters(const FTid font) const{
    if(fonts_.size() > font)
      if(fonts_[font]){
        return fonts_[font]->characters();
      }else
        return nullptr;
    else
      return nullptr;
  }

  bool TextManager::font_delete(FTid font){
    bool ok{false};

    if(fonts_.size() > font)
      if(fonts_[font]){
        try{
          for(Visualizer::TextIndex index : text_indices_.at(font))
            if(index.exist && texts_.size() > index.index)
              if(texts_[index.index].text)
                delete texts_[index.index].text;

          text_indices_.erase(font);
        }catch(const std::out_of_range&){
        }
        delete fonts_[font];
        fonts_[font] = nullptr;
        ok = true;
      }
    return ok;
  }

  void TextManager::font_purge(){
    purge();
    for(FontLoader *font : fonts_)
      if(font)
        delete font;

    fonts_.clear();
  }

  bool TextManager::font_change(TXMid id, FTid new_font){
    bool ok{false};

    if(fonts_.size() > new_font)
      if(fonts_[new_font])
        if(texts_.size() > id)
          if(texts_[id].text){
            if(text_indices_.find(texts_[id].font) != text_indices_.end())
              text_indices_[texts_[id].font][texts_[id].index].exist = false;
            texts_[id].font = new_font;
            text_indices_[new_font].push_back({static_cast<std::size_t>(id), true});
            ok = true;
          }

    return ok;
  }

  TXMid TextManager::add_3D(FTid font, const std::vector<Visualizer::TextSimple> *data,
                            const std::string name, const Algebraica::mat4f *transformation_matrix,
                            const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_, Visualizer::THREE_DIMENSIONAL),
                                  font, name, visible };
        if(transformation_matrix != nullptr)
          text.text->set_transformation_matrix(transformation_matrix);

        text.text->set_font(fonts_[font]);
        text.text->set_input(data);

        texts_.push_back(text);
        std::size_t id{texts_.size() - 1u};

        text_indices_[font].push_back({id, true});
        texts_[id].index = text_indices_.size() - 1u;

        return static_cast<TXMid>(id);
      }else
        return -1;
  }

  TXMid TextManager::add_3D(FTid font, const std::vector<Visualizer::TextColored> *data,
                         const std::string name, const Algebraica::mat4f *transformation_matrix,
                         const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_, Visualizer::THREE_DIMENSIONAL),
                                  font, name, visible };
        if(transformation_matrix != nullptr)
          text.text->set_transformation_matrix(transformation_matrix);

        text.text->set_font(fonts_[font]);
        text.text->set_input(data);

        texts_.push_back(text);
        std::size_t id{texts_.size() - 1u};

        text_indices_[font].push_back({id, true});
        texts_[id].index = text_indices_.size() - 1u;

        return static_cast<TXMid>(id);
      }else
        return -1;
  }

  TXMid TextManager::add_2D(FTid font, const std::vector<Visualizer::TextSimple> *data,
                            const std::string name, const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_, Visualizer::TWO_DIMENSIONAL),
                                  font, name, visible };

        text.text->set_font(fonts_[font]);
        text.text->set_input(data);

        texts_.push_back(text);
        std::size_t id{texts_.size() - 1u};

        text_indices_[font].push_back({id, true});
        texts_[id].index = text_indices_.size() - 1u;

        return static_cast<TXMid>(id);
      }else
        return -1;
  }

  TXMid TextManager::add_2D(FTid font, const std::vector<Visualizer::TextColored> *data,
                            const std::string name, const bool visible){
    if(fonts_.size() > font)
      if(fonts_[font]){
        Visualizer::Text text = { new TextObject(shader_, Visualizer::TWO_DIMENSIONAL),
                                  font, name, visible };

        text.text->set_font(fonts_[font]);
        text.text->set_input(data);

        texts_.push_back(text);
        std::size_t id{texts_.size() - 1u};

        text_indices_[font].push_back({id, true});
        texts_[id].index = text_indices_.size() - 1u;

        return static_cast<TXMid>(id);
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
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_foggy(foggy);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_font(TXMid id, FTid font){
    if(texts_.size() > id && fonts_.size() > font)
      if(texts_[id].text && fonts_[font]){
        texts_[id].font = font;
        texts_[id].text->set_font(fonts_[font]);
        return true;
      }else
        return false;
    else
      return false;
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

  bool TextManager::set_font_size(TXMid id, const unsigned int size){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_size(size);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_font_weight(TXMid id, const Visualizer::TextWeight width){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_weight(width);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_edge_softness(TXMid id, const unsigned int softness){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_edge_softness(softness);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_border_width(TXMid id, const unsigned int width){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_border_width(width);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_border_softness(TXMid id, const unsigned int softness){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_border_softness(softness);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_border_color(TXMid id, const float R, const float G,
                                     const float B, const float A){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_border_color(R/255.0f, G/255.0f, B/255.0f, A/255.0f);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_border_offset(TXMid id, const unsigned int x, const unsigned int y){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_border_offset(x, y);
        return true;
      }else
        return false;
    else
      return false;
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

  bool TextManager::set_line_height(TXMid id, const float line_height){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_line_height(line_height);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_horizontal_alignment(TXMid id, const Visualizer::Alignment alignment){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_horizontal_alignment(alignment);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TextManager::set_vertical_alignment(TXMid id, const Visualizer::Alignment alignment){
    if(texts_.size() > id)
      if(texts_[id].text){
        texts_[id].text->set_vertical_alignment(alignment);
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
    bool ok{false};

    if(texts_.size() > id)
      if(texts_[id].text)
        if(fonts_.size() > texts_[id].font)
          if(fonts_[texts_[id].font]){
            fonts_[texts_[id].font]->use();
            texts_[id].text->draw();
            ok = true;
          }
    return ok;
  }

  void TextManager::draw_all(){
    std::map<FTid, std::vector<Visualizer::TextIndex> >::iterator it = text_indices_.begin();
    while(it != text_indices_.end()){
      fonts_[it->first]->use();
      for(Visualizer::TextIndex index : it->second)
        if(index.exist)
          texts_[index.index].text->draw();
      ++it;
    }
  }

  bool TextManager::delete_text(TXMid id){
    if(texts_.size() > id)
      if(texts_[id].text){
        if(texts_[id].connection.connected())
          texts_[id].connection.disconnect();
        delete texts_[id].text;
        texts_[id].text = nullptr;
        text_indices_[texts_[id].font][texts_[id].index].exist = false;
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
    text_indices_.clear();
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
    shader_->set_value(u_projection_, core_->camera_matrix_perspective());
    shader_->set_value(u_view_, core_->camera_matrix_view());
    shader_->set_value(u_window_, Algebraica::vec2f(FLOAT(*width_), FLOAT(*height_)));
  }

  void TextManager::initialize(){
    if(!shader_->use()){
      std::cout << shader_->error_log() << std::endl;
    }else{
      updated_camera();
      shader_->set_value(shader_->uniform_location("u_font_alias"), 9);
    }
  }
}
