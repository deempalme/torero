#ifndef THREE_DIMENSIONAL_MODEL_LOADER_H
#define THREE_DIMENSIONAL_MODEL_LOADER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "includes/definitions.h"
#include "includes/types.h"
#include "includes/console.h"

#include "Algebraica.h"
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <IL/il.h>

#include <string>
#include <fstream>
#include <stdio.h>

using namespace std;
using namespace boost;

class Console;

class ThreeDimensionalModelLoader
{
public:
  explicit ThreeDimensionalModelLoader(const char *folder_address);
  explicit ThreeDimensionalModelLoader(const string folder_address);
  explicit ThreeDimensionalModelLoader(const unsigned int model);
  ~ThreeDimensionalModelLoader();

  const bool is_ready();

  // this class could be called several times and the shader program will always be the same
  static void set_shader_program(const GLuint shader_program);
  static void set_console(Console *console);
  static void set_window(GLFWwindow *window);

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  signals2::signal<void (const char*, const int)> error_signal;
  signals2::signal<void (const char*, const int)> message_signal;
  signals2::signal<void ()> ready;

private:
  void initialize();
  void model_ready();

  const char *folder_address_;
  bool is_ready_;

  static Console *console_;
  static GLFWwindow *window_;
  static GLuint shader_program_;
  GLuint vertex_array_, vertex_buffer_;
  GLint i_position_, i_texture_, i_normal_;
  GLint u_colored_, u_color_, u_metallized_, u_metallic_;
  GLint u_roughed_, u_roughness_, u_is_interior_;

  GLsizei data_size_;
  vector<SimpleShaderData> buffer_data_;

  boost::mutex protector_;
  boost::thread runner_;
};

#endif // THREE_DIMENSIONAL_MODEL_LOADER_H
