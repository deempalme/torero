#ifndef TORERO_CONTROLLER_H
#define TORERO_CONTROLLER_H

#include "torero/definition/types.h"

#include <boost/signals2.hpp>
#include <string>

namespace torero {
  class Core;

  namespace gui {
    class Controller
    {
    public:
      Controller(torero::Core *core = nullptr);
      ~Controller();

      void go_to_dataset(const int dataset_number);
      void go_to_dataset(const torero::Move position);

      const int go_to_frame(const int frame_number);
      const int go_to_frame(const torero::Move position);
      /*
     * Move the current frame a certain number of frames
     *
     * Adds `frames_moved` to the current frame, if the result is less than zero, then the
     * total number of frames is added (a _spin_ happened), or if is bigger than the total
     * number of frames then is reduced by it (a _spin_ happened).
     *
     * @parameters
     * {const int} frames_moved = amount of frames you want to move
     *
     * @returns
     * {const bool} TRUE if there was a spin in the frame counting
     *
     */
      const bool move_frames(const int frames_moved = 1);

    private:
      torero::Core *core_;
      int current_dataset_, current_frame_, total_datasets_, total_frames_;

      boost::signals2::signal<void ()> frame_signal_, dataset_signal_;
    };
  }
}
#endif // TORERO_CONTROLLER_H
