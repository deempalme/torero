#ifndef TORERO_FILE_MANAGER_H
#define TORERO_FILE_MANAGER_H

#include "torero/definition/types.h"

#include <boost/filesystem.hpp>
#include <string>

namespace torero {
  namespace file {
    class Manager
    {
    public:
      Manager();

      /*
     * ### Adds the missing absolute path to a relative path
     *
     * This function is useful when using relative paths to find/call/etc a file. It will copy
     * the executable's absolute path and paste it at the beginning of the `relative_path`
     *
     * @parameters
     * {std::string *} relative_path = pointer address to a string containing the relative path
     *
     */
      static void absolute_path(std::string *relative_path);
      /*
     * ### Checks if file/folder path exists
     *
     * This function checks if the file/folder in `path` exist, if not then, it will add at the
     * beginning of `path` the executable's absolute path, if still not found, then returns
     * `false`. This function will modify `*path` adding the absolute path of the program if
     * the original was not found, if still not found after the addition then, it will be
     * returned to the initial state.
     *
     * @parameters
     * {std::string *} path = pointer address to a string containing the file/folder path
     *
     * @returns
     * {bool} `true` if found
     *
     */
      static bool check_path(std::string *path);
      /*
     * ### Checks if file/folder path exists
     *
     * This function checks if the file/folder in `path` exist, if not then, it will add
     * the executable's absolute path at the beginning of `path`, if still not found,
     * then returns `false`. This function does not modify `path`.
     *
     * @parameters
     * {const std::string &} path = constant string containing the file/folder path
     *
     * @returns
     * {bool} `true` if found
     *
     */
      static bool check_path(const std::string &path);
      /*
     * ### Checks if file/folder path exists
     *
     * Simple file/folder existencial checking, this does not modify the input `path`
     *
     * @parameters
     * {std::string &} path = absolute path of file/folder to check
     *
     * @returns
     * {bool} `true` if found
     *
     */
      static bool exists(const std::string &path);
      /*
     * ### Returns the absolute executable's path
     *
     * This function returns the absolute path of the executable
     *
     * @returns
     * {const std::string &} String containing the executable's absolute path
     *
     */
      static const std::string &program_path();
      /*
     * ### Updating the program's path
     *
     * The program's absolute path does not updates automatically if you change its location.
     * Call this function every time you change the program's location folder to
     * update the executable's absolute path.
     *
     */
      static void reset();

    private:
      static bool initialized_;
      static std::string program_path_;
    };
  }
}
#endif // TORERO_FILE_MANAGER_H
