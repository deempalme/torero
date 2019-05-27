#ifndef TORERO_VEHICLE_MANAGER_H
#define TORERO_VEHICLE_MANAGER_H

#include "torero/coordinate_conversor.h"
#include "torero/definition/types.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace vehicle {
    class Manager
    {
    public:
      Manager(torero::Core *core);
      ~Manager();

      void position(const float *latitude = nullptr,
                    const float *longitude = nullptr,
                    const float *altitude = nullptr);
      void position_xyz(const float *x = nullptr,
                        const float *y = nullptr,
                        const float *z = nullptr);
      void orietation(const float *x, const float *y, const float *z, const float *w);
      void orietation(const torero::OrientationXYZW *quaternion);

      void acceleration(const float *x = nullptr,
                        const float *y = nullptr,
                        const float *z = nullptr);
      void velocity(const float *x = nullptr,
                    const float *y = nullptr,
                    const float *z = nullptr);
      void steering(const float *angle = nullptr,
                    const float ratio = 3.0f);

      void connect(boost::signals2::signal<void ()> *signal);
      void update();

      const torero::Pose3D *global_frame() const;
      const torero::Pose3D *navigation_frame() const;
      const torero::Pose3D *vehicle_frame() const;

    private:
      torero::Core *core_;

      float null_;

      const float *acceleration_x_, *acceleration_y_, *acceleration_z_;
      const float *latitude_, *longitude_;
      const float *position_x_, *position_y_, *altitude_;
      const float *steering_angle_;
      float steering_ratio_;
      const float *velocity_x_, *velocity_y_, *velocity_z_;
      const float *x_, *y_, *z_, *w_;

      torero::Pose3D global_frame_, navigation_frame_, vehicle_frame_;
      torero::CoordinateConversor<float> GPS_;
      boost::signals2::connection signal_update_;
    };
  }
}
#endif // TORERO_VEHICLE_MANAGER_H
