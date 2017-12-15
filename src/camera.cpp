#include "include/camera.h"

namespace Toreo {
  Camera::Camera(const Algebraica::vec3f position, const Algebraica::vec3f target,
                 const Algebraica::vec3f up, Algebraica::mat4f *vehicle_transformation_matrix) :
    vehicle_transformation_matrix_(vehicle_transformation_matrix),
    view_matrix_(),
    perspective_matrix_(),
    pv_matrix_(),
    spv_matrix_(),
    camera_rotation_matrix_(),
    camera_position_(-position.y(), position.z(), -position.x()),
    camera_target_(-target.y(), target.z(), -target.x()),
    camera_up_(-up.y(), up.z(), -up.x()),
    last_camera_position_(camera_position_),
    last_camera_target_(camera_target_),
    camera_translation_(),
    pitch_(0.0f),
    initial_min_pitch_(_PI2 - atan2(position.z() - target.z(), -position.x() + target.x())),
    initial_max_pitch_(_PI + initial_min_pitch_),
    yaw_(0.0f),
    roll_(0.0f),
    zoom_(INITIAL_ZOOM),
    width_(DEFAULT_WIDTH),
    height_(DEFAULT_HEIGHT),
    fixed_view_(false),
    locked_camera_(true),
    is_callback_set_(false)
  {
    update_perspective(false);
    update_view();
  }

  void Camera::set_position(const float x, const float y, const float z){
    camera_position_(x, y, z);

    initialize();
    update_view();
  }

  void Camera::set_target(const float x, const float y, const float z){
    camera_target_(x, y, z);

    initialize();
    update_view();
  }

  void Camera::set_up(const float x, const float y, const float z){
    camera_up_(x, y, z);
    initialize(false);
    update_view();
  }

  void Camera::set_zoom(const float zoom){
    float previous_zoom = zoom_;

    zoom_ = (zoom > MAXIMUM_ZOOM)? MAXIMUM_ZOOM : zoom;
    zoom_ = (zoom_ < MINIMUM_ZOOM)? MINIMUM_ZOOM : zoom_;

    if(previous_zoom != zoom_)
      update_view();
  }

  void Camera::zooming(const bool zooming_in){
    float previous_zoom = zoom_;

    if(zooming_in){
      zoom_ *= 0.9f;
      zoom_ = (zoom_ < MINIMUM_ZOOM)? MINIMUM_ZOOM : zoom_;
    }else{
      zoom_ *= 1.1f;
      zoom_ = (zoom_ > MAXIMUM_ZOOM)? MAXIMUM_ZOOM : zoom_;
    }

    if(previous_zoom != zoom_)
      update_view();
  }

  void Camera::rotate_camera(const float pitch, const float yaw, const float roll){
    set_rotation(&pitch_, pitch, false);
    set_rotation(&yaw_, yaw, false);
    set_rotation(&roll_, roll, false);

    camera_rotation_matrix_.to_identity();
    camera_rotation_matrix_.rotate(pitch_, yaw_, roll_);

    if(pitch_ > initial_min_pitch_ && pitch_ < initial_max_pitch_)
      camera_up_(0.0f, -1.0f, 0.0f);
    else
      camera_up_(0.0f, 1.0f, 0.0f);
    update_view();
  }

  void Camera::translate_camera(const float x, const float y, const float z){
    camera_translation_ += Algebraica::vec3f(x, y, z);
    update_view();
  }

  void Camera::modify_camera(const int dx, const int dy, const bool rotate){
    if(rotate){
      set_rotation(&yaw_, rotational_width_factor_ * dx);
      set_rotation(&pitch_, rotational_height_factor_ * dy);

      camera_rotation_matrix_.to_identity();
      camera_rotation_matrix_.rotate(pitch_, yaw_, roll_);

      if(pitch_ > initial_min_pitch_ && pitch_ < initial_max_pitch_)
        camera_up_(0.0f, -1.0f, 0.0f);
      else
        camera_up_(0.0f, 1.0f, 0.0f);
    }else{
      camera_translation_ += camera_rotation_matrix_ *
          Algebraica::vec3f(-dx * translational_width_factor_ * zoom_,
                            dy * translational_height_factor_ * zoom_,
                            0);
    }
    update_view();
  }

  void Camera::top_view(){
    camera_rotation_matrix_.to_identity();
    camera_position_(0.0f, 0.0f, 45.0f);
    camera_target_(0.0f, 0.0f, 0.0f);
    camera_up_(0.0f, 1.0f, 0.0f);
    initialize();
    zoom_ = 3.75f;

    update_view();
  }

  void Camera::isometric_view(){
    camera_rotation_matrix_.to_identity();
    camera_position_(0.0f, 5.0f, 12.0f);
    camera_target_(0.0f, 0.0f, 0.0f);
    camera_up_(0.0f, 1.0f, 0.0f);
    initialize();

    update_view();
  }

  void Camera::fix_view(const bool fix){
    fixed_view_ = fix;

    if(fixed_view_)
      fixed_camera_translation_ = *vehicle_transformation_matrix_ * Algebraica::vec3f();
    else
      fixed_camera_translation_();
  }

  void Camera::lock_camera(const bool lock){
    locked_camera_ = lock;
  }

  void Camera::set_resolution(const int window_width, const int window_height,
                              const int frame_width, const int frame_height){
    width_ = window_width;
    height_ = window_height;
    width_factor_ = frame_width / window_width;
    height_factor_ = frame_height / window_height;
    rotational_width_factor_ = width_factor_ * 0.2;
    rotational_height_factor_ = height_factor_ * 0.2;
    translational_width_factor_ = width_factor_ * 0.02;
    translational_height_factor_ = height_factor_ * 0.02;

    update_perspective(true);
  }

  void Camera::update_perspective(const bool multiply){
    perspective_matrix_.perspective(FIELD_OF_VIEW, width_/static_cast<float>(height_),
                                    NEAR_PLANE, FAR_PLANE);
    if(multiply)
      multiply_matrices();
  }

  void Camera::update_view(){

    Algebraica::vec3f position, target;

    if(fixed_view_){
      position = camera_rotation_matrix_ * camera_position_ + fixed_camera_translation_;
      target = camera_rotation_matrix_ * camera_target_ + fixed_camera_translation_;
    }else{
      if(locked_camera_){
        position = *vehicle_transformation_matrix_ * camera_rotation_matrix_ * camera_position_;
        target = *vehicle_transformation_matrix_ * camera_rotation_matrix_ * camera_target_;
      }else{
        position = vehicle_transformation_matrix_->only_translation() * camera_rotation_matrix_ *
            camera_position_;
        target = vehicle_transformation_matrix_->only_translation() * camera_rotation_matrix_ *
            camera_target_;
      }
    }

    position += camera_translation_;
    target += camera_translation_;

    last_camera_position_ = (position - target) * zoom_ + target;
    last_camera_target_ = target;

    view_matrix_.look_at(last_camera_position_, last_camera_target_, camera_up_);
    multiply_matrices();
  }

  void Camera::set_function_callback(boost::function<void ()> callback_function){
    is_callback_set_ = true;
    callback_function_ = callback_function;
  }

  const Algebraica::mat4f &Camera::view_matrix(){
    return view_matrix_;
  }

  const Algebraica::mat4f &Camera::perspective_matrix(){
    return perspective_matrix_;
  }

  const Algebraica::mat4f &Camera::pv_matrix(){
    return pv_matrix_;
  }

  const Algebraica::mat4f &Camera::static_pv_matrix(){
    return spv_matrix_;
  }

  const Algebraica::vec3f &Camera::camera_position(){
    return last_camera_position_;
  }

  const Algebraica::vec3f &Camera::camera_target(){
    return last_camera_target_;
  }

  const Algebraica::vec3f &Camera::camera_up(){
    return camera_up_;
  }

  void Camera::multiply_matrices(){
    pv_matrix_ = perspective_matrix_ * view_matrix_;
    spv_matrix_ = perspective_matrix_ * view_matrix_.stationary();
    if(is_callback_set_)
      callback_function_();
  }

  void Camera::set_rotation(float *variable, float angle, const bool convert){
    if(convert)
      angle *= toRADIANS;

    angle = *variable + angle;

    if(angle < 0)
      angle += _2PI;
    while(angle >= _2PI)
      angle -= _2PI;

    *variable = angle;
  }

  void Camera::initialize(const bool restart_translation){
    if(restart_translation)
      camera_translation_();

    if(fixed_view_)
      fixed_camera_translation_ = *vehicle_transformation_matrix_ * Algebraica::vec3f();
    else
      fixed_camera_translation_();

    zoom_ = INITIAL_ZOOM;
    pitch_ = 0.0f;
    yaw_ = 0.0f;
    roll_ = 0.0f;

    initial_min_pitch_ = _PI2 - atan2(camera_position_.y() - camera_target_.y(),
                                      camera_position_.z() - camera_target_.z());
    initial_max_pitch_ = _PI + initial_min_pitch_;
  }
}
