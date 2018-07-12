#ifndef TORERO_VEHICLE_MANAGER_H
#define TORERO_VEHICLE_MANAGER_H

#include "torero/type_definitions.h"

#include "algebraica/algebraica.h"
#include "torero/coordinate_conversor.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  class VehicleManager
  {
  public:
    VehicleManager(Core *core);
    ~VehicleManager();

    void position(const float *latitude = nullptr,
                  const float *longitude = nullptr,
                  const float *altitude = nullptr);
    void position_xyz(const float *x = nullptr,
                      const float *y = nullptr,
                      const float *z = nullptr);
    void orietation(const float *pitch, const float *yaw, const float *roll);
    void orietation(const float *x, const float *y, const float *z, const float *w);

    void acceleration(const float *x = nullptr,
                      const float *y = nullptr,
                      const float *z = nullptr);
    void velocity(const float *x = nullptr,
                  const float *y = nullptr,
                  const float *z = nullptr);
    void steering(const float *angle = nullptr,
                  const float ratio = 3.0f);

    void set_vehicle_frame(algebraica::mat4f *vehicle_frame);
    void set_vehicle_frame_yaw(algebraica::mat4f *vehicle_frame_yaw);
    void set_navigation_frame(algebraica::mat4f *navigation_frame);
    void set_navigation_plus_frame(algebraica::mat4f *navigation_plus_frame);

    void connect(boost::signals2::signal<void ()> *signal);
    void update();

    const algebraica::mat4f *navigation_frame() const;
    const algebraica::mat4f *navigation_plus_frame() const;
    const algebraica::mat4f *vehicle_frame() const;
    const algebraica::mat4f *vehicle_frame_yaw() const;

  private:
    Core *core_;

    float null_;

    const float *latitude_, *longitude_;
    const float *position_x_, *position_y_, *altitude_;
    const float *pitch_, *yaw_, *roll_;
    const float *x_, *y_, *z_, *w_;
    const float *acceleration_x_, *acceleration_y_, *acceleration_z_;
    const float *velocity_x_, *velocity_y_, *velocity_z_;
    const float *steering_angle_;
    float ratio_;
    bool is_quaternion_;

    algebraica::mat4f *navigation_frame_, *navigation_plus_frame_;
    algebraica::mat4f *vehicle_frame_, *vehicle_frame_yaw_;
    CoordinateConversor<float> GPS_;
    boost::signals2::connection signal_update_;
  };
}
#endif // TORERO_VEHICLE_MANAGER_H
