#include "torero/core.h"
#include "torero/vehicle/manager.h"

namespace torero {
  namespace vehicle {
    Manager::Manager(torero::Core *core) :
      core_(core),
      null_(0.0f),
      acceleration_x_(&null_),
      acceleration_y_(&null_),
      acceleration_z_(&null_),
      latitude_(nullptr),
      longitude_(nullptr),
      position_x_(&null_),
      position_y_(&null_),
      altitude_(&null_),
      steering_angle_(&null_),
      steering_ratio_(3.0f),
      velocity_x_(&null_),
      velocity_y_(&null_),
      velocity_z_(&null_),
      x_(&null_),
      y_(&null_),
      z_(&null_),
      w_(&null_),
      global_frame_(),
      navigation_frame_(),
      vehicle_frame_(),
      GPS_(latitude_, longitude_)
    {
      core->set_module(this);
    }

    Manager::~Manager(){
      if(signal_update_.connected())
        signal_update_.disconnect();
    }

    void Manager::position(const float *latitude,
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

    void Manager::position_xyz(const float *x, const float *y, const float *z){
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

    void Manager::orietation(const float *x, const float *y, const float *z, const float *w){
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
    }

    void Manager::orietation(const torero::OrientationXYZW *quaternion){
      if(quaternion){
        x_ = &quaternion->axes.x;
        y_ = &quaternion->axes.y;
        z_ = &quaternion->axes.z;
        w_ = &quaternion->axes.w;
      }
    }

    void Manager::acceleration(const float *x, const float *y, const float *z){
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

    void Manager::velocity(const float *x, const float *y, const float *z){
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

    void Manager::steering(const float *angle, const float ratio){
      if(angle)
        steering_angle_ = angle;
      else
        steering_angle_ = &null_;

      steering_ratio_ = ratio;
    }

    void Manager::connect(boost::signals2::signal<void ()> *signal){
      if(signal_update_.connected())
        signal_update_.disconnect();
      signal_update_ = signal->connect(boost::bind(&vehicle::Manager::update, this));
    }

    void Manager::update(){
      // Position
      if(latitude_ && longitude_){
        torero::PointXY<float> distance = GPS_.GPS_to_origin();
        vehicle_frame_.position.point.x = distance.point.x;
        vehicle_frame_.position.point.y = distance.point.y;
      }else{
        vehicle_frame_.position.point.x = *position_x_;
        vehicle_frame_.position.point.z = *position_y_;
      }
      vehicle_frame_.position.point.z = *altitude_;

      // Orientation
      vehicle_frame_.orientation.axes.x = *x_;
      vehicle_frame_.orientation.axes.y = *y_;
      vehicle_frame_.orientation.axes.z = *z_;
      vehicle_frame_.orientation.axes.w = *w_;

      navigation_frame_.position = vehicle_frame_.position;

      /*core_->message_handler(std::to_string(*x_) + ", "
                           + std::to_string(*y_) + ", "
                           + std::to_string(*z_) + ", "
                           + std::to_string(*w_), torero::Message::Attention);*/
    }

    const torero::Pose3D *Manager::global_frame() const{
      return &global_frame_;
    }

    const torero::Pose3D *Manager::navigation_frame() const{
      return &navigation_frame_;
    }

    const torero::Pose3D *Manager::vehicle_frame() const{
      return &vehicle_frame_;
    }
  }
}
