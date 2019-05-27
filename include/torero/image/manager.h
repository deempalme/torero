#ifndef TORERO_IMAGE_MANAGER_H
#define TORERO_IMAGE_MANAGER_H

#include "torero/definition/types.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <string>
#include <vector>

class SDL_Surface;

namespace torero {
  class Core;
  namespace thread {class Manager; }

  namespace image {
    struct ManagerImage {
      boost::function<void (torero::image::File*)> callback;
      torero::image::File *data_pointer;
      boost::mutex *mutex_pointer;
      const std::string path;
    };

    struct ManagerSurface {
      boost::function<void (torero::image::Surface*)> callback;
      torero::image::Surface *data_pointer;
      boost::mutex *mutex_pointer;
      const std::string path;
    };

    class Manager
    {
    public:
      /*
     * ### Constructor
     *
     * Use this constructor when you don't want to create a `torero::Core` class and you also
     * don't want to create a external `torero::MultiThreadManager` class.
     *
     */
      Manager();
      /*
     * ### Constructor
     *
     * Use this constructor when you have already created `torero::Core` class so, this will
     * obtain the `MultiThreadManager`'s class function without need of creating it again.
     *
     * @Arguments
     * {torero::Core*} core = Torero main class where `MultiThreadManager`'s class would be read
     *
     */
      Manager(torero::Core *core);
      /*
     * ### Constructor
     *
     * Use this constructor when you don't want to create a `torero::Core` class and you have
     * already created the `torero::MultiThreadManager` class.
     *
     * @Arguments
     * {torero::MultiThreadManager*} threader = Thread management class
     *
     */
      Manager(torero::thread::Manager *threader);
      ~Manager();

      /*
     * ### Frees memory of SDL Surface
     *
     * This function frees the memory of a SDL Surface, call it when the surface has no
     * more use. You MUST use this function for each SDL Surface you loaded using this
     * class, if not then, memory will leak and you computer crash.
     *
     * @Arguments
     * {toreo::SurfaceImage &} surface = variable where the data will be deleted
     *
     * @Returns
     * {bool} `true` if data was found and deleted
     *
     */
      bool free(torero::image::Surface &surface);
      /*
     * ### Frees memory of a lone STBI image
     *
     * This function frees the memory of a image data loaded using STB library, call it when
     * the image data has no more use. You MUST use this function for each image loaded using
     * this class, if not then, memory will leak and you computer crash.
     *
     * @Arguments
     * {toreo::ImageFile &} image = variable where the data will be deleted
     *
     * @Returns
     * {bool} `true` if data was found and deleted
     *
     */
      bool free(torero::image::File &image);
      /*
     * ### Frees memory of a lone STBI image data address
     *
     * This function frees the memory of a image data loaded using STB library using only the
     * data pointer `unsigned char*`, call it when the image data has no more use. You MUST
     * use this function for each image loaded using this class, if not then, memory will
     * leak and you computer crash.
     *
     * @Arguments
     * {unsigned char *} data = pointer to data loaded using STBI library
     *
     * @Returns
     * {bool} `true` if data was found and deleted
     *
     */
      bool free(unsigned char *data);
      /*
     * ### Load image file and saves it into memory
     *
     * This function loads an image file from `file_path`, and saves it into memory.
     * You should use `bool free(torero::ImageFile &image) to delete the loaded data when is
     * not used anymore, or use `stbi_free(torero::ImageFile.data)`. You could mirror the Y axis
     * by setting `y_mirrored = true`, which is very useful when loading textures or images
     * into OpenGL.
     *
     * @Arguments
     * {const std::string &} file_path = absolute or relative file location
     * {const bool}         y_mirrored = indicates if Y-axis should be mirrored
     *
     * @returns
     * {torero::ImageFile} Image data pointed using `unsigned char*`, DELETE after use.
     *
     */
      torero::image::File load_image(const std::string &file_path, const bool y_mirrored = true);
      /*
     * ### Load image file and saves it into a SDL surface
     *
     * This function loads an image file from `file_path`, save it into memory and create a new
     * `SDL_Surface`'s class. You should use `bool free(torero::SurfaceImage &surface) to delete
     * the loaded data when is not used anymore, or **delete** `torero::SurfaceImage.data` before
     * **deleting** `torero::SurfaceImage.surface`. You could mirror the Y axis by setting
     * `y_mirrored = true`, which is very useful when loading textures or images into OpenGL.
     *
     * @Arguments
     * {const std::string &} file_path = absolute or relative file location
     * {const bool}         y_mirrored = indicates if Y-axis should be mirrored
     *
     * @returns
     * {torero::SurfaceImage} Surface struct that contains the pointer to dynamically created
     * data and SDL surface. Make sure you **DELETE IT** when not used anymore.
     *
     */
      torero::image::Surface load_surface(const std::string &file_path, const bool y_mirrored = false);
      /*
     * ### Load image file and saves it into memory using separated CPU's thread
     *
     * This function loads an image file from `file_path`, and saves it into memory using a new
     * CPU's thread, this will not interfere with the main thread (slow it down or freeze). It
     * is important to define a **callback** function, which will be called after the loading is
     * finished returning the `torero::ImageFile` variable so, you could handle it.
     * You should use `bool free(torero::ImageFile &image) to delete the loaded data when is
     * not used anymore, or use `stbi_free(torero::ImageFile.data)`. You could mirror the Y axis
     * by setting `y_mirrored = true`, which is very useful when loading textures or images
     * into OpenGL.
     *
     * @Arguments
     * {boost::function<void (torero::ImageFile)>} callback = Function that will be called after
     *                                                        the image is fully loaded.
     *    Structure:
     *        ```c++
     *          // Function definition:
     *          void YourClass::your_function(torero::ImageFile incoming_image);
     *
     *          // Example of use:
     *          load_image_concurrently(boost::bind(&YourClass::your_function, this, _1), ...);
     *        ```
     *
     * {const std::string &} file_path = absolute or relative file location
     * {const bool}         y_mirrored = indicates if Y-axis should be mirrored
     *
     */
      void load_image_concurrently(boost::function<void (torero::image::File*)> callback,
                                   const std::string &file_path, const bool y_mirrored = true);
      /*
     * ### Load image file and saves it into a SDL surface using a separated CPU's thread
     *
     * This function loads an image file from `file_path`, and saves it into memory and creates
     * a new `SDL_Surface`'s class using a new CPU's thread, this will not interfere with the
     * main thread (slow it down or freeze). It is important to define a **callback** function,
     * which will be called after the loading is finished returning the `torero::SurfaceImage`
     * variable so, you could handle it. You should use `bool free(torero::SurfaceImage &surface)
     * to delete the loaded data when is not used anymore. You could mirror the Y axis by setting
     * `y_mirrored = true`, which is very useful when loading textures or images into OpenGL.
     *
     * @Arguments
     * {boost::function<void (torero::SurfaceImage)>} callback = Function that will be called after
     *                                                           the SDL_Surface is fully loaded.
     *    Structure:
     *        ```c++
     *          // Function definition:
     *          void YourClass::your_function(torero::SurfaceImage incoming_surface);
     *
     *          // Example of use:
     *          load_surface_concurrently(boost::bind(&YourClass::your_function, this, _1), ...);
     *        ```
     *
     * {const std::string &} file_path = absolute or relative file location
     * {const bool}         y_mirrored = indicates if Y-axis should be mirrored
     *
     */
      void load_surface_concurrently(boost::function<void (torero::image::Surface*)> callback,
                                     const std::string &file_path, const bool y_mirrored = false);

    private:
      void run_image(void *data_poniter);
      void run_surface(void *data_poniter);
      void ready_image(void *data_pointer);
      void ready_surface(void *data_pointer);

      torero::Core *core_;
      torero::thread::Manager *threader_;
      bool lone_wolf_;
    };
  }
}
#endif // TORERO_IMAGE_MANAGER_H
