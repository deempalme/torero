#include "torero/file/manager.h"
#include "torero/terminal/printer.h"

namespace torero {
  namespace file {
    Manager::Manager(){}

    void Manager::absolute_path(std::string *relative_path){
      if(relative_path->front() != '/')
        *relative_path = program_path() + '/' + *relative_path;
      else
        *relative_path = program_path() + *relative_path;
    }

    bool Manager::check_path(std::string *path){
      const std::string initial_path(*path);
      if(path->front() != '/') *path = '/' + *path;

      if(!boost::filesystem::exists(boost::filesystem::path(*path))){
        *path = program_path() + *path;

        if(!boost::filesystem::exists(boost::filesystem::path(*path))){
          torero::terminal::Printer::error("The file: " + initial_path + " was not found.\n" +
                                           "  Neither: " + *path + "\n");
          *path = initial_path;
          return false;
        }
      }
      return true;
    }

    bool Manager::check_path(const std::string &path){
      std::string modifiable(path);
      if(path.front() != '/') modifiable = '/' + modifiable;

      if(!boost::filesystem::exists(boost::filesystem::path(modifiable))){
        modifiable = program_path() + modifiable;

        if(!boost::filesystem::exists(boost::filesystem::path(modifiable))){
          torero::terminal::Printer::error("The file: " + path + " was not found.\n" +
                                           "  Neither: " + modifiable + "\n");
          return false;
        }
      }
      return true;
    }

    bool Manager::exists(const std::string &path){
      return boost::filesystem::exists(boost::filesystem::path(path));
    }

    const std::string &Manager::program_path(){
      if(!initialized_) reset();
      return program_path_;
    }

    void Manager::reset(){
      program_path_ = boost::filesystem::current_path().string();
      initialized_ = true;
    }

    // ::::::::::::::::::::::::::::::: INITIALIZING STATIC VARIABLES ::::::::::::::::::::::::::::::::

    std::string Manager::program_path_;
    bool Manager::initialized_{false};
  }
}
