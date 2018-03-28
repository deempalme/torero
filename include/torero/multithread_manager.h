#ifndef TORERO_MULTITHREAD_MANAGER_H
#define TORERO_MULTITHREAD_MANAGER_H

#include "torero/types.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <vector>

namespace Toreo {
  struct Process {
    boost::function<void ()> run;
    boost::function<void ()> ready;
    boost::function<bool ()> is_ready;
    boost::thread *runner = nullptr;
  };

  class MultiThreadManager
  {
  public:
    MultiThreadManager();

    void multithread_add_process(boost::function<void ()> run,
                         boost::function<void ()> ready,
                         boost::function<bool ()> is_ready);
    void multithread_update_process();

    const bool multithread_finished();

  private:
    std::vector<Process> active_processes_;
    std::vector<Process> awaiting_processes_;

    unsigned int cores_number_;
    unsigned int active_cores_;
    bool finished_;
  };
}

#endif // TORERO_MULTITHREAD_MANAGER_H
