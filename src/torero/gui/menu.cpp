#include "torero/core.h"
#include "torero/camera/view.h"
#include "torero/event/handler.h"
#include "torero/gui/menu.h"

namespace torero {
  namespace gui {
    Menu::Menu(torero::Core *core, torero::gl::Shader *color_shader, torero::gl::Shader *id_shader) :
      core_(core),
      color_shader_(color_shader),
      id_shader_(id_shader),
      buffer_(true),
      type_size_(sizeof(torero::gui::ButtonShader)),
      color_u_offset_(color_shader_->uniform_location("u_offset")),
      color_u_alignment_(color_shader_->uniform_location("u_alignment")),
      id_u_offset_(id_shader_->uniform_location("u_offset")),
      id_u_alignment_(id_shader_->uniform_location("u_alignment")),
      i_position_(0),
      i_texture_(1),
      i_size_(2),
      i_id_(3),
      i_element_(4),
      menu_offset_(0.0f, 8.0f),
      menu_alignment_(0.0f, 0.0f),
      buttons_(5)
    {
      torero::gui::button::Button button;

      button.left = 0.0f;
      button.top = 0.0f;
      button.u = 8.0f;
      button.v = 1954.0f;
      button.id = static_cast<float>(torero::gui::menu::ZoomIn);
      buttons_[0] = button;

      button.left = 94.0f;
      button.v = 1859.0f;
      button.id = static_cast<float>(torero::gui::menu::IsometricView);
      buttons_[1] = button;

      button.left = 188.0f;
      button.v = 1764.0f;
      button.id = static_cast<float>(torero::gui::menu::TopView);
      buttons_[2] = button;

      button.left = 282.0f;
      button.v = 1668.0f;
      button.id = static_cast<float>(torero::gui::menu::ZoomOut);
      buttons_[3] = button;

      button.left = 0.0f;
      button.v = 1553.0f;
      button.id = static_cast<float>(torero::gui::menu::PEM_Logo);
      buttons_[4] = button;

      set_buffer();
    }

    void Menu::change_button_state(const torero::gui::menu::ButtonType button_id,
                                   const torero::gui::button::State state){
      const std::size_t id{static_cast<std::size_t>(button_id)};
      const float width{k_menu_button_width}, height{k_menu_button_height};

      if(buttons_[id].state != state){
        buttons_[id].state = state;

        const float plus{static_cast<float>(state) * width};

        const float top{buttons_[id].top}, top_plus{height + top};
        const float left{buttons_[id].left}, left_plus{width + left};
        const float u{buttons_[id].u + plus}, u_plus{width + u};
        const float v{buttons_[id].v}, v_plus{height + v};
        const float i{buttons_[id].id}, p{static_cast<float>(torero::gui::object::Menu)};

        const torero::gui::ButtonShader button[6] = {
          { top_plus, left_plus, u_plus,      v, width, height, i, p }, // bottom-right corner
          { top_plus, left_plus, u_plus,      v, width, height, i, p }, // bottom-right corner
          { top_plus,      left,      u,      v, width, height, i, p }, // bottom-left corner
          {      top, left_plus, u_plus, v_plus, width, height, i, p }, // top-right corner
          {      top,      left,      u, v_plus, width, height, i, p }, // top-left corner
          {      top,      left,      u, v_plus, width, height, i, p }  // top-left corner
        };

        buffer_.vertex_bind();
        buffer_.allocate_section_array(button, static_cast<GLint>(id) * 6
                                       * static_cast<GLint>(type_size_), 6 * type_size_);

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

        core_->reload_screen();
        if(state == torero::gui::button::Click)
          core_->camera().blocked(true);
        else
          core_->camera().blocked(false);

        if(state == torero::gui::button::Hover || state == torero::gui::button::Click)
          core_->change_cursor(torero::gui::Cursor::Hand);
        else
          core_->change_cursor(torero::gui::Cursor::Normal);
      }
    }

    void Menu::restart_buttons(){
      for(torero::gui::button::Button &button : buttons_)
        if(button.state != torero::gui::button::Normal)
          change_button_state(static_cast<torero::gui::menu::ButtonType>(button.id),
                              torero::gui::button::Normal);
    }

    void Menu::click_event(const torero::gui::menu::ButtonType button_id){
      switch(button_id){
        case torero::gui::menu::ZoomIn:
          core_->camera().zoom(torero::camera::Zoom::In);
        break;
        case torero::gui::menu::ZoomOut:
          core_->camera().zoom(torero::camera::Zoom::Out);
        break;
        case torero::gui::menu::IsometricView:
          core_->camera().isometric_view();
        break;
        case torero::gui::menu::TopView:
          core_->camera().top_view();
        break;
        default: break;
      }
    }

    void Menu::set_offset(const int top, const int left){
      menu_offset_(static_cast<float>(top), static_cast<float>(left));
    }

    void Menu::set_alignment(const float top, const float left){
      menu_alignment_(top, left);
    }

    void Menu::draw(){
      color_shader_->set_value(color_u_offset_, menu_offset_);
      color_shader_->set_value(color_u_alignment_, menu_alignment_);

      buffer_.vertex_bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);

      color_shader_->set_value(color_u_alignment_, algebraica::vec2f(20.0f, 12.0f));
      glDrawArrays(GL_TRIANGLE_STRIP, 24, 30);
      buffer_.vertex_release();
    }

    void Menu::draw_id(){
      id_shader_->set_value(id_u_offset_, menu_offset_);
      id_shader_->set_value(id_u_alignment_, menu_alignment_);

      buffer_.vertex_bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
      buffer_.vertex_release();
    }

    void Menu::set_buffer(){
      const float width{k_menu_button_width}, height{k_menu_button_height};
      const float p{static_cast<float>(torero::gui::object::Menu)};

      const float top_plus[5] = { height + buttons_[0].top,
                                  height + buttons_[1].top,
                                  height + buttons_[2].top,
                                  height + buttons_[3].top,
                                  90.0f + buttons_[4].top };

      const float left_plus[5] = { width + buttons_[0].left,
                                   width + buttons_[1].left,
                                   width + buttons_[2].left,
                                   width + buttons_[3].left,
                                   90.0f + buttons_[4].left };

      const float u_plus[5] = { width + buttons_[0].u,
                                width + buttons_[1].u,
                                width + buttons_[2].u,
                                width + buttons_[3].u,
                                90.0f + buttons_[4].u };

      const float v_plus[5] = { height + buttons_[0].v,
                                height + buttons_[1].v,
                                height + buttons_[2].v,
                                height + buttons_[3].v,
                                90.0f + buttons_[4].v };

      const torero::gui::ButtonShader buttons[30] = {
        // Zoom in button:
        {     top_plus[0],     left_plus[0],     u_plus[0], buttons_[0].v, width, height, buttons_[0].id, p }, // bottom-right corner
        {     top_plus[0],     left_plus[0],     u_plus[0], buttons_[0].v, width, height, buttons_[0].id, p }, // bottom-right corner
        {     top_plus[0], buttons_[0].left, buttons_[0].u, buttons_[0].v, width, height, buttons_[0].id, p }, // bottom-left corner
        { buttons_[0].top,     left_plus[0],     u_plus[0],     v_plus[0], width, height, buttons_[0].id, p }, // top-right corner
        { buttons_[0].top, buttons_[0].left, buttons_[0].u,     v_plus[0], width, height, buttons_[0].id, p }, // top-left corner
        { buttons_[0].top, buttons_[0].left, buttons_[0].u,     v_plus[0], width, height, buttons_[0].id, p }, // top-left corner
        // Isometric view button:
        {     top_plus[1],     left_plus[1],     u_plus[1], buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-right corner
        {     top_plus[1],     left_plus[1],     u_plus[1], buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-right corner
        {     top_plus[1], buttons_[1].left, buttons_[1].u, buttons_[1].v, width, height, buttons_[1].id, p }, // bottom-left corner
        { buttons_[1].top,     left_plus[1],     u_plus[1],     v_plus[1], width, height, buttons_[1].id, p }, // top-right corner
        { buttons_[1].top, buttons_[1].left, buttons_[1].u,     v_plus[1], width, height, buttons_[1].id, p }, // top-left corner
        { buttons_[1].top, buttons_[1].left, buttons_[1].u,     v_plus[1], width, height, buttons_[1].id, p }, // top-left corner
        // Top view button:
        {     top_plus[2],     left_plus[2],     u_plus[2], buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-right corner
        {     top_plus[2],     left_plus[2],     u_plus[2], buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-right corner
        {     top_plus[2], buttons_[2].left, buttons_[2].u, buttons_[2].v, width, height, buttons_[2].id, p }, // bottom-left corner
        { buttons_[2].top,     left_plus[2],     u_plus[2],     v_plus[2], width, height, buttons_[2].id, p }, // top-right corner
        { buttons_[2].top, buttons_[2].left, buttons_[2].u,     v_plus[2], width, height, buttons_[2].id, p }, // top-left corner
        { buttons_[2].top, buttons_[2].left, buttons_[2].u,     v_plus[2], width, height, buttons_[2].id, p }, // top-left corner
        // Zoom out button:
        {     top_plus[3],     left_plus[3],     u_plus[3], buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-right corner
        {     top_plus[3],     left_plus[3],     u_plus[3], buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-right corner
        {     top_plus[3], buttons_[3].left, buttons_[3].u, buttons_[3].v, width, height, buttons_[3].id, p }, // bottom-left corner
        { buttons_[3].top,     left_plus[3],     u_plus[3],     v_plus[3], width, height, buttons_[3].id, p }, // top-right corner
        { buttons_[3].top, buttons_[3].left, buttons_[3].u,     v_plus[3], width, height, buttons_[3].id, p }, // top-left corner
        { buttons_[3].top, buttons_[3].left, buttons_[3].u,     v_plus[3], width, height, buttons_[3].id, p }, // top-left corner
        // PEM Logotype:
        {     top_plus[4],     left_plus[4],     u_plus[4], buttons_[4].v, 90.0f, 90.0f, 0.0f, 0.0f }, // bottom-right corner
        {     top_plus[4],     left_plus[4],     u_plus[4], buttons_[4].v, 90.0f, 90.0f, 0.0f, 0.0f }, // bottom-right corner
        {     top_plus[4], buttons_[4].left, buttons_[4].u, buttons_[4].v, 90.0f, 90.0f, 0.0f, 0.0f }, // bottom-left corner
        { buttons_[4].top,     left_plus[4],     u_plus[4],     v_plus[4], 90.0f, 90.0f, 0.0f, 0.0f }, // top-right corner
        { buttons_[4].top, buttons_[4].left, buttons_[4].u,     v_plus[4], 90.0f, 90.0f, 0.0f, 0.0f }, // top-left corner
        { buttons_[4].top, buttons_[4].left, buttons_[4].u,     v_plus[4], 90.0f, 90.0f, 0.0f, 0.0f }  // top-left corner
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
}
