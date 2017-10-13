#ifndef CONSOLE_H
#define CONSOLE_H

// OpenGL loader and core library
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "headers/definitions.h"
#include "headers/types.h"
#include "headers/gui.h"

// linear mathematical functions
#include "Eigen/Core"
#include "Eigen/Geometry"
//#include "Algebraica.h"
// timer
#include <chrono>
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// Image loader
#include <IL/il.h>
// multithreading
#include <thread>
#include <mutex>
// standard
#include <iostream>

using namespace std;
using namespace boost;
using namespace Eigen;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Console
{
public:
  explicit Console(int _argc, char **_argv);
  ~Console();
  // This is the Main function; it will open a new window.
  // Width and height represent the window resolution, if full_screen is true then, your screen
  // resolution will change, to have the native resolution of your system, set width and height to
  // zero, also, if width and height is equal to zero and full_screen is false then, width and
  // height will change to initial values: 1350 and 600 respectively.
  int execute(const int width = 1350, const int height = 600, const char *title = "Torero",
              const bool full_screen = true, const bool maximized = false);

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  static signals2::signal<void (int, int)> resize_signal;

protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(const int width, const int height);

private:
  int argc_;
  char **argv_;
  GLFWwindow *window_;
  int width_, height_;
  bool is_main_open_;
  GUI *gui_;

  unsigned int VBO, VAO;
  int shaderProgram;
};

#endif // CONSOLE_H
