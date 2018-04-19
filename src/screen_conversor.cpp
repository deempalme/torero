#include "torero/screen_conversor.h"

namespace Toreo {
  ScreenConversor::ScreenConversor() :
    vehicle_frame_(nullptr),
    view_matrix_(nullptr),
    perspective_matrix_(nullptr),
    inverse_vehicle_frame_(nullptr),
    inverse_view_matrix_(nullptr),
    inverse_perspective_matrix_(nullptr)
  {

  }

  const Algebraica::vec3f ScreenConversor::to_screen(const Algebraica::vec3f position,
                                                     const Algebraica::mat4f *model_matrix){
    Algebraica::vec4f result(position, 1.0f);
    float width{FLOAT(*width_)}, height(FLOAT(*height_));
    if(model_matrix)
      result = *model_matrix * result;

    if(vehicle_frame_)
      result = *vehicle_frame_ * result;

    if(view_matrix_)
      result = *view_matrix_ * result;

    if(perspective_matrix_)
      result = *perspective_matrix_ * result;

    result /= result.w();

    return Algebraica::vec3f((width )? result[0] * width  : result[0],
                             (height)? result[1] * height : result[1],
                             result[2]);
  }

  const Algebraica::vec3f ScreenConversor::to_3d_world(const Algebraica::vec3f position){
    return position;
  }

  void ScreenConversor::set_vehicle_frame(const Algebraica::mat4f *vehicle_frame){
    vehicle_frame_ = vehicle_frame;
  }

  void ScreenConversor::set_inverse_vehicle_frame(const Algebraica::mat4f *inverse_vehicle_frame){
    inverse_vehicle_frame_ = inverse_vehicle_frame;
  }

  void ScreenConversor::set_view_matrix(const Algebraica::mat4f *view_matrix){
    view_matrix_ = view_matrix;
  }

  void ScreenConversor::set_inverse_view_matrix(const Algebraica::mat4f *inverse_view_matrix){
    inverse_view_matrix_ = inverse_view_matrix;
  }

  void ScreenConversor::set_perspective_matrix(const Algebraica::mat4f *perspective_matrix){
    perspective_matrix_ = perspective_matrix;
  }

  void ScreenConversor::set_inverse_perspective_matrix(const Algebraica::mat4f
                                                       *inverse_perspective_matrix){
    inverse_perspective_matrix_ = inverse_perspective_matrix;
  }

  void ScreenConversor::set_screen_size(const int *width, const int *height){
    if(width)
      width_ = width;

    if(height)
      height_ = height;
  }
}
