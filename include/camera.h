#ifndef TORERO_CAMERA_H
#define TORERO_CAMERA_H

#include "definitions.h"
#include "algebraica/algebraica.h"

#include <boost/function.hpp>

namespace Toreo {
  class Camera
  {
  public:
    Camera(const algebraica::vec3f position, const algebraica::vec3f target,
           const algebraica::vec3f up, algebraica::mat4f *vehicle_transformation_matrix = nullptr);

    void set_position(const float x, const float y, const float z);
    void set_target(const float x, const float y, const float z);
    void set_up(const float x, const float y, const float z);

    void set_zoom(const float zoom);
    void zooming(const bool zooming_in = true);

    void rotate_camera(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void translate_camera(const float x, const float y, const float z);
    void modify_camera(const int dx, const int dy, const bool rotate);

    void top_view();
    void isometric_view();

    void fix_view(const bool fix = true);
    void lock_camera(const bool lock = true);

    void set_resolution(const int window_width, const int window_height,
                        const int frame_width, const int frame_height);
    void update_perspective(const bool multiply = true);
    void update_view();

    void set_function_callback(boost::function<void()> callback_function);

    const algebraica::mat4f &view_matrix();
    const algebraica::mat4f &perspective_matrix();
    const algebraica::mat4f &pv_matrix();
    const algebraica::mat4f &static_pv_matrix();
    const algebraica::vec3f &camera_position();
    const algebraica::vec3f &camera_target();
    const algebraica::vec3f &camera_up();

  private:
    void multiply_matrices();
    void set_rotation(float *variable, float angle, const bool convert = true);
    void initialize(const bool restart_translation = true);

    algebraica::mat4f *vehicle_transformation_matrix_;
    algebraica::mat4f view_matrix_, perspective_matrix_, pv_matrix_, spv_matrix_;
    algebraica::mat4f camera_rotation_matrix_;
    algebraica::vec3f camera_position_, camera_target_, camera_up_;
    algebraica::vec3f last_camera_position_, last_camera_target_;
    algebraica::vec3f camera_translation_, fixed_camera_translation_;
    float pitch_, initial_min_pitch_, initial_max_pitch_, yaw_, roll_, zoom_;
    int width_, height_;
    float width_factor_, height_factor_;
    float rotational_width_factor_, rotational_height_factor_;
    float translational_width_factor_, translational_height_factor_;

    bool fixed_view_, locked_camera_;
    boost::function<void()> callback_function_;
    bool is_callback_set_;
  };
  }

#endif // TORERO_CAMERA_H
