#ifndef TORERO_VEHICLE_MANAGER_H
#define TORERO_VEHICLE_MANAGER_H

#include "include/definitions.h"
#include "include/types.h"

#include "Algebraica.h"
#include "coordinate_conversor.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace Toreo {
  class VehicleManager
  {
  public:
    VehicleManager();
    ~VehicleManager();

    void position(double *latitude = nullptr, double *longitude = nullptr,
                  float *altitude = nullptr);
    void position(float *x = nullptr, float *y = nullptr, float *z = nullptr);
    void orietation(float *pitch = nullptr, float *yaw = nullptr, float *roll = nullptr);

    void velocity(float *x = nullptr, float *y = nullptr, float *z = nullptr);
    void steering(float *angle = nullptr, float ratio = 3.0f);

    void connect(boost::signals2::signal<void ()> *signal);
    void update();

    const Algebraica::mat4f *navigation_frame() const;
    const Algebraica::mat4f *vehicle_frame() const;

  private:
    float null_;
    double null_d_;

    double *latitude_, *longitude_;
    float *position_x_, *position_y_, *altitude_;
    float *pitch_, *yaw_, *roll_;
    float *velocity_x_, *velocity_y_, *velocity_z_;
    float *steering_angle_;
    float ratio_;

    Algebraica::mat4f navigation_frame_, vehicle_frame_;
    CoordinateConversor<float> GPS_;
    boost::signals2::connection signal_update_;
  };
}
#endif // TORERO_VEHICLE_MANAGER_H
