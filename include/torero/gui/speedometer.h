#ifndef TORERO_GUI_SPEEDOMETER_H
#define TORERO_GUI_SPEEDOMETER_H

#include "torero/gl/buffer.h"
#include "torero/gl/texture.h"
#include "torero/gl/shader.h"
#include "torero/definition/types.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/thread/mutex.hpp>
// Standard
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace gui {
    class Speedometer
    {
    public:
      Speedometer(torero::Core *core, torero::gl::Shader *pbr, torero::gl::Shader *text,
                  const std::vector<torero::text::FontCharacter> *characters,
                  const std::string &folder_path);
      ~Speedometer();

      void update(float speed, float rpm);
      void rotate(const float factor);

      void draw();
      void draw_emission();
      void draw_text();

      void run();
      void ready();
      bool is_ready();

    private:
      bool check_folder();
      void calculate_model(std::vector<torero::ComplexShaderData> *buffer_data,
                           const std::string subfolder);

      void update_text(const float speed, const float rpm);

      void load_buffer(torero::gl::Buffer *buffer,
                       torero::gl::Buffer *objects,
                       const GLsizei size,
                       const std::vector<torero::gui::PBR_Shader> &object_data,
                       std::vector<torero::ComplexShaderData> *buffer_data,
                       GLsizei *data_size);
      void update_buffer(torero::gl::Buffer *buffer,
                         torero::gl::Buffer *objects,
                         const GLsizei size,
                         const std::vector<torero::gui::PBR_Shader> &object_data);

      Core *core_;
      std::string folder_path_;
      bool folder_exist_, is_ready_, is_loaded_;

      torero::gl::Shader *pbr_;
      GLint i_angle_, i_color_;
      GLint u_rotation_, u_translation_, u_light_, u_light_color_;
      torero::gl::Texture *t_albedo_, *t_normal_, *t_pbr_emission_;
      const GLsizei data_stride_size_, object_stride_size_;

      torero::gl::Buffer buffer_background_, buffer_background_objects_;
      GLsizei background_data_size_;
      std::vector<torero::gui::PBR_Shader> background_objects_;

      torero::gl::Buffer buffer_marker_, buffer_marker_objects_;
      GLsizei marker_data_size_;
      std::size_t previous_speed_level_, previous_rpm_level_;
      std::vector<torero::gui::PBR_Shader> marker_objects_;

      torero::gl::Buffer buffer_needle_, buffer_needle_objects_;
      GLsizei needle_data_size_;
      std::vector<torero::gui::PBR_Shader> needle_objects_;

      torero::gl::Shader *text_;
      GLint  i_size_;
      GLint u_t_rotation_, u_t_translation_;
      torero::gl::Buffer buffer_text_;
      GLsizei text_data_size_;
      const GLsizei text_stride_size_;
      const std::vector<torero::text::FontCharacter> *characters_;

      torero::image::File im_albedo_, im_normal_, im_pbr_emission_;
      std::vector<torero::ComplexShaderData> data_background_, data_marker_, data_needle_;
      boost::mutex protector_;

      float previous_rotation_;
      algebraica::quaternionF rotation_;
      const algebraica::vec3f translation_;
      const algebraica::vec3f light_color_;
      algebraica::vec3f light_;
    };
  }
}
#endif // TORERO_GUI_SPEEDOMETER_H
