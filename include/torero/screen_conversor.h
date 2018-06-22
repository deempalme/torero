#ifndef TORERO_SCREEN_CONVERSOR_H
#define TORERO_SCREEN_CONVERSOR_H

#include "torero/definitions.h"
#include "torero/types.h"

#include "algebraica/algebraica.h"

namespace Toreo {
  class ScreenConversor
  {
  public:
    ScreenConversor();

    const Algebraica::vec3f to_screen(const Algebraica::vec3f position,
                                      const Algebraica::mat4f *model_matrix = nullptr);

    const Algebraica::vec3f to_3d_world(const Algebraica::vec3f position);

    void set_vehicle_frame(const Algebraica::mat4f *vehicle_frame = nullptr);
    void set_inverse_vehicle_frame(const Algebraica::mat4f *inverse_vehicle_frame = nullptr);

    void set_view_matrix(const Algebraica::mat4f *view_matrix = nullptr);
    void set_inverse_view_matrix(const Algebraica::mat4f *inverse_view_matrix = nullptr);

    void set_perspective_matrix(const Algebraica::mat4f *perspective_matrix = nullptr);
    void set_inverse_perspective_matrix(const Algebraica::mat4f
                                        *inverse_perspective_matrix = nullptr);

    void set_screen_size(const int *width = nullptr, const int *height = nullptr);

  private:
    const Algebraica::mat4f *vehicle_frame_;
    const Algebraica::mat4f *view_matrix_;
    const Algebraica::mat4f *perspective_matrix_;

    const Algebraica::mat4f *inverse_vehicle_frame_;
    const Algebraica::mat4f *inverse_view_matrix_;
    const Algebraica::mat4f *inverse_perspective_matrix_;

    const int *width_;
    const int *height_;
  };
}

#endif // TORERO_SCREEN_CONVERSOR_H
