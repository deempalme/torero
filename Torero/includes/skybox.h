#ifndef SKYBOX_H
#define SKYBOX_H

#include "glad/glad.h"

#include "includes/definitions.h"
#include "includes/types.h"
#include "includes/console.h"

#include "Algebraica.h"
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// image loader
#define  STB_IMAGE_STATIC
#define  STB_IMAGE_IMPLEMENTATION
#define  STBI_NO_HDR
#include <stb_image.h>

#include <fstream>
#include <string>

using namespace std;
using namespace boost;

class Console;

class Skybox
{
public:
  explicit Skybox(const char *folder_path, const char *file_extension,
                  const mat4f *pv_matrix, Console *console);
  ~Skybox();

  void change_folder(const char *folder_path);
  void reload_data();
  void draw();

  const bool is_ready();

private:
  void load_images();
  void load_ready();

  const char *load_shader(const char *file_path);
  void write_data_opengl(const ImageFile &image, const int level);

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  signals2::signal<void (const string, const int)> error_signal;
  signals2::signal<void (const string, const int)> message_signal;
  signals2::signal<void ()> ready;

  string folder_path_;
  string file_extension_;
  const mat4f *pv_matrix_;
  Console *console_;
  bool is_ready_;

  GLuint shader_program_, textures_id_;
  GLint i_position_, u_pv_, u_skybox_;

  ImageFile up_, down_, left_, right_, front_, back_;

  boost::thread runner_;
  boost::mutex protector_;
};

#endif // SKYBOX_H
