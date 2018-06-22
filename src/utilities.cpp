#include "torero/utilities.h"
#include "torero/core.h"

namespace Toreo {
  Core Utilities::core_ = nullptr;

  bool Utilities::check_file_existence(std::string &file_path,
                                       const char *file_calling, const int line_calling){
    if(file_path.front() != '/') file_path = "/" + file_path;

    std::string first_path(file_path);
    std::string file_calling_string(file_calling);
    std::string line_calling_string(line_calling);

    if(file_calling_string.size() > 0)
      file_calling_string = "*** " + file_calling_string + " ***\n ";

    if(line_calling_string.size() > 0)
      line_calling_string = "   -> At line: " + line_calling_string + "\n";

    if(!boost::filesystem::exists(boost::filesystem::path(file_path))){
      file_path = boost::filesystem::current_path().string() + file_path;

      if(!boost::filesystem::exists(boost::filesystem::path(file_path))){
        core_->message_handler(file_calling_string + "The file: " + first_path +
                               " was not found.\n  Neither: " + file_path + "\n" +
                               line_calling_string,
                               Visualizer::Message::ERROR);
        return false;
      }
    }
    return true;
  }

  bool Utilities::check_folder_existence(std::string &folder_path,
                                         const char *file_calling, const int line_calling){
    if(folder_path.front() != '/') folder_path = "/" + folder_path;
    if(folder_path.back() != '/') folder_path += "/";

    std::string first_path(folder_path);
    std::string file_calling_string(file_calling);
    std::string line_calling_string(line_calling);

    if(file_calling_string.size() > 0)
      file_calling_string = "*** " + file_calling_string + " ***\n ";

    if(line_calling_string.size() > 0)
      line_calling_string = "   -> At line: " + line_calling_string + "\n";

    if(!boost::filesystem::exists(boost::filesystem::path(folder_path))){
      folder_path = boost::filesystem::current_path().string() + folder_path;

      if(!boost::filesystem::exists(boost::filesystem::path(folder_path))){
        core_->message_handler(file_calling_string + "The folder: " + first_path +
                               " was not found.\n  Neither: " + file_path + "\n" +
                               line_calling_string,
                               Visualizer::Message::ERROR);
        return false;
      }
    }
    return true;
  }
}
