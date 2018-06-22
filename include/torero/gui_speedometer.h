#ifndef TORERO_GUI_SPEEDOMETER_H
#define TORERO_GUI_SPEEDOMETER_H

#include "torero/buffer.h"
#include "torero/texture.h"
#include "torero/shader.h"
#include "torero/types.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/thread/mutex.hpp>
// Standard
#include <string>
#include <vector>

namespace Toreo {
  class Core;

  class GUISpeedometer
  {
  public:
    GUISpeedometer(Core *core, Shader *pbr, Shader *emission, Shader *text,
                   const std::vector<Visualizer::FontCharacter> *characters,
                   const std::string folder_path, bool *changer);
    ~GUISpeedometer();

    void update(float speed, float rpm);
    void rotate(const float factor);

    void draw();
    void draw_emission();
    void draw_text();

    void run();
    void ready();
    const bool is_ready();

  private:
    bool check_folder();
    void calculate_model(std::vector<Visualizer::ComplexShaderData> *buffer_data,
                         const std::string subfolder);

    void update_text(const float speed, const float rpm);

    void load_buffer(Buffer *buffer, Buffer *objects, const GLsizei size,
                     const std::vector<Visualizer::PBR_GUIShader> &object_data,
                     std::vector<Visualizer::ComplexShaderData> *buffer_data,
                     GLsizei *data_size);
    void update_buffer(Buffer *buffer, Buffer *objects, const GLsizei size,
                       const std::vector<Visualizer::PBR_GUIShader> &object_data);

    Core *core_;
    std::string folder_path_;
    bool *has_changed_, folder_exist_, is_ready_, is_loaded_;

    Shader *pbr_, *emission_;
    GLint i_position_, i_uv_, i_normal_, i_tangent_, i_bitangent_, i_angle_, i_color_;
    GLint u_rotation_, u_translation_, u_emitting_, u_light_, u_light_color_;
    Texture *t_albedo_, *t_normal_, *t_metallic_, *t_roughness_, *t_ao_, *t_emission_;
    const GLsizei data_stride_size_, object_stride_size_;

    Buffer buffer_background_, buffer_background_objects_;
    GLsizei background_data_size_;
    std::vector<Visualizer::PBR_GUIShader> background_objects_;

    Buffer buffer_marker_, buffer_marker_objects_;
    GLsizei marker_data_size_;
    std::size_t previous_speed_level_, previous_rpm_level_;
    std::vector<Visualizer::PBR_GUIShader> marker_objects_;

    Buffer buffer_needle_, buffer_needle_objects_;
    GLsizei needle_data_size_;
    std::vector<Visualizer::PBR_GUIShader> needle_objects_;

    Shader *text_;
    GLint  i_size_;
    GLint u_t_rotation_, u_t_translation_;
    Buffer buffer_text_;
    GLsizei text_data_size_;
    const GLsizei text_stride_size_;
    const std::vector<Visualizer::FontCharacter> *characters_;

    Visualizer::ImageFile im_albedo_, im_normal_, im_metallic_, im_roughness_, im_ao_, im_emission_;
    std::vector<Visualizer::ComplexShaderData> data_background_, data_marker_, data_needle_;
    boost::mutex protector_;

    float previous_rotation_;
    Algebraica::quaternionF rotation_;
    const Algebraica::vec3f translation_;
    const Algebraica::vec3f light_color_;
    Algebraica::vec3f light_;
  };
}

#endif // TORERO_GUI_SPEEDOMETER_H
