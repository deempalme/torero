#include "include/vehicle_manager.h"

namespace Toreo {
  VehicleManager::VehicleManager() :
    null_(0.0f),
    null_d_(0.0),
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
    steering_angle_(&null_),
    ratio_(3.0f),
    navigation_frame_(),
    vehicle_frame_(),
    GPS_(latitude_, longitude_)
  {
  }

  VehicleManager::~VehicleManager(){
    if(signal_update_.connected())
      signal_update_.disconnect();
  }

  void VehicleManager::position(double *latitude, double *longitude, float *altitude){
    position_x_ = &null_;
    position_y_ = &null_;

    if(latitude)
      latitude_ = latitude;
    else
      latitude_ = &null_d_;

    if(longitude)
      longitude_ = longitude;
    else
      longitude_ = &null_d_;

    if(altitude)
      altitude_ = altitude;
    else
      altitude_ = &null_;
  }

  void VehicleManager::position(float *x, float *y, float *z){
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

  void VehicleManager::orietation(float *pitch, float *yaw, float *roll){
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
  }

  void VehicleManager::velocity(float *x, float *y, float *z){
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

  void VehicleManager::steering(float *angle, float ratio){
    if(angle)
      steering_angle_ = angle;
    else
      steering_angle_ = &null_;

    ratio_ = ratio;
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
      Visualizer::pointXY<float> distance = GPS_.GPS_to_origin();
      position_x = distance.x;
      position_z = -distance.y;
    }else{
      position_x = -*position_y_;
      position_z = -*position_x_;
    }

    vehicle_frame_.to_identity();
    vehicle_frame_.translate(position_x, *altitude_, position_z);
    vehicle_frame_.rotate(*pitch_, *yaw_, *roll_);

    navigation_frame_ = vehicle_frame_.only_translation();
  }

  const Algebraica::mat4f *VehicleManager::navigation_frame() const{
    return &navigation_frame_;
  }

  const Algebraica::mat4f *VehicleManager::vehicle_frame() const{
    return &vehicle_frame_;
  }
}
