#ifndef CONSOLE_H
#define CONSOLE_H

// OpenGL loader and core library
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "includes/camera.h"
#include "includes/definitions.h"
#include "includes/gui.h"
#include "includes/shader.h"
#include "includes/types.h"

// linear mathematical functions
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Algebraica.h"
// timer
#include <chrono>
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// Image loader
#include <stb_image.h>
// multithreading
#include <thread>
#include <mutex>
// standard
#include <iostream>
#include <string>

void resize_callback(GLFWwindow *window, int width, int height);
void mouse_click_callback(GLFWwindow *window, int button, int action, int mods);
void mouse_move_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

class Skybox;
class ThreeDimensionalModelLoader;

class Console
{
public:
  explicit Console(int _argc, char **_argv);
  ~Console();

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- CAMERA MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  // Changes the position of the camera, note that is only the location of the camera,
  // the point that the camera is aiming will not be modified
  void camera_set_position(const float x = -12.0f, const float y = 0.0f, const float z = 5.0f);
  // This specifies the point which the camera is looking
  void camera_set_target(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
  // Changes the rotation of the camera, changes the upward side of the camera.
  // You must introduce the coordinates of a vector, this vector represents the camera
  // orientation, by default is pointing up parallel to Y axis.
  void camera_set_up(const float x = 0.0f, const float y = 0.0f, const float z = 1.0f);
  // This changes the scenary rotation in euler angles, the angles are in RADIANS
  void camera_rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
  // This translates the camera position and target
  void camera_translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
  // This changes the scenary zoom, maximum = 0.05 : minimum = 15
  void camera_set_zoom(const float zoom = 1.0f);
  // Changes the view to "Top view"
  void camera_top_view();
  // Changes the view to "Isometric view"
  void camera_isometric_view();
  // Returns the view transformation matrix
  const Algebraica::mat4f &view_matrix();
  // Returns the perspective transformation matrix
  const Algebraica::mat4f &perspective_matrix();
  // Returns the multiplied matrix between Perspective and view transformation matrices
  const Algebraica::mat4f &pv_matrix();
  // Returns the multiplied matrix between Perspective and view transformation matrices,
  // note that this matrix only containts the rotations of pv_matrix and not translations
  const Algebraica::mat4f &static_pv_matrix();

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- SCENE's FRAME MATRICES ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  // Returns the fixed frame matrix, its origins is the Latitude and Longitude 0°.
  const Algebraica::mat4f *fixed_frame();
  // Returns the vehicle frame matrix, its origins is the vehicle origin.
  const Algebraica::mat4f *vehicle_frame();
  // Returns the navigation frame matrix, its origins is the vehicle origin
  // and always is pointing east.
  const Algebraica::mat4f *navigation_frame();

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  // Use this to change the window title (which appears at the bar in the top side)
  // Using const char:
  void set_window_title(const char *title);
  // Using standard string:
  void set_window_title(const std::string title);
  // This function changes the size of the window, use negative values to equalize window
  // and monitor width or/and height. For full screen windows, the specified size becomes
  // the new resolution of the window's desired video mode. The video mode most closely
  // matching the new desired video mode is set immediately. The window is resized to fit
  // the resolution of the set video mode.
  void set_window_size(const int width, const int height);
  // Changes the window's position in screen,
  // The origin of the coordinate system is at the top-left corner of the monitor
  void set_window_position(const int x, const int y);
  // Maximizes or resizes the window to its defined width and height
  void maximize_window(const bool maximized = true);
  // Minimizes or resizes the window to its defined width and height
  void minimize_window(const bool minimized = true);
  // Makes the window full screen
  void full_screen(const bool make_full = true);

  // Redraws the window's contents
  void redraw_screen();

  // The next three functions are very useful and give the program enough time to redraw
  // the screen, if you are using a infinite loop to execute your program then, you will
  // need to call one of these functions at least once every cycle:

  // Waits until an event is triggered (mouse, keyboard, etc)
  void wait_for_events();
  // Waits until an event is triggered (mouse, keyboard, etc) or
  // until the timeout (in seconds) has passed.
  void wait_for_events(const double timeout);
  // Processes all pending events.
  void process_pending_events();

  // This is the main function; it will open a new window with predefined properties or
  // properties defined by the user suchlike, width, height, title, etc.
  // If infinite_loop = true then, it will maintain the window open until a close event
  // is triggered.
  int execute(const bool infinite_loop = true);
  // Width and height represent the window resolution, if full_screen is true then, your
  // screen resolution will change, to have the native resolution of your system, set width
  // and height to zero, also, if width and height is equal to zero and full_screen is false
  // then, width and height will change to initial values: 1350 and 600 respectively.
  int execute(const int width, const int height, const char *title = "Torero",
              const bool full_screen = true, const bool maximized = false,
              const bool infinite_loop = true);

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  // This is the handler to realize a connection to a signal that is activated after the
  // camera has been modified.
  boost::signals2::signal<void ()> *updated_camera_signal();
  // This is the handler to realize a connection to a signal that is activated after the
  // OpenGL screen has been painted.
  boost::signals2::signal<void ()> *updated_screen_signal();

  static boost::signals2::signal<void (int, int)> resize_signal;
  static boost::signals2::signal<void (double, double)> mouse_move_signal;
  static boost::signals2::signal<void (int, int)> mouse_click_signal;
  static boost::signals2::signal<void (double)> mouse_scroll_signal;

  // ------------------------------------------------------------------------------------ //
  // --------------------------------------- SLOTS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  void message_handler(const std::string text, const int message_type);
  void model_ready(ThreeDimensionalModelLoader *model);

protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(const int width, const int height);

private:
  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- EVENT MANAGER ---------------------------------- //
  // ------------------------------------------------------------------------------------ //
  void mouse_click_event(int button, int action);
  void mouse_move_event(double xpos, double ypos);
  void mouse_scroll_event(double yoffset);

  void updated_camera();

  // texture loading
  void load_icon();

  int argc_;
  char **argv_;
  GLFWwindow *window_;
  int width_, height_, half_height_, position_x_, position_y_;
  int error_log_, error_;
  bool is_left_click_, is_right_click_, is_scroll_click_;
  int old_x_, old_y_;
  bool is_inversed_;

  // subclasses
  GUI *gui_;
  Skybox *skybox_;
  std::vector<Visualizer::Model3D> models_;

  float max_filtering_;
  std::vector<GLuint> textures_id_;

  // physical properties
  Algebraica::mat4f fixed_frame_, vehicle_frame_, navigation_frame_;
  Camera camera_;

  // signals
  boost::signals2::signal<void ()> updated_camera_signal_, updated_screen_signal_;
};

#endif // CONSOLE_H
