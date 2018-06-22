#include "torero/gui_title_bar.h"
#include "torero/core.h"

namespace Toreo {
  GUITitleBar::GUITitleBar(Core *core, Shader *color_shader,
                           Shader *id_shader, bool *screen_changer, bool *blocker) :
    core_(core),
    window_width_(core->width()),
    color_shader_(color_shader),
    id_shader_(id_shader),
    has_changed_(screen_changer),
    blocked_(blocker),
    window_mover_(core->mover()),
    buffer_(true),
    type_size_(sizeof(Visualizer::ButtonShader)),
    color_u_offset_(color_shader_->uniform_location("u_offset")),
    color_u_alignment_(color_shader_->uniform_location("u_alignment")),
    id_u_offset_(id_shader_->uniform_location("u_offset")),
    id_u_alignment_(id_shader_->uniform_location("u_alignment")),
    i_position_(0),
    i_texture_(1),
    i_size_(2),
    i_id_(3),
    i_element_(4),
    zero_(),
    buttons_(5)
  {
    Visualizer::Button::Button button;

    button.left = 30.0f;
    button.top = 0.0f;
    button.u = 8.0f;
    button.v = 1515.0f;
    button.width = -120.0f;
    button.height = TITLE_HEIGHT;
    button.id = static_cast<float>(Visualizer::Title::TITLE);
    buttons_[0] = button;

    button.left = -0.001f;
    button.top = 0.0f;
    button.u = 8.0f;
    button.v = 1485.0f;
    button.width = TITLE_BUTTON_WIDTH;
    button.id = static_cast<float>(Visualizer::Title::CLOSE);
    buttons_[1] = button;

    button.left = -30.0f;
    button.v = 1455.0f;
    button.id = static_cast<float>(Visualizer::Title::MAXIMIZE);
    buttons_[2] = button;

    button.left = -60.0f;
    button.v = 1425.0f;
    button.id = static_cast<float>(Visualizer::Title::MINIMIZE);
    buttons_[3] = button;

    button.left = 0.0f;
    button.v = 1395.0f;
    button.id = static_cast<float>(Visualizer::Title::OPTIONS);
    buttons_[4] = button;

    set_buffer();
  }

  void GUITitleBar::change_button_state(const Visualizer::Title::ButtonType button_id,
                                        const Visualizer::Button::State state){
    const std::size_t id{static_cast<std::size_t>(button_id)};

    if(buttons_[id].state != state){
      buttons_[id].state = state;

      const float width{buttons_[id].width}, height{buttons_[id].height};
      const float width2 = (width <= 0.0f)? *window_width_ + width : width;
      const float plus{static_cast<float>(state) * TITLE_BUTTON_WIDTH};

      const float top{buttons_[id].top}, top_plus{height + top};
      const float left = (buttons_[id].left < 0.0f)? -width2 + buttons_[id].left : buttons_[id].left;
      const float left_plus = (buttons_[id].left < 0.0f)? buttons_[id].left : width2 + left;
      const float u{buttons_[id].u + plus}, u_plus{u + TITLE_BUTTON_WIDTH};
      const float v{buttons_[id].v}, v_plus{height + v};
      const float i{buttons_[id].id}, p{static_cast<float>(Visualizer::GUIid::TITLE_BAR)};

      const Visualizer::ButtonShader button[6] = {
        {      top,      left,      u, v_plus, width, height, i, p }, // top-left corner
        {      top,      left,      u, v_plus, width, height, i, p }, // top-left corner
        { top_plus,      left,      u,      v, width, height, i, p }, // bottom-left corner
        {      top, left_plus, u_plus, v_plus, width, height, i, p }, // top-right corner
        { top_plus, left_plus, u_plus,      v, width, height, i, p }, // bottom-right corner
        { top_plus, left_plus, u_plus,      v, width, height, i, p }  // bottom-right corner
      };

      buffer_.vertex_bind();
      buffer_.allocate_range_array(button,
                                   static_cast<GLint>(id) * 6 * static_cast<GLint>(type_size_),
                                   6 * type_size_);

      GLint offset{0};
      buffer_.enable(i_position_);
      buffer_.attributte_buffer(i_position_, _2D, offset, type_size_);

      offset += 2 * sizeof(float);
      buffer_.enable(i_texture_);
      buffer_.attributte_buffer(i_texture_, _2D, offset, type_size_);

      offset += 2 * sizeof(float);
      buffer_.enable(i_size_);
      buffer_.attributte_buffer(i_size_, _2D, offset, type_size_);

      offset += 2 * sizeof(float);
      buffer_.enable(i_id_);
      buffer_.attributte_buffer(i_id_, _1D, offset, type_size_);

      offset += sizeof(float);
      buffer_.enable(i_element_);
      buffer_.attributte_buffer(i_element_, _1D, offset, type_size_);

      buffer_.vertex_release();

      *has_changed_ = true;
      if(state == Visualizer::Button::CLICK)
        *blocked_ = true;
      else
        *blocked_ = false;

      if(state == Visualizer::Button::HOVER || state == Visualizer::Button::CLICK)
        if(button_id == Visualizer::Title::TITLE)
          core_->set_cursor(Visualizer::Cursor::MOVE);
        else
          core_->set_cursor(Visualizer::Cursor::HAND);
      else
        core_->set_cursor(Visualizer::Cursor::NORMAL);
    }
  }

  void GUITitleBar::restart_buttons(){
    for(Visualizer::Button::Button &button : buttons_)
      if(button.state != Visualizer::Button::NORMAL)
        change_button_state(static_cast<Visualizer::Title::ButtonType>(button.id),
                            Visualizer::Button::NORMAL);
  }

  void GUITitleBar::click_event(const Visualizer::Title::ButtonType button_id,
                                const Visualizer::Mouse::Event event){
    if(event == Visualizer::Mouse::CLICK)
      if(button_id == Visualizer::Title::TITLE)
        *window_mover_ = true;
    else
      switch(button_id){
      case Visualizer::Title::CLOSE:
        core_->close();
      break;
      case Visualizer::Title::MAXIMIZE:
        core_->maximize();
      break;
      case Visualizer::Title::MINIMIZE:
        core_->minimize();
      break;
      case Visualizer::Title::OPTIONS:
        core_->camera_view_top();
      break;
      }
  }

  void GUITitleBar::draw(){
    color_shader_->set_value(color_u_offset_, zero_);
    color_shader_->set_value(color_u_alignment_, zero_);

    buffer_.vertex_bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 30);
    buffer_.vertex_release();
  }

  void GUITitleBar::draw_id(){
    id_shader_->set_value(id_u_offset_, zero_);
    id_shader_->set_value(id_u_alignment_, zero_);

    buffer_.vertex_bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 30);
    buffer_.vertex_release();
  }

  void GUITitleBar::set_buffer(){
    const float width{TITLE_BUTTON_WIDTH}, height{TITLE_HEIGHT};
    const float p{static_cast<float>(Visualizer::GUIid::TITLE_BAR)};

    const float top_plus[5] = { height + buttons_[0].top,
                                height + buttons_[1].top,
                                height + buttons_[2].top,
                                height + buttons_[3].top,
                                height + buttons_[4].top };

    const float left_plus[5] = {*window_width_ + buttons_[0].width + buttons_[0].left,
                                -width + buttons_[1].left,
                                -width + buttons_[2].left,
                                -width + buttons_[3].left,
                                 width + buttons_[4].left };

    const float u_plus[5] = { width + buttons_[0].u,
                              width + buttons_[1].u,
                              width + buttons_[2].u,
                              width + buttons_[3].u,
                              width + buttons_[4].u };

    const float v_plus[5] = { height + buttons_[0].v,
                              height + buttons_[1].v,
                              height + buttons_[2].v,
                              height + buttons_[3].v,
                              height + buttons_[4].v };

    const Visualizer::ButtonShader buttons[30] = {
      // Title button:
      { buttons_[0].top, buttons_[0].left, buttons_[0].u,     v_plus[0], width, height, buttons_[0].id, p }, // top-left corner
      { buttons_[0].top, buttons_[0].left, buttons_[0].u,     v_plus[0], width, height, buttons_[0].id, p }, // top-left corner
      {     top_plus[0], buttons_[0].left, buttons_[0].u, buttons_[0].v, width, height, buttons_[0].id, p }, // bottom-left corner
      { buttons_[0].top,     left_plus[0],     u_plus[0],     v_plus[0], width, height, buttons_[0].id, p }, // top-right corner
      {     top_plus[0],     left_plus[0],     u_plus[0], buttons_[0].v, width, height, buttons_[0].id, p }, // bottom-right corner
      {     top_plus[0],     left_plus[0],     u_plus[0], buttons_[0].v, width, height, buttons_[0].id, p },  // bottom-right corner
      // Close view button:
      { buttons_[1].top,     left_plus[1], buttons_[1].u,     v_plus[1], width, height, buttons_[1].id, p }, // top-left corner
      { buttons_[1].top,     left_plus[1], buttons_[1].u,     v_plus[1], width, height, buttons_[1].id, p }, // top-left corner
      {     top_plus[1],     left_plus[1], buttons_[1].u, buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-left corner
      { buttons_[1].top, buttons_[1].left,     u_plus[1],     v_plus[1], width, height, buttons_[1].id, p }, // top-right corner
      {     top_plus[1], buttons_[1].left,     u_plus[1], buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-right corner
      {     top_plus[1], buttons_[1].left,     u_plus[1], buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-right corner
      // Maximize button:
      { buttons_[2].top,     left_plus[2], buttons_[2].u,     v_plus[2], width, height, buttons_[2].id, p }, // top-left corner
      { buttons_[2].top,     left_plus[2], buttons_[2].u,     v_plus[2], width, height, buttons_[2].id, p }, // top-left corner
      {     top_plus[2],     left_plus[2], buttons_[2].u, buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-left corner
      { buttons_[2].top, buttons_[2].left,     u_plus[2],     v_plus[2], width, height, buttons_[2].id, p }, // top-right corner
      {     top_plus[2], buttons_[2].left,     u_plus[2], buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-right corner
      {     top_plus[2], buttons_[2].left,     u_plus[2], buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-right corner
      // Minimize button:
      { buttons_[3].top,     left_plus[3], buttons_[3].u,     v_plus[3], width, height, buttons_[3].id, p }, // top-left corner
      { buttons_[3].top,     left_plus[3], buttons_[3].u,     v_plus[3], width, height, buttons_[3].id, p }, // top-left corner
      {     top_plus[3],     left_plus[3], buttons_[3].u, buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-left corner
      { buttons_[3].top, buttons_[3].left,     u_plus[3],     v_plus[3], width, height, buttons_[3].id, p }, // top-right corner
      {     top_plus[3], buttons_[3].left,     u_plus[3], buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-right corner
      {     top_plus[3], buttons_[3].left,     u_plus[3], buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-right corner
      // Options button:
      { buttons_[4].top, buttons_[4].left, buttons_[4].u,     v_plus[4], width, height, buttons_[4].id, p }, // top-left corner
      { buttons_[4].top, buttons_[4].left, buttons_[4].u,     v_plus[4], width, height, buttons_[4].id, p }, // top-left corner
      {     top_plus[4], buttons_[4].left, buttons_[4].u, buttons_[4].v, width, height, buttons_[4].id, p }, // bottom-left corner
      { buttons_[4].top,     left_plus[4],     u_plus[4],     v_plus[4], width, height, buttons_[4].id, p }, // top-right corner
      {     top_plus[4],     left_plus[4],     u_plus[4], buttons_[4].v, width, height, buttons_[4].id, p }, // bottom-right corner
      {     top_plus[4],     left_plus[4],     u_plus[4], buttons_[4].v, width, height, buttons_[4].id, p }  // bottom-right corner
    };

    buffer_.vertex_bind();
    buffer_.allocate_array(buttons, 30 * type_size_, GL_DYNAMIC_DRAW);

    GLint offset{0};
    buffer_.enable(i_position_);
    buffer_.attributte_buffer(i_position_, _2D, offset, type_size_);

    offset += 2 * sizeof(float);
    buffer_.enable(i_texture_);
    buffer_.attributte_buffer(i_texture_, _2D, offset, type_size_);

    offset += 2 * sizeof(float);
    buffer_.enable(i_size_);
    buffer_.attributte_buffer(i_size_, _2D, offset, type_size_);

    offset += 2 * sizeof(float);
    buffer_.enable(i_id_);
    buffer_.attributte_buffer(i_id_, _1D, offset, type_size_);

    offset += sizeof(float);
    buffer_.enable(i_element_);
    buffer_.attributte_buffer(i_element_, _1D, offset, type_size_);

    buffer_.vertex_release();
  }
}
