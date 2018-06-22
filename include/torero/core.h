#ifndef TORERO_CORE_H
#define TORERO_CORE_H

// OpenGL loader and core library
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "torero/camera.h"
#include "torero/definitions.h"
#include "torero/multithread_manager.h"
#include "torero/screen_conversor.h"
#include "torero/types.h"

// linear mathematical functions
#include "algebraica/algebraica.h"
// standard
#include <iostream>
#include <string>
// Boost
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

namespace Toreo {
  class ModelManager;
  class VehicleManager;

  class Core : public MultiThreadManager
  {
  public:
    /*
     * ### Constructor
     *
     * This function will create the **Window management** *class*.
     *
     * **Arguments**
     * {int} argc = Number of arguments.
     * {char**} argv = Array containing the character string of all arguments.
     *
     * **Errors**
     * This will throw an error if GLFW or GLAD libraries were not correctly loaded/created.
     * In this case a message will be displayed at the Terminal/console.
     *
     */
    explicit Core(int argc, char **argv, const bool system_title = false);
    ~Core();

    // ------------------------------------------------------------------------------------ //
    // -------------------------------- CAMERA MANAGEMENT --------------------------------- //
    // ------------------------------------------------------------------------------------ //
    bool *camera_blocker();
    /*
     * ### Obtaining the perspective transformation matrix
     *
     * Returns the **perspective** transformation matrix, this matrix is calculated using
     * **perspective** matrix transformation with window's width and height as parameters
     * to get the aspect radio. A near plane with value `NEAR_PLANE`, far plane with value
     * `FAR_PLANE` and field of view with value `FIELD_OF_VIEW`.
     *
     * **Returns**
     * {const Algebraica::mat4f} Returns the 4x4 view projection matrix (perspective view).
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f &camera_matrix_perspective();
    const Algebraica::mat4f &camera_matrix_perspective_inversed();
    /*
     * ### Obtaining the multiplied matrix between perspective and view matrices
     *
     * Returns the multiplied matrix between **perspective and view** transformation matrices.
     *
     * **Returns**
     * {const Algebraica::mat4f} Returns the resulting 4x4 matrix of the projection and
     * camera matrices multiplication.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f &camera_matrix_perspective_view();
    const Algebraica::mat4f &camera_matrix_perspective_view_inversed();
    /*
     * ### Obtaining the non-translated multiplied matrix between perspective and view matrices
     *
     * Returns the **non-translated** multiplied matrix between **perspective and view**
     * transformation matrices. **Note:* this matrix only contains the **rotations** of
     * pv_matrix and **not translations**.
     *
     * **Returns**
     * {const Algebraica::mat4f} Returns the resulting 4x4 matrix of the projection and
     * camera matrices multiplication WITHOUT translation (only rotation).
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f &camera_matrix_perspective_view_static();
    const Algebraica::mat4f &camera_matrix_perspective_view_static_inversed();
    /*
     * ### Obtaining the view transformation matrix
     *
     * Returns the **view** transformation matrix, this matrix is calculated using
     * **look at** matrix transformation with the camera as parameter.
     *
     * **Returns**
     * {const Algebraica::mat4f} Returns the 4x4 transformation matrix of the camera.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f &camera_matrix_view();
    const Algebraica::mat4f &camera_matrix_view_inversed();
    /*
     * ### Obtaining the camera position
     *
     * This function returns a 3D vector with the camera position in X, Y and Z.
     *
     * **Returns**
     * {const Algebraica::vec3f} 3D vector width camera position.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::vec3f &camera_position();
    const Algebraica::vec3f &camera_relative_position();
    /*
     * ### Rotating the camera
     *
     * Multiplies the Camera position by a rotation matrix using euler angles
     * (the center is the camera target's position), the angles are in **RADIANS**.
     * See the [[coordinate systems|Coordinate-systems#orientation-angles]] to view more
     * details about the angles.
     *
     * **Arguments**
     * {const float} pitch = Pitch angle in radians.
     * {const float} yaw = Yaw angle in radians.
     * {const float} roll = Roll angle in radians.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
//    void camera_rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void camera_rotate(const Algebraica::quaternionF quaternion);
    /*
     * ### Setting camera position
     *
     * Changes the position of the camera, note that is only the location of the camera,
     * the point that the camera is aiming will not be modified.
     *
     * **Arguments**
     * {const float} x = Position in meters in the X axis.
     * {const float} y = Position in meters in the Y axis.
     * {const float} z = Position in meters in the Z axis.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_set_position(const float x = -12.0f, const float y = 0.0f, const float z = 5.0f);
    /*
     * ### Setting camera target position
     *
     * This specifies the point which the camera is looking.
     *
     * **Arguments**
     * {const float} x = Position in meters in the X axis.
     * {const float} y = Position in meters in the Y axis.
     * {const float} z = Position in meters in the Z axis.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_set_target(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    /*
     * ### Setting camera upwards direction
     *
     * Changes the rotation of the camera, changes the upward side of the camera.
     * You must introduce the coordinates of a **normalized** vector, this vector
     * represents the camera orientation, by default is pointing up parallel to Y axis.
     *
     * **Arguments**
     * {const float} x = Position in meters in the X axis, must be normalized.
     * {const float} y = Position in meters in the Y axis, must be normalized.
     * {const float} z = Position in meters in the Z axis, must be normalized.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_set_up(const float x = 0.0f, const float y = 0.0f, const float z = 1.0f);
    /*
     * ### Setting the camera's zoom
     *
     * This changes the scenary zoom, maximum = 0.05 : minimum = 15. The smaller the zoom
     * factor is, the closer to the target the camera position becomes.
     *
     * **Arguments**
     * {const float} zoom = Zoom factor, if `zoom = 0.05` then, the camera position
     * **vector** is multiplied for 0.05.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_set_zoom(const float zoom = 1.0f);
    /*
     * ### Translating the camera
     *
     * Multiplies the Camera position by a translation matrix (the center is the camera target's position).
     *
     * **Arguments**
     * {const float} x = Distance in X axis to translate.
     * {const float} y = Distance in Y axis to translate.
     * {const float} z = Distance in Z axis to translate.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    /*
     * ### Updating the camera
     *
     * This function updates the camera position and orientation, it is very useful when the
     * position of the **vehicle transformation matrix** has been modified by your own code.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_update();
    /*
     * ### Change view to isometric view
     *
     * Changes the view to **Isometric view**.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_view_isometric();
    /*
     * ### Change view to top view
     *
     * Changes the view to **Top view**.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_view_top();
    void camera_zoom_in();
    void camera_zoom_out();
    void close();
    /*
     * ### Checking if window is closing
     *
     * This function returns if the window is closing (which may not close the program execution),
     * if you are utilizing an infinite loop, this function will be very useful to break that
     * loop and also close the program.
     *
     * **Returns**
     * `{bool} Returns `true` if is closed/closing or `false` if is open.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    bool closing();
    /*
     * ### Execute window
     *
     * This is the main function; it will open a new window with predefined properties
     * or properties defined by the user suchlike, width, height, title, etc. If
     * `infinite_loop = true` then, it will maintain the window open until a close
     * event is triggered.
     *
     * **Arguments**
     * {const bool} infinite_loop = if set to `true` it will maintain the window open
     * until close event.
     *
     * **Errors**
     * This will return the type of error if the window was not created properly;
     * `EXIT_SUCCESS`, `GLFW_NOT_LOADED`, `WINDOW_NOT_LOADED`, `GLAD_NOT_LOADED`
     * or `EXISTING_WINDOW`.
     *
     */
    int execute(const bool infinite_loop = true);
    /*
     * ### Execute window with extra parameters
     *
     * **Width** and **height** represent the window resolution, if `full_screen` is `true`
     * then, your screen resolution will change, to have the native resolution of your system,
     * set **width** and **height** to *zero*, also, if **width** and **height** is equal to
     * *zero* and `full_screen` is `false` then, **width** and **height** will change to
     * initial values: 1500 and 800 respectively.
     *
     * **Arguments**
     * {const bool} infinite_loop = If set to `true` it will maintain the window open until
     * close event.
     * {const int} width = Window's width in pixels.
     * {const int} height = Window's height in pixels.
     * {const std::string} title = The UTF-8 encoded window's title.
     * {const bool} full_screen = if set to `true` it will make the window full screen.
     * {const bool} maximized = if set to `true` it will maximize the window, ignored if
     * `full_screen = true`.
     *
     * **Errors**
     * This will return the type of error if the window was not created properly;
     * `EXIT_SUCCESS`, `GLFW_NOT_LOADED`, `WINDOW_NOT_LOADED`, `GLAD_NOT_LOADED`
     * or `EXISTING_WINDOW`.
     *
     */
    int execute(const int width, const int height, const std::string title = "Torero",
                const bool full_screen = true, const bool maximized = false,
                const bool infinite_loop = true);

    // ------------------------------------------------------------------------------------ //
    // ----------------------------- SCENE's FRAME MATRICES ------------------------------- //
    // ------------------------------------------------------------------------------------ //
    /*
     * ### Obtaining the "fixed frame" transformation matrix
     *
     * This function returns the *address* to the **fixed frame** transformation matrix,
     * its origins is the Latitude and Longitude 0° (see
     * [[coordinate system|Coordinate-systems#fixed-frame]] for more information).
     *
     * **Returns**
     * {const Algebraica::mat4f *const} Address to the **fixed frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f *frame_global() const;
    /*
     * ### Obtaining the "vehicle frame" transformation matrix
     *
     * This function returns the *address* to the **vehicle frame** transformation matrix,
     * its origins is at the center of the rear wheel axis (see
     * [[coordinate system|Coordinate-systems#vehicle-frame]] for more information).
     *
     * **Returns**
     * {const Algebraica::mat4f *const} Address to the **vehicle frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f *frame_vehicle() const;
    const Algebraica::mat4f *frame_vehicle_yaw() const;
    /*
     * ### Obtaining the "navigation frame" transformation matrix
     *
     * This function returns the *address* to the **navigation frame** transformation matrix,
     * its origins is at the center of the rear wheel axis but it does NOT rotate (see
     * [[coordinate system|Coordinate-systems#navigation-frame]] for more information).
     *
     * **Returns**
     * {const Algebraica::mat4f *const} Address to the **navigation frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const Algebraica::mat4f *frame_navigation() const;
    const Algebraica::mat4f *frame_navigation_plus() const;
    /*
     * ### Full screen window
     *
     * Makes the window full screen or resizes back the window to its defined width and
     * height. **This will hide the title and launcher bar**.
     *
     * **Arguments**
     * {const bool}	make_full =	Set this to `true` to make full screen or `false` to restore
     * the window's size back to `width` and `height`.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void full_screen(const bool make_full = true);
    const double get_time();
    const int *height() const;
    /*
     * ### Maximize window
     *
     * Maximizes to fit all the monitor or resizes back the window to its defined width and height.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void maximize();
    /*
     * ### Displaying messages in GUI and Terminal/Console
     *
     * This function displays messages into the GUI window (in the message box) and in the
     * Ubuntu Terminal or Window's console.
     *
     * **Arguments**
     * {const std::string} text = Text to display in the GUI and Terminal/Console.
     * {const int} message_type = Type of message to display.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void message_handler(const std::string message, const Visualizer::Message message_type);
    void message_handler(const unsigned int message, const Visualizer::Message message_type);
    /*
     * ### Minimize window
     *
     * Minimizes the window or resizes it back to its defined width and height.
     *
     * **Arguments**
     * {const bool}	minimized =	Set this to `true` to minimize or `false` to restore the window's
     * size back to `width` and `height`.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void minimize(const bool minimized = true);
    const int move_frame();
    bool *mover();
    const bool paused();
    /*
     * ### Processing all pending events
     *
     * This function processes all pending events suchlike screen drawing without any waiting.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void process_pending_events();
    /*
     * ### Restarting the screen viewport
     *
     * This function restarts the screen viewport to its original aspect radio; defined when
     * you defined the window's **width** and **height** or uses its pre-defined values.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void restart_viewport();
    /*
     * ### Maximum anisotropic filtering
     *
     * This functions return the maximum anisotropic filtering than your computer could handle,
     * it is very useful when defining your own textures.
     *
     * **Returns**
     * {const GLfloat} Returns the maximum anisotropic filtering value or 8.0f if is bigger.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    bool *screen_changer();
    ScreenConversor *screen_conversor();
    const GLfloat screen_max_anisotropic_filtering();
    void screen_paint();
    /*
     * ### Redrawing the window's screen
     *
     * Redraws the whole window's screen, if you have updated data that have not been drawn
     * in screen you could call this function. **Note:* try to call this function only when
     * is really needed because is a big performance consumer, a **better practice** is to
     * draw every element separately.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void screen_redraw();
    /*
     * ### Changing between framebuffers (OpenGL)
     *
     * This function switches between back and front framebuffers, it is mandatory to use it
     * after you manually redraw the screen \[`redraw_screen()`\] and if you used `execute(false)`.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void screen_swap_buffers();
    void set_cursor(const Visualizer::Cursor type);
    bool set_module(ModelManager *model_manager);
    /*
     * ### Setting the vehicle management class
     *
     * This function obtains and connects the **vehicle**, **navigation matrices** and other
     * data from the `VehicleManager` class addressed at the argument.
     *
     * **Arguments**
     * {VehicleManager*} vehicle_manager = Address to the **vehicle manager** class.
     *
     * **Returns**
     * {bool} Returns `true` if there was a previously addressed `VehicleManager` class.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    bool set_module(VehicleManager *vehicle_manager);
    /*
     * ### Window position
     *
     * Changes the window's position in screen, The origin of the coordinate system is at
     * the top-left corner of the monitor.
     *
     * **Arguments**
     * {const int}	x =	Position from the left side of the monitor in pixels.
     * {const int}	y =	Position from the top side of the monitor in pixels.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void set_position(const int x, const int y);
    /*
     * ### Window size
     *
     * This function changes the size of the window, use negative values to equalize window
     * and monitor width or/and height. For full screen windows, the specified size becomes
     * the new resolution of the window. The window is resized to fit the resolution of the
     * set video mode.
     *
     * **Arguments**
     * {const int} width = Size in pixels of the window. Use any negative value to set the
     * window's width equal to the monitor's resolution.
     * {const int}	height	= Size in pixels of the window. Use any negative value to set the
     * window's height equal to the monitor's resolution.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void set_size(const int width, const int height);
    /*
     * ### Window title
     *
     * To change the window title (which appears at the bar in the top side of the window) use:
     *
     * **Arguments**
     * {const std::string} title = The UTF-8 encoded window title.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void set_title(const std::string title);
    /*
     * ### Checking window's visility
     *
     * This function returns the window's visibility; `true` if is not minimized or
     * `false` if is minimized.
     *
     * **Returns**
     * {bool}`true` if is not minimized or `false` if is minimized.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    bool visibility();
    /*
     * ### Waiting for triggering events
     *
     * Waits until an event is triggered (mouse, keyboard, etc). This will pause the program's
     * execution until an event is triggered.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void wait_for_events();
    /*
     * ### Waiting for triggering events or until time runs out
     *
     * Waits until an event is triggered (mouse, keyboard, etc) or until the timeout
     * (in seconds) has passed. This will pause the program's execution until an event
     * is triggered or the time is over.
     *
     * **Arguments**
     * {const double} timeout = Waiting time in seconds, use decimals for more precision.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void wait_for_events(const double timeout);
    const int *width() const;

    // ------------------------------------------------------------------------------------ //
    // ------------------------------------- SIGNALS -------------------------------------- //
    // ------------------------------------------------------------------------------------ //
    /*
     * ### Sincronizing your code with custom signals
     *
     * This functions returns a signal using the **enumerator** `Visualizer::Order`, use it
     * to connect it to your own functions. This signal is triggered when the *class manager*
     * with name equal to this **enumerator** is drawn in screen.
     *
     * **Arguments**
     * {enum Visualizer::Order} object = *Class manager* name.
     *
     * **Returns**
     * This returns a **boost signal** that you could use to connect your code.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    boost::signals2::signal<void ()> *syncronize(const Visualizer::Order object);
    /*
     * ### Signal triggered by camera changes
     *
     * This signal is triggered every time the camera is modified, for example: rotation,
     * translation, zoom, etc. You can obtain the address to this signal by using the
     * following function, to manage such connection is recommended to use
     * `boost::signals2::connection` to connect or disconnect it from your own functions.
     *
     * **Returns**
     * This returns a **boost signal** that you could use to connect your code.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    boost::signals2::signal<void ()> *signal_updated_camera();
    /*
     * ### Signal triggered when OpenGL's screen is redrawn
     *
     * This signal is triggered every time the OpenGL screen is redrawn; every time you
     * see a change in the window's screen this signal will be triggered. You can obtain
     * the address to this signal by using the following function, to manage such connection
     * is recommended to use `boost::signals2::connection` to connect or disconnect it from
     * your own functions.
     *
     * **Returns**
     * This returns a **boost signal** that you could use to connect your code.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    boost::signals2::signal<void ()> *signal_updated_screen();
    boost::signals2::signal<void (int, int)> *signal_resized_screen();
    boost::signals2::signal<void (int, int)> *signal_moved_mouse();
    boost::signals2::signal<void (int, int, int)> *signal_clicked_mouse();
    boost::signals2::signal<void (int, int, int)> *signal_released_mouse();

  protected:
    virtual void initialize();
    virtual void paint();
    virtual void resize(const int width, const int height);

  private:
    static void callback_resize(GLFWwindow *window, int width, int height);
    static void callback_mouse_click(GLFWwindow *window, int button, int action, int mods);
    static void callback_mouse_move(GLFWwindow *window, double xpos, double ypos);
    static void callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset);
    static void callback_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    void event_mouse_click(int button, int action, int mods);
    void event_mouse_move(double xpos, double ypos);
    void event_mouse_scroll(double yoffset);
    void event_key_callback(int key, int scancode, int action, int mods);

    void updated_camera();
    void load_window_icon();
    GLFWcursor *load_mouse_icon(const std::string path, const int x_offset, const int y_offset);

    int argc_;
    char **argv_;
    GLFWwindow *window_;
    int width_, height_, initial_width_, initial_height_, half_height_;
    int position_x_, position_y_, initial_position_x_, initial_position_y_;
    int error_log_, error_;
    bool is_left_click_, is_right_click_, is_scroll_click_, window_moving_;
    int old_x_, old_y_;
    bool is_inversed_, is_maximized_, maximization_, has_changed_, blocked_;
    bool is_window_paused_;
    int frame_mover_;

    GLfloat max_filtering_;
    Algebraica::mat4f global_frame_, vehicle_frame_, vehicle_frame_yaw_;
    Algebraica::mat4f navigation_frame_, navigation_plus_frame_;
    Algebraica::mat4f inversed_vehicle_frame_;
    Camera camera_;

    VehicleManager *vehicle_;
    ModelManager *modeler_;
    ScreenConversor screen_conversor_;

    GLFWcursor *cursor_normal_, *cursor_pointer_, *cursor_beam_, *cursor_cross_, *cursor_move_;

    // signals
    static boost::signals2::signal<void (int, int)> signal_window_resize;
    static boost::signals2::signal<void (int, int, int)> signal_mouse_click;
    static boost::signals2::signal<void (double, double)> signal_mouse_move;
    static boost::signals2::signal<void (double)> signal_mouse_scroll;
    static boost::signals2::signal<void (int, int, int, int)> signal_key_callback;

    boost::signals2::signal<void ()> signal_updated_camera_, signal_updated_screen_;
    boost::signals2::signal<void (int, int)> signal_resized_screen_, signal_mouse_moved_;
    boost::signals2::signal<void (int, int, int)> signal_clicked_mouse_, signal_released_mouse_;
    std::vector<boost::signals2::signal<void ()> > signal_draw_;
  };
}

#endif // TORERO_CORE_H
