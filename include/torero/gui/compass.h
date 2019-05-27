#ifndef TORERO_GUI_COMPASS_H
#define TORERO_GUI_COMPASS_H

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/texture.h"
#include "torero/gl/shader.h"
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
    class Compass
    {
    public:
      Compass(torero::Core *core, torero::gl::Shader *pbr, torero::gl::Shader *text,
              const std::vector<torero::text::FontCharacter> *characters,
              const std::string &folder_path);
      ~Compass();

      void update_rotation(const float compass_angle, const float steering_angle);
      void update_pedals(const float brake, const float accelerator,
                         const float clutch, const std::string gear_position);
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

      void update_text(const float steering, const std::string gear);
      void load_buffer(torero::gl::Buffer *buffer, torero::gl::Buffer *objects,
                       const GLsizei size,
                       const std::vector<torero::gui::PBR_Shader> &object_data,
                       std::vector<torero::ComplexShaderData> *buffer_data,
                       GLsizei *data_size);
      void update_buffer(torero::gl::Buffer *buffer,
                         torero::gl::Buffer *objects,
                         const GLsizei size,
                         const std::vector<torero::gui::PBR_Shader> &object_data);

      torero::Core *core_;
      std::string folder_path_;
      bool file_exists_, is_ready_, is_loaded_;

      torero::gl::Shader *pbr_;
      GLint i_angle_, i_color_;
      GLint u_rotation_, u_translation_, u_light_, u_light_color_;
      torero::gl::Texture *t_albedo_, *t_normal_, *t_pbr_emission_;
      const GLsizei data_stride_size_, object_stride_size_;

      torero::gl::Buffer buffer_background_, buffer_background_objects_;
      GLsizei background_data_size_;
      std::vector<torero::gui::PBR_Shader> background_objects_;

      torero::gl::Buffer buffer_compass_, buffer_compass_objects_;
      GLsizei compass_data_size_;
      float previous_compass_angle_;
      std::vector<torero::gui::PBR_Shader> compass_objects_;

      torero::gl::Buffer buffer_steering_, buffer_steering_objects_;
      GLsizei steering_data_size_;
      float previous_steering_angle_;
      std::vector<torero::gui::PBR_Shader> steering_objects_;

      torero::gl::Buffer buffer_clutch_, buffer_clutch_objects_;
      GLsizei clutch_data_size_;
      float previous_cloutch_;
      const algebraica::vec3f clutch_color_;
      std::vector<torero::gui::PBR_Shader> clutch_objects_;

      torero::gl::Buffer buffer_marker_, buffer_marker_objects_;
      GLsizei marker_data_size_;
      float previous_brake_value_, previous_accelerator_value_;
      std::size_t previous_brake_, previous_accelerator_;
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
      std::string previous_gear_;

      torero::image::File im_albedo_, im_normal_, im_pbr_emission_;
      std::vector<torero::ComplexShaderData> data_background_, data_compass_, data_steering_;
      std::vector<torero::ComplexShaderData> data_clutch_, data_marker_, data_needle_;
      boost::mutex protector_;

      float previous_rotation_;
      algebraica::quaternionF rotation_;
      const algebraica::vec3f translation_;
      const algebraica::vec3f light_color_;
      algebraica::vec3f light_;
    };
  }
}
#endif // TORERO_GUI_COMPASS_H
