#ifndef TORERO_ROS_BAG_CONTROLLER_H
#define TORERO_ROS_BAG_CONTROLLER_H

#include "torero/definition/types.h"

#include <string>
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/player.h>
#include <rosbag/view.h>

namespace torero {
  namespace Ros {
    class BagController
    {
    public:
      BagController();
      ~BagController();

      bool close();

      bool end(const double end);
      double end();

      const std::string &bag();

      const std::vector<std::string> &list();

      bool open(const std::string &rosbag_path);

      bool play();

      bool speed(const double scale);
      double speed();

      bool start(const double start);
      double start();

      bool stop();


    private:
      rosbag::PlayerOptions properties_;
      double bag_duration_;
      double duration_, start_, end_;
      rosbag::Player *player_;
      std::string bag_;
      bool exist_;
    };
  }
}
#endif // TORERO_ROS_BAG_CONTROLLER_H
