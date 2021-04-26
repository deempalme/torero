#ifndef TORERO_CORE_H
#define TORERO_CORE_H

// OpenGL loader and core library
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "include/camera.h"
#include "include/definitions.h"
#include "include/types.h"

// linear mathematical functions
#include "algebraica/algebraica.h"
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
    explicit Core(int argc, char **argv);
    virtual ~Core();

    // ------------------------------------------------------------------------------------ //
    // -------------------------------- CAMERA MANAGEMENT --------------------------------- //
    // ------------------------------------------------------------------------------------ //
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
    void camera_rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
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
     * ### Change view to top view
     *
     * Changes the view to **Top view**.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_top_view();
    /*
     * ### Change view to isometric view
     *
     * Changes the view to **Isometric view**.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void camera_isometric_view();
    /*
     * ### Obtaining the view transformation matrix
     *
     * Returns the **view** transformation matrix, this matrix is calculated using
     * **look at** matrix transformation with the camera as parameter.
     *
     * **Returns**
     * {const algebraica::mat4f} Returns the 4x4 transformation matrix of the camera.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f &camera_matrix_view();
    /*
     * ### Obtaining the perspective transformation matrix
     *
     * Returns the **perspective** transformation matrix, this matrix is calculated using
     * **perspective** matrix transformation with window's width and height as parameters
     * to get the aspect radio. A near plane with value `NEAR_PLANE`, far plane with value
     * `FAR_PLANE` and field of view with value `FIELD_OF_VIEW`.
     *
     * **Returns**
     * {const algebraica::mat4f} Returns the 4x4 view projection matrix (perspective view).
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f &camera_matrix_perspective();
    /*
     * ### Obtaining the multiplied matrix between perspective and view matrices
     *
     * Returns the multiplied matrix between **perspective and view** transformation matrices.
     *
     * **Returns**
     * {const algebraica::mat4f} Returns the resulting 4x4 matrix of the projection and
     * camera matrices multiplication.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f &camera_matrix_perspective_view();
    /*
     * ### Obtaining the non-translated multiplied matrix between perspective and view matrices
     *
     * Returns the **non-translated** multiplied matrix between **perspective and view**
     * transformation matrices. **Note:* this matrix only contains the **rotations** of
     * pv_matrix and **not translations**.
     *
     * **Returns**
     * {const algebraica::mat4f} Returns the resulting 4x4 matrix of the projection and
     * camera matrices multiplication WITHOUT translation (only rotation).
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f &camera_matrix_static_perspective_view();
    /*
     * ### Obtaining the camera position
     *
     * This function returns a 3D vector with the camera position in X, Y and Z.
     *
     * **Returns**
     * {const algebraica::vec3f} 3D vector width camera position.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::vec3f &camera_position();

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
     * {const algebraica::mat4f *const} Address to the **fixed frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f *fixed_frame() const;
    /*
     * ### Obtaining the "vehicle frame" transformation matrix
     *
     * This function returns the *address* to the **vehicle frame** transformation matrix,
     * its origins is at the center of the rear wheel axis (see
     * [[coordinate system|Coordinate-systems#vehicle-frame]] for more information).
     *
     * **Returns**
     * {const algebraica::mat4f *const} Address to the **vehicle frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f *vehicle_frame() const;
    /*
     * ### Obtaining the "navigation frame" transformation matrix
     *
     * This function returns the *address* to the **navigation frame** transformation matrix,
     * its origins is at the center of the rear wheel axis but it does NOT rotate (see
     * [[coordinate system|Coordinate-systems#navigation-frame]] for more information).
     *
     * **Returns**
     * {const algebraica::mat4f *const} Address to the **navigation frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f *navigation_frame() const;

    // ------------------------------------------------------------------------------------ //
    // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
    // ------------------------------------------------------------------------------------ //
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
    void set_window_title(const std::string title);
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
    void set_window_size(const int width, const int height);
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
    void set_window_position(const int x, const int y);
    /*
     * ### Maximize window
     *
     * Maximizes to fit all the monitor or resizes back the window to its defined width and height.
     *
     * **Arguments**
     * {const bool}	maximized =	Set this to `true` to maximize or `false` to restore the window's
     * size back to `width` and `height`.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void maximize_window(const bool maximized = true);
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
    void minimize_window(const bool minimized = true);
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
    void redraw_screen();
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
     * ### Giving time to OpenGL to draw into the screen
     *
     * The next four functions are very useful and give the program enough time to redraw
     * the screen, if you are using an infinite loop to execute your program, then, you will
     * need to call `swap_buffers()` and one of other functions at least 30 times per second
     * to have a fluid animation.
     *
     */
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
    void swap_buffers();
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
    bool window_visibility();
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
    bool window_closing();
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
    // ------------------------------ OPENGL TEXTURE MANAGER ------------------------------ //
    // ------------------------------------------------------------------------------------ //
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
    const GLfloat max_anisotropic_filtering();
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
    boost::signals2::signal<void ()> *syncronize(Visualizer::Order object);
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

    static boost::signals2::signal<void (int, int)> signal_window_resize;
    static boost::signals2::signal<void (int, int)> signal_mouse_click;
    static boost::signals2::signal<void (double, double)> signal_mouse_move;
    static boost::signals2::signal<void (double)> signal_mouse_scroll;

    // ------------------------------------------------------------------------------------ //
    // --------------------------------------- SLOTS -------------------------------------- //
    // ------------------------------------------------------------------------------------ //
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
    void message_handler(const std::string text, const Visualizer::Message message_type);

  protected:
    virtual void initialize();
    virtual void paint();
    virtual void resize(const int width, const int height);

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
    algebraica::mat4f identity_matrix_;
    algebraica::mat4f *fixed_frame_, *vehicle_frame_, *navigation_frame_;
    Camera camera_;

    // signals
    boost::signals2::signal<void ()> signal_updated_camera_, signal_updated_screen_;
    std::vector<boost::signals2::signal<void ()> > signal_draw_;
  };
}

#endif // TORERO_CORE_H
