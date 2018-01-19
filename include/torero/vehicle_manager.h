#ifndef TORERO_VEHICLE_MANAGER_H
#define TORERO_VEHICLE_MANAGER_H

#include "torero/definitions.h"
#include "torero/types.h"

#include "algebraica/algebraica.h"
#include "torero/coordinate_conversor.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace Toreo {
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
    void orietation(const float *pitch = nullptr,
                    const float *yaw = nullptr,
                    const float *roll = nullptr);
    void orietation(const float *x = nullptr,
                    const float *y = nullptr,
                    const float *z = nullptr,
                    const float *w = nullptr);

    void acceleration(const float *x = nullptr,
                      const float *y = nullptr,
                      const float *z = nullptr);
    void velocity(const float *x = nullptr,
                  const float *y = nullptr,
                  const float *z = nullptr);
    void steering(const float *angle = nullptr,
                  const float ratio = 3.0f);

    void set_vehicle_frame(Algebraica::mat4f *vehicle_frame);
    void set_navigation_frame(Algebraica::mat4f *navigation_frame);

    void connect(boost::signals2::signal<void ()> *signal);
    void update();

    const Algebraica::mat4f *navigation_frame() const;
    const Algebraica::mat4f *vehicle_frame() const;

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

    Algebraica::mat4f *navigation_frame_, *vehicle_frame_;
    CoordinateConversor<float> GPS_;
    boost::signals2::connection signal_update_;
  };
}
#endif // TORERO_VEHICLE_MANAGER_H
