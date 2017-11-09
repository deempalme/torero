#ifndef SKYBOX_H
#define SKYBOX_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "includes/buffer.h"
#include "includes/definitions.h"
#include "includes/shader.h"
#include "includes/types.h"

#include "Algebraica.h"
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <stb_image.h>

#include <fstream>
#include <string>

class Console;

class Skybox
{
public:
  explicit Skybox(const char *folder_path, const char *file_extension,
                  Algebraica::mat4f *const pv_matrix, Console *console, GLFWwindow *window);

  void change_folder(const char *folder_path);
  void reload_data();
  void draw();

  const bool is_ready();

private:
  void load_images();
  void load_ready();

  void write_data_opengl(const Visualizer::ImageFile &image, const int level);

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  boost::signals2::signal<void (const std::string, const int)> message_signal;
  boost::signals2::signal<void ()> ready;

  std::string folder_path_;
  std::string file_extension_;
  Algebraica::mat4f *const pv_matrix_;
  Console *console_;
  GLFWwindow *window_;
  bool is_ready_;

  Shader shader_program_;
  GLuint texture_id_;
  GLint i_position_, u_pv_, u_skybox_;
  Buffer vertex_array_;
  GLsizei stride_size_;
  const GLvoid *offset_pointer_;

  Visualizer::ImageFile up_, down_, left_, right_, front_, back_;

  boost::thread runner_;
  boost::mutex protector_;
};

#endif // SKYBOX_H
