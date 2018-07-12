#ifndef TORERO_GUI_COMPASS_H
#define TORERO_GUI_COMPASS_H

#include "torero/buffer.h"
#include "torero/texture.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// Boost
#include <boost/thread/mutex.hpp>
// Standard
#include <string>
#include <vector>

namespace torero {
  class Core;

  class GUICompass
  {
  public:
    GUICompass(Core *core, Shader *pbr, Shader *emission, Shader *text,
               const std::vector<torero::FontCharacter> *characters,
               const std::string folder_path, bool *changer);
    ~GUICompass();

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
    void load_buffer(Buffer *buffer, Buffer *objects, const GLsizei size,
                     const std::vector<torero::PBR_GUIShader> &object_data,
                     std::vector<torero::ComplexShaderData> *buffer_data,
                     GLsizei *data_size);
    void update_buffer(Buffer *buffer, Buffer *objects, const GLsizei size,
                       const std::vector<torero::PBR_GUIShader> &object_data);

    Core *core_;
    std::string folder_path_;
    bool *has_changed_, file_exists_, is_ready_, is_loaded_;

    Shader *pbr_, *emission_;
    GLint i_position_, i_uv_, i_normal_, i_tangent_, i_bitangent_, i_angle_, i_color_;
    GLint u_rotation_, u_translation_, u_emitting_, u_light_, u_light_color_;
    Texture *t_albedo_, *t_normal_, *t_metallic_, *t_roughness_, *t_ao_, *t_emission_;
    const GLsizei data_stride_size_, object_stride_size_;

    Buffer buffer_background_, buffer_background_objects_;
    GLsizei background_data_size_;
    std::vector<torero::PBR_GUIShader> background_objects_;

    Buffer buffer_compass_, buffer_compass_objects_;
    GLsizei compass_data_size_;
    float previous_compass_angle_;
    std::vector<torero::PBR_GUIShader> compass_objects_;

    Buffer buffer_steering_, buffer_steering_objects_;
    GLsizei steering_data_size_;
    float previous_steering_angle_;
    std::vector<torero::PBR_GUIShader> steering_objects_;

    Buffer buffer_clutch_, buffer_clutch_objects_;
    GLsizei clutch_data_size_;
    float previous_cloutch_;
    const algebraica::vec3f clutch_color_;
    std::vector<torero::PBR_GUIShader> clutch_objects_;

    Buffer buffer_marker_, buffer_marker_objects_;
    GLsizei marker_data_size_;
    float previous_brake_value_, previous_accelerator_value_;
    std::size_t previous_brake_, previous_accelerator_;
    std::vector<torero::PBR_GUIShader> marker_objects_;

    Buffer buffer_needle_, buffer_needle_objects_;
    GLsizei needle_data_size_;
    std::vector<torero::PBR_GUIShader> needle_objects_;

    Shader *text_;
    GLint  i_size_;
    GLint u_t_rotation_, u_t_translation_;
    Buffer buffer_text_;
    GLsizei text_data_size_;
    const GLsizei text_stride_size_;
    const std::vector<torero::FontCharacter> *characters_;
    std::string previous_gear_;

    torero::ImageFile im_albedo_, im_normal_, im_metallic_, im_roughness_, im_ao_, im_emission_;
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

#endif // TORERO_GUI_COMPASS_H
