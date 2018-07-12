#ifndef TORERO_UTILITIES_H
#define TORERO_UTILITIES_H

#include <string>
// Boost libraries
#include <boost/filesystem.hpp>

namespace torero {
  class Core;

  class Utilities
  {
  public:
    inline static bool check_file_existence(std::string &file_path,
                                            const char *file_calling = "",
                                            const int line_calling = 0);

    inline static bool check_folder_existence(std::string &folder_path,
                                              const char *file_calling = "",
                                              const int line_calling = 0);

  private:
    static Core *core_;
  };
}

#endif // TORERO_UTILITIES_H
