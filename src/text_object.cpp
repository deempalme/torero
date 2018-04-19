#include "torero/text_object.h"

namespace Toreo {
  TextObject::TextObject(Shader *shader, const Visualizer::TextType dimensions) :
    shader_(shader),
    buffer_(true),
    font_(nullptr),
    type_(Visualizer::Complexity::SIMPLE),
    weight_(Visualizer::TextWeight::NORMAL),
    is_3D_(dimensions == Visualizer::THREE_DIMENSIONAL),
    text_simple_(nullptr),
    text_colored_(nullptr),
    text_origins_(0),
    primary_model_(nullptr),
    secondary_model_(),
    identity_matrix_(),
    color_(255.0f, 255.0f, 255.0f, 255.0f),
    border_color_(127.5f, 127.5f, 127.5f, 255.0f),
    line_height_(1.2f),
    size_softness_weight_(15.0f, 1.0f, 0.2f),
    border_width_softness_offset_(0.0f, 0.0f, 0.0f, 0.0f),
    has_fog_(false),
    has_border_(false),
    has_color_(false),
    has_orientation_(1.0f),
    has_fog_int_(0),
    has_border_int_(0),
    has_color_int_(0),
    vertical_alignment_(Visualizer::Alignment::TOP),
    horizontal_alignment_(Visualizer::Alignment::LEFT),
    type_size_(sizeof(Visualizer::TextSimpleShader)),
    data_size_(0),
    size_1_(sizeof(float)),
    size_2_(size_1_ * 2),
    size_3_(size_1_ * 3),
    size_4_(size_1_ * 4)
  {
    initialize();
  }

  void TextObject::set_input(const std::vector<Visualizer::TextSimple> *text){
    if(type_ == Visualizer::Complexity::SIMPLE)
      text_simple_ = text;
    else{
      text_simple_ = text;
      text_colored_ = nullptr;
      type_ = Visualizer::Complexity::SIMPLE;
      type_size_ = sizeof(Visualizer::TextSimpleShader);
      data_size_ = 0;
      has_color_ = true;
      has_color_int_ = 1;
      has_orientation_ = 0.0f;
    }
  }

  void TextObject::set_input(const std::vector<Visualizer::TextColored> *text){
    if(type_ == Visualizer::Complexity::MEDIUM)
      text_colored_ = text;
    else{
      text_simple_ = nullptr;
      text_colored_ = text;
      type_ = Visualizer::Complexity::MEDIUM;
      type_size_ = sizeof(Visualizer::TextColoredShader);
      data_size_ = 0;
      has_orientation_ = 1.0f;
    }
  }

  void TextObject::set_font(FontLoader *font){
    font_ = font;
  }

  void TextObject::set_color(const float R, const float G, const float B, const float A){
    color_(R, G, B, A);
    has_color_ = true;
    has_color_int_ = 1;
  }

  void TextObject::set_foggy(const bool fog){
    has_fog_ = fog;
    has_fog_int_ = (has_fog_)? 1 : 0;
  }

  void TextObject::set_size(const unsigned int size){
    size_softness_weight_[0] = FLOAT(size);
    set_weight(weight_);
  }

  void TextObject::set_line_height(const float line_height){
    line_height_ = line_height;
  }

  void TextObject::set_vertical_alignment(const Visualizer::Alignment alignment){
    vertical_alignment_ = alignment;
  }

  void TextObject::set_horizontal_alignment(const Visualizer::Alignment alignment){
    horizontal_alignment_ = alignment;
  }

  void TextObject::set_weight(const Visualizer::TextWeight weight){
    weight_ = weight;
    float multiplier;
    switch(weight){
    case Visualizer::TextWeight::LIGHTER:
      multiplier = 0.03f;
    break;
    case Visualizer::TextWeight::LIGHT:
      multiplier = 0.06f;
    break;
    case Visualizer::TextWeight::BOLD:
      multiplier = 0.15f;
    break;
    case Visualizer::TextWeight::BOLDER:
      multiplier = 0.2f;
    break;
    default:
      multiplier = 0.1f;
    break;
    }
    size_softness_weight_[2] = std::round(multiplier * size_softness_weight_[0]);
  }

  bool TextObject::set_edge_softness(const unsigned int softness){
    size_softness_weight_[1] = FLOAT(softness);
  }

  bool TextObject::set_border_width(const unsigned int width){
    border_width_softness_offset_[0] = FLOAT(width);
    has_border_ = width > 0;
    has_border_int_ = (has_border_)? 1 : 0;
  }

  bool TextObject::set_border_softness(const unsigned int softness){
    border_width_softness_offset_[1] = FLOAT(softness);
  }

  bool TextObject::set_border_color(const float R, const float G, const float B, const float A){
    border_color_(R, G, B, A);
  }

  bool TextObject::set_border_offset(const unsigned int x, const unsigned int y){
    border_width_softness_offset_[2] = FLOAT(x);
    border_width_softness_offset_[3] = FLOAT(y);
  }

  void TextObject::set_transformation_matrix(const Algebraica::mat4f *transformation_matrix){
    primary_model_ = transformation_matrix;
  }

  void TextObject::translate(const float x, const float y, const float z){
    secondary_model_.translate(x, y, z);
  }

  void TextObject::rotate(const float pitch, const float yaw, const float roll){
    secondary_model_.rotate(pitch, yaw, roll);
  }

  void TextObject::rotate_in_x(const float angle){
    secondary_model_.rotate_z(angle);
  }

  void TextObject::rotate_in_y(const float angle){
    secondary_model_.rotate_x(angle);
  }

  void TextObject::rotate_in_z(const float angle){
    secondary_model_.rotate_y(angle);
  }

  bool TextObject::update(){
    bool no_error{shader_->use() && font_};

    if(no_error){
      if(type_ == Visualizer::Complexity::MEDIUM){
//        std::size_t counter{0u}, index{0u};
//        text_origins_.resize(text_colored_->size());
//        for(const Visualizer::TextColored &text : *text_colored_){
//          counter += text.text.size();
//        }

//        std::vector<Visualizer::TextColoredShader> buffer_data(counter);

//        for(const Visualizer::TextColored &text : *text_colored_){
//          std::vector<float> line_start(0);
//          Algebraica::vec2f offset;
//          std::size_t previous_index{index};

//          text_metrics(text.text, &offset, &line_start, &index, nullptr, &buffer_data);

//          for(std::size_t i = previous_index; i < index; ++i){
//            buffer_data[i].r = text.r;
//            buffer_data[i].g = text.g;
//            buffer_data[i].b = text.b;
//            buffer_data[i].a = text.alpha;

//            buffer_data[i].x = -text.y;
//            buffer_data[i].y =  text.z;
//            buffer_data[i].z = -text.x;

//            buffer_data[i].p_x += offset[0];
//            buffer_data[i].p_y += offset[1];

//            buffer_data[i].angle = text.angle;

//            std::cout << " x: " << buffer_data[i].x << " y: " << buffer_data[i].y
//                      << " z: " << buffer_data[i].z
//                      << " p_x: " << buffer_data[i].p_x * size_softness_weight_[0]
//                      << " p_y: " << buffer_data[i].p_y * size_softness_weight_[0]
//                      << " width: " << buffer_data[i].width * size_softness_weight_[0]
//                      << " height: " << buffer_data[i].height * size_softness_weight_[0]
//                      << std::endl;
//          }
//        }
//        buffer_data.resize(index);

        std::vector<Visualizer::TextColoredShader> buffer_data(1);
        buffer_data[0].x = 0.0f;
        buffer_data[0].y = 0.0f;
        buffer_data[0].z = 0.0f;

        data_size_ = buffer_data.size();
        buffer_.vertex_bind();
        buffer_.allocate_array(buffer_data.data(), data_size_ * type_size_, GL_DYNAMIC_DRAW);

        GLint offset{0};
        buffer_.enable(i_origin_);
        buffer_.attributte_buffer(i_origin_, _3D, 0, type_size_);

//        offset += size_3_;
//        buffer_.enable(i_position_);
//        buffer_.attributte_buffer(i_position_, _2D, offset, type_size_);

//        offset += size_2_;
//        buffer_.enable(i_size_);
//        buffer_.attributte_buffer(i_size_, _2D, offset, type_size_);

//        offset += size_2_;
//        buffer_.enable(i_color_);
//        buffer_.attributte_buffer(i_color_, _4D, offset, type_size_);

//        offset += size_4_;
//        buffer_.enable(i_texture_);
//        buffer_.attributte_buffer(i_texture_, _4D, offset, type_size_);

//        offset += size_4_;
//        buffer_.enable(i_angle_);
//        buffer_.attributte_buffer(i_angle_, _1D, offset, type_size_);

        buffer_.vertex_release();
      }else{
      }
    }
    return no_error;
  }

  bool TextObject::draw(){
    const bool no_error{shader_->use() && font_};

    if(no_error){
      if(primary_model_)
        shader_->set_value(u_primary_model_, *primary_model_);
      else
        shader_->set_value(u_primary_model_, identity_matrix_);

      shader_->set_value(u_secondary_model_, secondary_model_);

//      shader_->set_value(u_colored_, has_color_int_);
//      if(has_color_)
//        shader_->set_value(u_color_, color_);
//      shader_->set_value(u_size_soft_weight_, size_softness_weight_);
//      shader_->set_value(u_foggy_, has_fog_int_);
//      shader_->set_value(u_border_, has_border_int_);
//      if(has_border_){
//        shader_->set_value(u_border_width_soft_offset_, border_width_softness_offset_);
//        shader_->set_value(u_border_color_, border_color_);
//      }
//      shader_->set_value(u_oriented_, has_orientation_);

      buffer_.vertex_bind();
      glDrawArrays(GL_POINT, 0, data_size_);
      buffer_.vertex_release();
    }
    return no_error;
  }

  void TextObject::initialize(){
    shader_->use();
    // GLSL attribute locations
    i_origin_   = shader_->attribute_location("i_origin");
    i_position_ = shader_->attribute_location("i_position");
    i_size_     = shader_->attribute_location("i_size");
    i_color_    = shader_->attribute_location("i_color");
    i_angle_    = shader_->attribute_location("i_angle");
    i_texture_  = shader_->attribute_location("i_texture");
    // GLSL uniform locations
    u_primary_model_    = shader_->uniform_location("u_primary_model");
    u_secondary_model_  = shader_->uniform_location("u_secondary_model");
    u_colored_          = shader_->uniform_location("u_colored");
    u_color_            = shader_->uniform_location("u_color");
    u_size_soft_weight_ = shader_->uniform_location("u_size_soft_weight");
    u_foggy_            = shader_->uniform_location("u_foggy");
    u_border_           = shader_->uniform_location("u_border");
    u_border_color_     = shader_->uniform_location("u_border_color");
    u_border_width_soft_offset_ = shader_->uniform_location("u_border_width_soft_offset");
    u_oriented_         = shader_->uniform_location("u_oriented");
  }

  void TextObject::text_metrics(const std::string &text, Algebraica::vec2f *offset,
                                std::vector<float> *line_starts, std::size_t *last,
                                std::vector<Visualizer::TextSimpleShader> *simple,
                                std::vector<Visualizer::TextColoredShader> *colored){
//    float maximum_length{0.0f}, counter{0.0f};
//    float lines{1.0f}, font_size{size_softness_weight_[0]}, line_size{font_size * line_height_};
//    const std::size_t size{text.size()};
//    const std::vector<Visualizer::FontCharacter> *font{font_->characters()};
//    const std::size_t fonts_size{font->size()};
//    std::size_t e{*last};

//    for(std::size_t i = 0u; i < size; ++i){
//      std::size_t index;
//      if(text[i] == '\n'){
//        maximum_length = (counter > maximum_length)? counter : maximum_length;
//        line_starts->push_back(counter);
//        counter = 0.0f;
//        ++lines;
////        std::cout << "new line" << std::endl;
//      }else{
//        index = static_cast<std::size_t>(text[i]);
//        if(index >= fonts_size)
//          index = 32u;

//        counter += (*font)[index].next;
////        std::cout << text[i] << " - " << index << " - " << (*font)[index].width << std::endl;
//      }
//      switch(type_){
//      case Visualizer::Complexity::MEDIUM:
//        (*colored)[e].p_x = counter - (*font)[index].next + (*font)[index].x;
//        (*colored)[e].p_y = (lines * line_height_ - (*font)[index].y - (*font)[index].height);
//        (*colored)[e].width = (*font)[index].width;
//        (*colored)[e].height = (*font)[index].height;
//        (*colored)[e].u1 = (*font)[index].u1;
//        (*colored)[e].v1 = (*font)[index].v1;
//        (*colored)[e].u2 = (*font)[index].u2;
//        (*colored)[e++].v2 = (*font)[index].v2;
//      break;
//      default:
//        (*simple)[e].p_x = counter - (*font)[index].next + (*font)[index].x;
//        (*simple)[e].p_y = (lines * line_height_ - (*font)[index].y - (*font)[index].height);
//        (*simple)[e].width = (*font)[index].width;
//        (*simple)[e].height = (*font)[index].height;
//        (*simple)[e].u1 = (*font)[index].u1;
//        (*simple)[e].v1 = (*font)[index].v1;
//        (*simple)[e].u2 = (*font)[index].u2;
//        (*simple)[e++].v2 = (*font)[index].v2;
//      break;
//      }
//    }
//    if(counter > 0.1f)
//      line_starts->push_back(counter);

//    maximum_length = (counter > maximum_length)? counter : maximum_length;

//    *last = e;

//    std::size_t lines_size{line_starts->size()};

//    switch(horizontal_alignment_){
//    case Visualizer::Alignment::CENTER:
//      for(std::size_t i = 0u; i < lines_size; ++i)
//        (*line_starts)[i] = (maximum_length - (*line_starts)[i])/2.0f;
//      (*offset)[0] = maximum_length/2.0f;
//    break;
//    case Visualizer::Alignment::RIGHT:
//      for(std::size_t i = 0u; i < lines_size; ++i)
//        (*line_starts)[i] = maximum_length - (*line_starts)[i];
//      (*offset)[0] = maximum_length;
//    break;
//    default:
//      for(std::size_t i = 0u; i < lines_size; ++i)
//        (*line_starts)[i] = 0.0f;
//      (*offset)[0] = 0.0f;
//    break;
//    }

//    switch(vertical_alignment_){
//    case Visualizer::Alignment::CENTER:
//      (*offset)[1] = (lines * line_size)/2.0f;
//    break;
//    case Visualizer::Alignment::BOTTOM:
//      (*offset)[1] = lines * line_size;
//    break;
//    default:
//      (*offset)[1] = 0.0f;
//    break;
//    }

//    std::cout << "maximum length:" << maximum_length << " - lines:" << lines * line_height_ << std::endl;
  }
}
