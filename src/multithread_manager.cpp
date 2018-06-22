#include "torero/multithread_manager.h"

namespace Toreo {
  MultiThreadManager::MultiThreadManager() :
    active_processes_(0),
    awaiting_processes_(0),
    cores_number_(0),
    active_cores_(0),
    finished_(true)
  {
    cores_number_ = boost::thread::hardware_concurrency();
    cores_number_ = (cores_number_ <= 1) ? boost::thread::physical_concurrency() : cores_number_;
  }

  void MultiThreadManager::multithread_add_process(boost::function<void ()> run,
                                                   boost::function<void ()> ready,
                                                   boost::function<bool ()> is_ready){
    if(active_cores_ >= cores_number_){
      Process new_process = { run, ready, is_ready };
      awaiting_processes_.push_back(new_process);
      finished_ = false;

    }else{
      Process new_process = { run, ready, is_ready, new boost::thread(run) };
      new_process.runner->detach();

      active_processes_.push_back(new_process);
      finished_ = false;

      ++active_cores_;
    }
  }

  void MultiThreadManager::multithread_update_process(){
    std::vector<std::size_t> to_delete(0);
    std::size_t active_size{active_processes_.size()};

    for(std::size_t i = 0u; i < active_size; ++i){
      if(active_processes_[i].is_ready()){
        active_processes_[i].ready();
        if(awaiting_processes_.size() > 0){
          delete active_processes_[i].runner;
          active_processes_[i] = awaiting_processes_[0];
          awaiting_processes_.erase(awaiting_processes_.begin());

          active_processes_[i].runner = new boost::thread(active_processes_[i].run);
          active_processes_[i].runner->detach();
        }else{
          --active_cores_;
          to_delete.push_back(i);
        }
      }
    }

    const int delete_size = to_delete.size() - 1;
    if(delete_size >= 0)
      for(int i = delete_size; i >= 0; i--){
        delete active_processes_[to_delete[i]].runner;
        active_processes_.erase(active_processes_.begin() + to_delete[i]);
      }

    if(active_processes_.size() + awaiting_processes_.size() <= 0){
      finished_ = true;
    }
  }

  const bool MultiThreadManager::multithread_finished(){
    return finished_;
  }
}
