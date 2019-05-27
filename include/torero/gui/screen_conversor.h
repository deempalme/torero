#ifndef TORERO_SCREEN_CONVERSOR_H
#define TORERO_SCREEN_CONVERSOR_H

#include "torero/definition/types.h"

#include "algebraica/algebraica.h"

namespace torero {
  namespace gui {
    class ScreenConversor
    {
    public:
      ScreenConversor();

      const algebraica::vec3f to_screen(const algebraica::vec3f position,
                                        const algebraica::mat4f *model_matrix = nullptr);

      const algebraica::vec3f to_3d_world(const algebraica::vec3f position);

      void set_vehicle_frame(const algebraica::mat4f *vehicle_frame = nullptr);
      void set_inverse_vehicle_frame(const algebraica::mat4f *inverse_vehicle_frame = nullptr);

      void set_view_matrix(const algebraica::mat4f *view_matrix = nullptr);
      void set_inverse_view_matrix(const algebraica::mat4f *inverse_view_matrix = nullptr);

      void set_perspective_matrix(const algebraica::mat4f *perspective_matrix = nullptr);
      void set_inverse_perspective_matrix(const algebraica::mat4f
                                          *inverse_perspective_matrix = nullptr);

      void set_screen_size(const int *width = nullptr, const int *height = nullptr);

    private:
      const algebraica::mat4f *vehicle_frame_;
      const algebraica::mat4f *view_matrix_;
      const algebraica::mat4f *perspective_matrix_;

      const algebraica::mat4f *inverse_vehicle_frame_;
      const algebraica::mat4f *inverse_view_matrix_;
      const algebraica::mat4f *inverse_perspective_matrix_;

      const int *width_;
      const int *height_;
    };
  }
}
#endif // TORERO_SCREEN_CONVERSOR_H
