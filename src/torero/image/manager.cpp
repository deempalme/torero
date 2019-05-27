#include "torero/core.h"
#include "torero/image/manager.h"
#include "torero/thread/manager.h"

// Image loader
#include "stb_image.h"
// SDL Surface class
#include <SDL2/SDL_surface.h>

namespace torero {
  namespace image {
    Manager::Manager() :
      core_(nullptr),
      threader_(new torero::thread::Manager()),
      lone_wolf_(true)
    {}

    Manager::Manager(torero::Core *core) :
      core_(core),
      threader_(nullptr),
      lone_wolf_(false)
    {}

    Manager::Manager(torero::thread::Manager *threader) :
      core_(nullptr),
      threader_(threader),
      lone_wolf_(false)
    {}

    Manager::~Manager(){
      if(lone_wolf_ && threader_)
        delete threader_;
    }

    torero::image::File Manager::load_image(const std::string &file_path, const bool y_mirrored){
      if(y_mirrored)
        stbi_set_flip_vertically_on_load(1);
      else
        stbi_set_flip_vertically_on_load(0);

      torero::image::File image;
      image.is_inverted = y_mirrored;
      image.data = stbi_load(file_path.c_str(), &image.width, &image.height,
                             &image.components_size, 0);

      if(!image.data)
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Loading image (" << file_path
                  << ") failed " << stbi_failure_reason() << "\033[0m" << std::endl;

      return image;
    }

    torero::image::Surface Manager::load_surface(const std::string &file_path,
                                                 const bool y_mirrored){
      if(y_mirrored)
        stbi_set_flip_vertically_on_load(1);
      else
        stbi_set_flip_vertically_on_load(0);

      torero::image::Surface surface;
      surface.image->is_inverted = y_mirrored;

      surface.image->data = stbi_load(file_path.c_str(), &surface.image->width,
                                      &surface.image->height, &surface.image->components_size,
                                      STBI_rgb_alpha);

      if(!surface.image->data){
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Loading image (" << file_path
                  << ") failed " << stbi_failure_reason() << "\033[0m" << std::endl;
        return surface;
      }

      uint32_t red_mask{0x000000ff}, green_mask{0x0000ff00};
      uint32_t blue_mask{0x00ff0000}, alpha_mask{0xff000000};

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      red_mask   = 0xff000000;
      green_mask = 0x00ff0000;
      blue_mask  = 0x0000ff00;
      alpha_mask = 0x000000ff;
#endif

      surface.surface = SDL_CreateRGBSurfaceFrom(static_cast<void*>(surface.image->data),
                                                 surface.image->width, surface.image->height,
                                                 sizeof(float), STBI_rgb_alpha * surface.image->width,
                                                 red_mask, green_mask, blue_mask, alpha_mask);

      if(!surface.surface){
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Creating surface (" << file_path
                  << ") failed " << SDL_GetError() << "\033[0m" << std::endl;
        stbi_image_free(surface.image->data);
        surface.image->data = nullptr;
      }

      return surface;
    }

    void Manager::load_image_concurrently(boost::function<void (torero::image::File*)> callback,
                                          const std::string &file_path, const bool y_mirrored){
      torero::image::ManagerImage *data{nullptr};

      if(core_){
        data = new torero::image::ManagerImage{ callback, new torero::image::File{ y_mirrored,
               0, 0, 0, nullptr }, new boost::mutex(), file_path };
        void *void_pointer = static_cast<void*>(data);

        core_->multithread_add_process(boost::bind(&image::Manager::run_image, this, void_pointer),
                                       boost::bind(&image::Manager::ready_image, this, void_pointer),
                                       data->mutex_pointer);
      }else if(threader_){
        data = new torero::image::ManagerImage{ callback, new torero::image::File{ y_mirrored,
               0, 0, 0, nullptr }, new boost::mutex(), file_path };
        void *void_pointer = static_cast<void*>(data);

        threader_->multithread_add_process(boost::bind(&image::Manager::run_image, this,
                                                       void_pointer),
                                           boost::bind(&image::Manager::ready_image, this,
                                                       void_pointer),
                                           data->mutex_pointer);
      }
    }

    void Manager::load_surface_concurrently(boost::function<void (torero::image::Surface*)> callback,
                                            const std::string &file_path, const bool y_mirrored){
      torero::image::ManagerSurface *data{nullptr};

      if(core_){
        data = new torero::image::ManagerSurface {
               callback, new torero::image::Surface{ nullptr,
               new torero::image::File{ y_mirrored, 0, 0, 0, nullptr }},
               new boost::mutex(), file_path };
        void *void_pointer = static_cast<void*>(data);

        core_->multithread_add_process(boost::bind(&image::Manager::run_surface, this, void_pointer),
                                       boost::bind(&image::Manager::ready_surface, this, void_pointer),
                                       data->mutex_pointer);
      }else if(threader_){
        data = new torero::image::ManagerSurface{
               callback, new torero::image::Surface{ nullptr,
               new torero::image::File{ y_mirrored, 0, 0, 0, nullptr }},
               new boost::mutex(), file_path };
        void *void_pointer = static_cast<void*>(data);

        threader_->multithread_add_process(boost::bind(&image::Manager::run_surface, this,
                                                       void_pointer),
                                           boost::bind(&image::Manager::ready_surface, this,
                                                       void_pointer),
                                           data->mutex_pointer);
      }
    }

    bool Manager::free(torero::image::Surface &surface){
      SDL_FreeSurface(surface.surface);
      stbi_image_free(surface.image->data);
    }

    bool Manager::free(torero::image::File &image){
      stbi_image_free(image.data);
    }

    bool Manager::free(unsigned char *data){
      stbi_image_free(data);
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PRIVATES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //

    void Manager::run_image(void *data_pointer){
      torero::image::ManagerImage *image = static_cast<torero::image::ManagerImage*>(data_pointer);

      if(image->data_pointer->is_inverted)
        stbi_set_flip_vertically_on_load(1);
      else
        stbi_set_flip_vertically_on_load(0);

      image->mutex_pointer->lock();

      image->data_pointer->data = stbi_load(image->path.c_str(), &image->data_pointer->width,
                                            &image->data_pointer->height,
                                            &image->data_pointer->components_size, 0);

      if(!image->data_pointer->data)
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Loading image ("
                  << image->path << ") failed " << stbi_failure_reason() << "\033[0m" << std::endl;

      image->mutex_pointer->unlock();
    }

    void Manager::run_surface(void *data_pointer){
      torero::image::ManagerSurface *surface =
          static_cast<torero::image::ManagerSurface*>(data_pointer);

      if(surface->data_pointer->image->is_inverted)
        stbi_set_flip_vertically_on_load(1);
      else
        stbi_set_flip_vertically_on_load(0);

      surface->mutex_pointer->lock();

      surface->data_pointer->image->data = stbi_load(surface->path.c_str(),
                                                     &surface->data_pointer->image->width,
                                                     &surface->data_pointer->image->height,
                                                     &surface->data_pointer->image->components_size,
                                                     STBI_rgb_alpha);

      if(!surface->data_pointer->image->data)
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Loading image (" << surface->path
                  << ") failed " << stbi_failure_reason() << "\033[0m" << std::endl;

      surface->mutex_pointer->unlock();
    }

    void Manager::ready_image(void *data_pointer){
      torero::image::ManagerImage *image = static_cast<torero::image::ManagerImage*>(data_pointer);
      delete image->mutex_pointer;

      if(!image->data_pointer->data){
        delete image->data_pointer;
        delete image;
        return;
      }

      image->callback(image->data_pointer);
      delete image;
    }

    void Manager::ready_surface(void *data_pointer){
      torero::image::ManagerSurface *surface =
          static_cast<torero::image::ManagerSurface*>(data_pointer);

      delete surface->mutex_pointer;
      if(!surface->data_pointer->image->data){
        delete surface->data_pointer->image;
        delete surface->data_pointer->surface;
        delete surface;
        return;
      }

      uint32_t red_mask{0x000000ff}, green_mask{0x0000ff00};
      uint32_t blue_mask{0x00ff0000}, alpha_mask{0xff000000};

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      red_mask   = 0xff000000;
      green_mask = 0x00ff0000;
      blue_mask  = 0x0000ff00;
      alpha_mask = 0x000000ff;
#endif

      surface->data_pointer->surface =
          SDL_CreateRGBSurfaceFrom(static_cast<void*>(surface->data_pointer->image->data),
                                   surface->data_pointer->image->width,
                                   surface->data_pointer->image->height, sizeof(float),
                                   STBI_rgb_alpha * surface->data_pointer->image->width,
                                   red_mask, green_mask, blue_mask, alpha_mask);

      if(!surface->data_pointer->surface){
        std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m " << "Creating surface ("
                  << surface->path << ") failed " << SDL_GetError() << "\033[0m" << std::endl;
        stbi_image_free(surface->data_pointer->image->data);
        surface->data_pointer->image->data = nullptr;
        delete surface->data_pointer->image;
        delete surface->data_pointer;
        delete surface;
        return;
      }
      surface->callback(surface->data_pointer);
      delete surface;
    }
  }
}
