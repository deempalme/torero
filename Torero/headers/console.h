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
#include <boost/timer.hpp>
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// Image loader
#include <IL/il.h>
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

  int execute();

  static signals2::signal<void (int, int)> resize_signal;

protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(const int width, const int height);

private:
  int argc_;
  char **argv_;
  GLFWwindow *window_;
  GUI *gui_;

  int width_, height_;
  unsigned int VBO, VAO;
  int shaderProgram;
};

#endif // CONSOLE_H
