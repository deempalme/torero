#include "torero/ros/bag_controller.h"

#include "torero/file/manager.h"
#include "torero/terminal/printer.h"

namespace torero {
  namespace Ros {
    BagController::BagController() :
      properties_(),
      bag_duration_(0.0),
      duration_(0.0),
      start_(0.0),
      end_(0.0),
      player_(nullptr),
      bag_(),
      exist_(false)
    {
      properties_.at_once = true;
      properties_.time_scale = 1.0;
//      properties_.bag_time = true;
//      properties_.bag_time_frequency = 1.0;
      properties_.loop = true;
//      properties_.keep_alive = true;
      properties_.quiet = true;
    }

    BagController::~BagController(){
      if(player_)
        delete player_;
    }

    bool BagController::close(){
      exist_ = false;
      return stop();
    }

    bool BagController::end(const double end){
      if(!player_ || !exist_ || (end + start_) > bag_duration_ || end <= 0.0) return false;
      duration_ = end - start_;
      end_ = end;
      properties_.duration = duration_;
      properties_.has_duration = true;
      stop();
      play();
      return true;
    }

    double BagController::end(){
      return end_;
    }

    const std::string &BagController::bag(){
      return bag_;
    }

    const std::vector<std::string> &BagController::list(){
      return properties_.topics;
    }

    bool BagController::open(const std::string &rosbag_path){
      std::string path(rosbag_path);
      if(player_) delete player_;

      if(!(exist_ = torero::file::Manager::check_path(&path)))
        return false;

      rosbag::Bag bag(path);
      rosbag::View view(bag);
      std::vector<const rosbag::ConnectionInfo*> connection_infos = view.getConnections();
      properties_.bags.clear();
      properties_.topics.clear();
      properties_.bags.push_back(rosbag_path);
      properties_.duration = 0.0f;
      properties_.has_duration = false;
      properties_.time = 0.0f;
      properties_.has_time = false;

      for(const rosbag::ConnectionInfo *info : connection_infos)
        properties_.topics.push_back(info->topic);

      bag_ = rosbag_path;
      bag_duration_ = duration_ = end_ = view.getEndTime().toSec() - view.getBeginTime().toSec();
      start_ = 0.0;

      return true;
    }

    bool BagController::play(){
      if(player_ || !exist_) return false;
      player_ = new rosbag::Player(properties_);
      try{
        player_->publish();
      }catch(std::runtime_error &ex){
        ROS_FATAL("%s", ex.what());
        return false;
      }
      return true;
    }

    bool BagController::speed(const double scale){
      if(!player_ || !exist_ || scale <= 0.0) return false;

      properties_.time_scale = scale;
      stop();
      play();
      return true;
    }

    double BagController::speed(){
      return properties_.time_scale;
    }

    bool BagController::start(const double start){
      if(!exist_ || start < 0.0 || start >= duration_) return false;
      properties_.time = start_ = start;
      properties_.has_time = true;
      stop();
      play();
      return true;
    }

    double BagController::start(){
      return start_;
    }

    bool BagController::stop(){
      if(!player_) return false;
      delete player_;
      return true;
    }
  }
}
