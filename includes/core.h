#ifndef TORERO_CORE_H
#define TORERO_CORE_H

// OpenGL loader and core library
#include "libs/GLAD/include/glad/glad.h"
#include "libs/glfw/include/GLFW/glfw3.h"

#include "includes/camera.h"
#include "includes/definitions.h"
#include "includes/types.h"

// linear mathematical functions
#include "libs/algebraica/Algebraica.h"
// Image loader
#include "libs/stb/stb_image.h"
// standard
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

void callback_resize(GLFWwindow *window, int width, int height);
void callback_mouse_click(GLFWwindow *window, int button, int action, int mods);
void callback_mouse_move(GLFWwindow *window, double xpos, double ypos);
void callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset);

namespace Toreo {
  class Core
  {
  public:
    explicit Core(int _argc, char **_argv);
    ~Core();

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
    const Algebraica::mat4f &camera_matrix_view();
    // Returns the perspective transformation matrix
    const Algebraica::mat4f &camera_matrix_perspective();
    // Returns the multiplied matrix between Perspective and view transformation matrices
    const Algebraica::mat4f &camera_matrix_perspective_view();
    // Returns the multiplied matrix between Perspective and view transformation matrices,
    // note that this matrix only contains the rotations of pv_matrix and not translations
    const Algebraica::mat4f &camera_matrix_static_perspective_view();
    // Returns the camera position
    const Algebraica::vec3f &camera_position();

    // ------------------------------------------------------------------------------------ //
    // ----------------------------- SCENE's FRAME MATRICES ------------------------------- //
    // ------------------------------------------------------------------------------------ //
    // Returns the fixed frame matrix, its origins is the Latitude and Longitude 0°.
    const Algebraica::mat4f *fixed_frame() const;
    // Returns the vehicle frame matrix, its origins is the vehicle origin.
    const Algebraica::mat4f *vehicle_frame() const;
    // Returns the navigation frame matrix, its origins is the vehicle origin
    // and always is pointing east.
    const Algebraica::mat4f *navigation_frame() const;

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
    void restart_viewport();

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
    // Returns the window object
    GLFWwindow *get_window();

    // ------------------------------------------------------------------------------------ //
    // ------------------------------ OPENGL TEXTURE MANAGER ------------------------------ //
    // ------------------------------------------------------------------------------------ //
    // Returns a new texture unit value
    //  const GLuint add_texture();
    // Returns the last current texture unit used
    //  const GLuint current_texture();
    // Returns the maximum anisotropic filtering value or 8 if is bigger
    const GLfloat max_anisotropic_filtering();
    // ------------------------------------------------------------------------------------ //
    // ------------------------------------- SIGNALS -------------------------------------- //
    // ------------------------------------------------------------------------------------ //
    // This is the handler to realize a connection to a signal that is activated after the
    // camera has been modified.
    boost::signals2::signal<void ()> *signal_updated_camera();
    // This is the handler to realize a connection to a signal that is activated after the
    // OpenGL screen has been painted. This will NOT update the data in point clouds,
    // trajectories, boxes, etc.
    boost::signals2::signal<void ()> *signal_updated_screen();
    // This is the handler to realize a connection to a signal that is activated after the
    // OpenGL frame has been changed. This WILL update the data in point clouds,
    // trajectories, boxes, etc.
    boost::signals2::signal<void ()> *signal_updated_frame();

    static boost::signals2::signal<void (int, int)> signal_window_resize;
    static boost::signals2::signal<void (int, int)> signal_mouse_click;
    static boost::signals2::signal<void (double, double)> signal_mouse_move;
    static boost::signals2::signal<void (double)> signal_mouse_scroll;

    // ------------------------------------------------------------------------------------ //
    // --------------------------------------- SLOTS -------------------------------------- //
    // ------------------------------------------------------------------------------------ //
    void message_handler(const std::string text, const int message_type);

  protected:
    virtual void initialize();
    virtual void paint();
    virtual void resize(const int width, const int height);
    virtual void update();

  private:
    void event_mouse_click(int button, int action);
    void event_mouse_move(double xpos, double ypos);
    void event_mouse_scroll(double yoffset);

    void updated_camera();
    void load_window_icon();

    int argc_;
    char **argv_;
    GLFWwindow *window_;
    int width_, height_, half_height_, position_x_, position_y_;
    int error_log_, error_;
    bool is_left_click_, is_right_click_, is_scroll_click_;
    int old_x_, old_y_;
    bool is_inversed_, has_changed_;

    GLfloat max_filtering_;
    Algebraica::mat4f identity_matrix_;
    Algebraica::mat4f *fixed_frame_, *vehicle_frame_, *navigation_frame_;
    Camera camera_;

    // signals
    boost::signals2::signal<void ()> signal_updated_camera_, signal_updated_screen_;
    boost::signals2::signal<void ()> signal_updated_frame_;
  };
}

#endif // TORERO_CORE_H
