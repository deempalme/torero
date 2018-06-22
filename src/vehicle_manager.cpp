#include "torero/vehicle_manager.h"
#include "torero/core.h"

namespace Toreo {
  VehicleManager::VehicleManager(Core *core) :
    core_(core),
    null_(0.0f),
    latitude_(nullptr),
    longitude_(nullptr),
    position_x_(&null_),
    position_y_(&null_),
    altitude_(&null_),
    pitch_(&null_),
    yaw_(&null_),
    roll_(&null_),
    velocity_x_(&null_),
    velocity_y_(&null_),
    velocity_z_(&null_),
    acceleration_x_(&null_),
    acceleration_y_(&null_),
    acceleration_z_(&null_),
    steering_angle_(&null_),
    ratio_(3.0f),
    is_quaternion_(false),
    navigation_frame_(nullptr),
    vehicle_frame_(nullptr),
    vehicle_frame_yaw_(nullptr),
    GPS_(latitude_, longitude_)
  {
    core->set_module(this);
  }

  VehicleManager::~VehicleManager(){
    if(signal_update_.connected())
      signal_update_.disconnect();
  }

  void VehicleManager::position(const float *latitude,
                                const float *longitude,
                                const float *altitude){
    position_x_ = &null_;
    position_y_ = &null_;

    if(latitude)
      latitude_ = latitude;
    else
      latitude_ = &null_;

    if(longitude)
      longitude_ = longitude;
    else
      longitude_ = &null_;

    if(altitude)
      altitude_ = altitude;
    else
      altitude_ = &null_;
  }

  void VehicleManager::position_xyz(const float *x, const float *y, const float *z){
    latitude_ = nullptr;
    longitude_ = nullptr;

    if(x)
      position_x_ = x;
    else
      position_x_ = &null_;

    if(y)
      position_y_ = y;
    else
      position_y_ = &null_;

    if(z)
      altitude_ = z;
    else
      altitude_ = &null_;
  }

  void VehicleManager::orietation(const float *pitch, const float *yaw, const float *roll){
    if(pitch)
      pitch_ = pitch;
    else
      pitch_ = &null_;

    if(yaw)
      yaw_ = yaw;
    else
      yaw_ = &null_;

    if(roll)
      roll_ = roll;
    else
      roll_ = &null_;
    is_quaternion_ = false;
  }

  void VehicleManager::orietation(const float *x, const float *y, const float *z, const float *w){
    if(x)
      x_ = x;
    else
      x_ = &null_;

    if(y)
      y_ = y;
    else
      y_ = &null_;

    if(z)
      z_ = z;
    else
      z_ = &null_;

    if(w)
      w_ = w;
    else
      w_ = &null_;
    is_quaternion_ = true;
  }

  void VehicleManager::acceleration(const float *x, const float *y, const float *z){
    if(x)
      acceleration_x_ = x;
    else
      acceleration_x_ = &null_;

    if(y)
      acceleration_y_ = y;
    else
      acceleration_y_ = &null_;

    if(z)
      acceleration_z_ = z;
    else
      acceleration_z_ = &null_;
  }

  void VehicleManager::velocity(const float *x, const float *y, const float *z){
    if(x)
      velocity_x_ = x;
    else
      velocity_x_ = &null_;

    if(y)
      velocity_y_ = y;
    else
      velocity_y_ = &null_;

    if(z)
      velocity_z_ = z;
    else
      velocity_z_ = &null_;
  }

  void VehicleManager::steering(const float *angle, const float ratio){
    if(angle)
      steering_angle_ = angle;
    else
      steering_angle_ = &null_;

    ratio_ = ratio;
  }

  void VehicleManager::set_vehicle_frame(Algebraica::mat4f *vehicle_frame){
    vehicle_frame_ = vehicle_frame;
  }

  void VehicleManager::set_vehicle_frame_yaw(Algebraica::mat4f *vehicle_frame_yaw){
    vehicle_frame_yaw_ = vehicle_frame_yaw;
  }

  void VehicleManager::set_navigation_frame(Algebraica::mat4f *navigation_frame){
    navigation_frame_ = navigation_frame;
  }

  void VehicleManager::set_navigation_plus_frame(Algebraica::mat4f *navigation_plus_frame){
    navigation_plus_frame_ = navigation_plus_frame;
  }

  void VehicleManager::connect(boost::signals2::signal<void ()> *signal){
    if(signal_update_.connected())
      signal_update_.disconnect();
    signal_update_ = signal->connect(boost::bind(&VehicleManager::update, this));
  }

  void VehicleManager::update(){
    float position_x;
    float position_z;

    if(latitude_ && longitude_){
      Visualizer::PointXY<float> distance = GPS_.GPS_to_origin();
      position_x = distance.x;
      position_z = -distance.y;
    }else{
      position_x = -*position_y_;
      position_z = -*position_x_;
    }

    vehicle_frame_->to_identity();
    vehicle_frame_->translate(position_x, *altitude_, position_z);
    *vehicle_frame_yaw_ = *vehicle_frame_;
    if(is_quaternion_)
      vehicle_frame_->rotate(*y_, -*z_, -*x_, *w_);
    else
      vehicle_frame_->rotate(-*pitch_, *yaw_, -*roll_);

    vehicle_frame_yaw_->rotate(0.0f, *yaw_, 0.0f);

    *navigation_frame_ = vehicle_frame_->only_translation();
    *navigation_plus_frame_ = *navigation_frame_;
    if(pitch_ && roll_)
        navigation_plus_frame_->rotate(-*pitch_ * std::cos(*yaw_),
                                       0.0f,
                                       -*roll_ * std::cos(*yaw_));
  }

  const Algebraica::mat4f *VehicleManager::navigation_frame() const{
    return navigation_frame_;
  }

  const Algebraica::mat4f *VehicleManager::navigation_plus_frame() const{
    return navigation_plus_frame_;
  }

  const Algebraica::mat4f *VehicleManager::vehicle_frame() const{
    return vehicle_frame_;
  }

  const Algebraica::mat4f *VehicleManager::vehicle_frame_yaw() const{
    return vehicle_frame_yaw_;
  }
}
