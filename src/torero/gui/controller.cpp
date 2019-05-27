#include "torero/core.h"
#include "torero/gui/controller.h"

namespace torero {
  namespace gui {
    Controller::Controller(torero::Core *core) :
      core_(core),
      current_dataset_(0),
      current_frame_(0),
      total_frames_(0)
    {
    }

    Controller::~Controller(){

    }

    void Controller::go_to_dataset(const int dataset_number){

    }

    void Controller::go_to_dataset(const Move position){

    }

    const int Controller::go_to_frame(const torero::Move position){
      switch(position){
        case torero::Move::Beginning:
          current_frame_ = 0;
        break;
        case torero::Move::Backward:
          current_frame_ = current_frame_ <= 0 ? total_frames_ - 1 : current_frame_ - 1;
        break;
        case torero::Move::Ending:
          current_frame_ = total_frames_ - 1;
        break;
        default:
          current_frame_ = current_frame_ >= total_frames_ - 1 ? 0 : current_frame_ + 1;
        break;
      }
      return current_frame_;
    }

    const int Controller::go_to_frame(const int frame_number){
      current_frame_ = frame_number;

      if(current_frame_ < 0) current_frame_ = 0;
      else if(current_frame_ >= total_frames_) current_frame_ = total_frames_ - 1;
      return current_frame_;
    }

    const bool Controller::move_frames(const int frames_moved){
      current_frame_ += frames_moved;
      if(current_frame_ < 0){
        current_frame_ = total_frames_ - 1 - current_frame_;
        return true;
      }else if(current_frame_ >= total_frames_){
        current_frame_ -= total_frames_ - 1;
        return true;
      }
      return false;
    }
  }
}
