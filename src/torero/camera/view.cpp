#include "torero/camera/view.h"

namespace torero {
  namespace camera {
    View::View(const algebraica::vec3f position,
               const algebraica::vec3f target,
               const algebraica::vec3f up,
               const torero::Pose3D *vehicle_frame) :
      vehicle_(vehicle_frame),
      view_matrix_(),
      perspective_matrix_(),
      pv_matrix_(),
      spv_matrix_(),
      camera_position_(position.x, position.y, position.z),
      camera_target_(target.x, target.y, target.z),
      camera_up_(up.x, up.y, up.z),
      axis_z_(0.0f, 0.0f, 1.0f),
      orientation_(),
      rotation_(),
      properties_{torero::k_default_width, torero::k_default_height,
                  torero::k_default_width, torero::k_default_height},
      rotational_factor_(0.2f),
      translational_factor_(0.02f),
      uniform_buffer_(false),
      matrix_size_(sizeof(algebraica::mat4f)),
      vector_size_(sizeof(algebraica::vec3f)),
      buffer_size_(matrix_size_ * 3 + vector_size_),
      offset_projection_(0),
      offset_view_(matrix_size_),
      offset_pv_(offset_view_ + matrix_size_),
      offset_spv_(offset_pv_ + matrix_size_),
      offset_position_(offset_spv_ + matrix_size_),
      fixed_view_(false),
      locked_camera_(true),
      blocked_camera_(false),
      is_callback_set_(false)
    {
    }

    bool View::blocked() const{
      return blocked_camera_;
    }

    void View::blocked(const bool block){
      blocked_camera_ = block;
    }

    void View::fix_view(const bool fix){
      fixed_view_ = fix;
    }

    void View::isometric_view(){
      camera_position_(-12.0f, 0.0f, 3.5f);
      camera_target_();
      camera_up_(0.0f, 0.0f, 1.0f);
      //    camera_up_(0.292372f, 0.0f, 0.956305f);
      rotation_();
      orientation_();
      calculate_factors();
      update(true);
    }

    void View::key_to_camera(const int x, const int y, const int z){
      if(x == 0 && y == 0 && z == 0) return;

      camera_target_ +=
          algebraica::vec3f(to_float(x), to_float(y), to_float(z)) * k_moving_distance;
      update(true);
    }

    void View::lock_camera(const bool lock){
      locked_camera_ = lock;
    }

    void View::mouse_to_camera(const int dx, const int dy, const bool rotate){
      if(dx == 0 && dy == 0)
        return;

      const float x{to_float(dx)}, y{to_float(dy)};

      if(rotate){
        if(dx != 0)
          rotation_ = algebraica::quaternionF::from_axis_and_angle(axis_z_,
                                                                   -x * rotational_factor_);

        if(dx == 0 && dy != 0){
          rotation_ = algebraica::quaternionF::from_axis_and_angle(0.0f, 1.0f, 0.0f,
                                                                   y * rotational_factor_);
          axis_z_ = rotation_ * axis_z_;
        }else if(dy != 0){
          const algebraica::quaternionF rotation_y =
              algebraica::quaternionF::from_axis_and_angle(0.0f, 1.0f, 0.0f,
                                                           y * rotational_factor_);
          axis_z_ = rotation_y * axis_z_;
          rotation_ *= rotation_y;
        }
        camera_position_ = rotation_ * camera_position_;
        camera_up_ = rotation_ * camera_up_;
        orientation_ = orientation_ * rotation_;
      }else{
        camera_target_ += orientation_ *
                          algebraica::vec3f(0.0f, x * translational_factor_,
                                            y * translational_factor_);
      }
      update(true);
    }

    const algebraica::mat4f &View::perspective_matrix() const{
      return perspective_matrix_;
    }

    const algebraica::mat4f &View::perspective_view_matrix() const{
      return pv_matrix_;
    }

    const algebraica::vec3f &View::position() const{
      return camera_position_;
    }

    void View::position(const float x, const float y, const float z){
      camera_position_(x, y, z);
      calculate_factors();
      update(true);
    }

    const camera::Resolution &View::resolution() const{
      return properties_;
    }

    void View::resolution(const int window_width, const int window_height,
                          const int frame_width, const int frame_height){
      properties_.window_height = window_height;
      properties_.window_width = window_width;
      properties_.frame_height = frame_height;
      properties_.frame_width = frame_width;
      properties_.relation = to_float(frame_width) / to_float(window_width);

      calculate_factors();
      update_perspective(true);
    }

    void View::rotate(const algebraica::quaternionF &rotation){
      camera_position_ = rotation * camera_position_;
      camera_up_ = rotation * camera_up_;
      update(true);
    }

    void View::set_function_callback(boost::function<void ()> callback_function){
      is_callback_set_ = true;
      callback_function_ = callback_function;

      uniform_buffer_.create();
      uniform_buffer_.bind();
      uniform_buffer_.allocate(nullptr, buffer_size_, GL_DYNAMIC_DRAW);
      uniform_buffer_.bind_base(torero::BufferLocation::Scene);
      uniform_buffer_.release();

      update_perspective();
      update(true);
    }

    const algebraica::mat4f &View::static_perspective_view_matrix() const{
      return spv_matrix_;
    }

    const algebraica::vec3f &View::target() const{
      return camera_target_;
    }

    void View::target(const float x, const float y, const float z){
      camera_target_(x, y, z);
      update(true);
    }

    void View::to_identity(){
      view_matrix_.to_identity();
      multiply_matrices();
    }

    void View::top_view(){
      camera_position_(0.0f, 0.0f, 45.0f);
      camera_target_();
      camera_up_(1.0f, 0.0f, 0.0f);
      rotation_();
      orientation_();
      calculate_factors();
      update(true);
    }

    void View::translate(const float x, const float y, const float z, const bool /*parallel*/){
      camera_target_ += algebraica::vec3f(x, y, z);
      update(true);
    }

    const algebraica::vec3f &View::up() const{
      return camera_up_;
    }

    void View::up(const float x, const float y, const float z, const bool normalize){
      camera_up_(x, y, z);

      if(normalize) camera_up_.normalize();
      update(true);
    }

    void View::update(const bool event){
      if(vehicle_ && !event){
        algebraica::vec3f modified_target;
        algebraica::quaternionF vehicle_orientation(vehicle_->orientation.axes.x,
                                                    vehicle_->orientation.axes.y,
                                                    vehicle_->orientation.axes.z,
                                                    vehicle_->orientation.axes.w);

        modified_target = vehicle_orientation * camera_target_;
        modified_target += algebraica::vec3f(vehicle_->position.point.x,
                                             vehicle_->position.point.y,
                                             vehicle_->position.point.z);

        view_matrix_.look_at(modified_target + vehicle_orientation * camera_position_,
                             modified_target, vehicle_orientation * camera_up_);
      }else
        view_matrix_.look_at(camera_target_ + camera_position_, camera_target_, camera_up_);

      multiply_matrices();
    }

    const algebraica::mat4f &View::view_matrix() const{
      return view_matrix_;
    }

    float View::zoom() const{
      return camera_position_.length();
    }

    bool View::zoom(const float distance){
      if(distance >= torero::k_min_distance){
        const float difference{distance / camera_position_.length()};

        if(float_differentiation(difference, 1.0f, 0.005f)){
          camera_position_ *= difference;
          calculate_factors();
          update();
          return true;
        }
        return false;
      }
      return false;
    }

    bool View::zoom(const torero::camera::Zoom zooming){
      const float length{camera_position_.length()};

      if(zooming == torero::camera::Zoom::In)
        return zoom(length - length * 0.1f);
      else
        return zoom(length + length * 0.1f);
    }

    // ::::::::::::::::::::::::::::::::::: Private functions :::::::::::::::::::::::::::::::::::

    void View::calculate_factors(){
      const float length{camera_position_.length()};
      const float windows_width{length * std::sin(torero::k_field_of_view) * 2.0f};

      translational_factor_ = windows_width / to_float(properties_.window_width);
      rotational_factor_ = length * std::sin(torero::k_1_degree) / windows_width;
    }

    void View::multiply_matrices(){
      pv_matrix_ = perspective_matrix_ * view_matrix_;
      spv_matrix_ = perspective_matrix_ * view_matrix_.stationary();

      if(is_callback_set_){
        callback_function_();

        uniform_buffer_.bind();
        uniform_buffer_.allocate_section(&view_matrix_, matrix_size_, offset_view_);
        uniform_buffer_.allocate_section(&pv_matrix_, matrix_size_, offset_pv_);
        uniform_buffer_.allocate_section(&spv_matrix_, matrix_size_, offset_spv_);
        uniform_buffer_.allocate_section(&camera_position_, vector_size_, offset_position_);
        uniform_buffer_.release();
      }
    }

    void View::update_perspective(const bool multiply){
      perspective_matrix_.perspective(torero::k_field_of_view, to_float(properties_.frame_width)
                                      / to_float(properties_.frame_height),
                                      torero::k_near_plane, torero::k_far_plane);
      if(is_callback_set_){
        uniform_buffer_.bind();
        uniform_buffer_.allocate_section(&perspective_matrix_, matrix_size_, offset_projection_);
        uniform_buffer_.release();
      }
      if(multiply)
        multiply_matrices();
    }
  }
}
