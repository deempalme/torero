#ifndef TORERO_CORE_H
#define TORERO_CORE_H

// OpenGL loader and SDL2 library
#include "glad/glad.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
//#include <SDL2/SDL_vulkan.h>

#include "torero/definition/types.h"
#include "torero/event/handler.h"
#include "torero/gl/texture.h"
#include "torero/gui/cursors.h"
#include "torero/thread/manager.h"

// linear mathematical functions
#include "algebraica/algebraica.h"
// standard
#include <iostream>
#include <string>
// Boost
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

namespace torero {
  namespace camera { class View; }
  namespace gui {
    class Controller;
    class Manager;
  }
  namespace model {
    class Environment;
    class Manager;
  }
  namespace image { class Manager; }
  namespace text { class Manager; }
  namespace vehicle { class Manager; }

  class Core : public gui::Cursors, public event::Handler, public thread::Manager
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
    explicit Core(int /*argc*/, char** /*argv*/, const bool system_title = false);
    ~Core();

    torero::camera::View &camera();
    torero::gui::Controller &controller();
    torero::model::Environment &environment();
    torero::image::Manager &image_manager();
    torero::text::Manager &text_manager();
    torero::vehicle::Manager &vehicle();

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
     * {int} width = Window's width in pixels.
     * {int} height = Window's height in pixels.
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
    int execute(int width, int height, const std::string title = "Torero",
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
     * {const algebraica::mat4f *const} Address to the **fixed frame** transformation matrix.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    const algebraica::mat4f *frame_global() const;
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
    const algebraica::mat4f *frame_vehicle() const;
    const algebraica::mat4f *frame_vehicle_yaw() const;
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
    const algebraica::mat4f *frame_navigation() const;
    const algebraica::mat4f *frame_navigation_plus() const;
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
    uint32_t get_time();
    /*
     * ### Maximize window
     *
     * Maximizes to fit all the monitor or resizes back the window to its defined width and height.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    void maximize(const bool maximize = true);
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
    void minimize(const bool minimize = true);
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
    void reload_screen(const bool change = true);
    void sdl_error();
    void set_module(torero::gui::Manager */*gui_manager*/,
                    torero::model::Environment *&environment,
                    torero::text::Manager *&text_manager);
    const torero::model::Environment &set_module(torero::model::Manager */*model_manager*/);
    void set_module(torero::vehicle::Manager *vehicle_manager);
    void screen_clear();
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
    const GLfloat screen_max_filtering();
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
    torero::PointXY<int> position();
    void position(const int x, const int y);
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
    torero::PointXY<int> size();
    void size(const int width, const int height);
    void sleep(const uint32_t milliseconds);
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
    void title(const std::string &title);
    const std::string &title();
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
    void visibility(const bool hide);
//    const int *width() const;

    // ------------------------------------------------------------------------------------ //
    // ------------------------------------- SIGNALS -------------------------------------- //
    // ------------------------------------------------------------------------------------ //
    /*
     * ### Sincronizing your code with custom signals
     *
     * This functions returns a signal using the **enumerator** `torero::Order`, use it
     * to connect it to your own functions. This signal is triggered when the *class manager*
     * with name equal to this **enumerator** is drawn in screen.
     *
     * **Arguments**
     * {enum torero::Order} object = *Class manager* name.
     *
     * **Returns**
     * This returns a **boost signal** that you could use to connect your code.
     *
     * **Errors**
     * This will return error if the window was not created properly.
     *
     */
    boost::signals2::connection syncronize(const torero::Order object,
                                           boost::function<void ()> callback);
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
    boost::signals2::connection connect_camera_updates(boost::function<void ()> callback);
    /*
     * ### Connection to a signal triggered when OpenGL's screen is redrawn
     *
     * Use this function to connect your own function to a signal that is triggered every time
     * the OpenGL screen is redrawn; every time you see a change in the window's screen this
     * signal will be triggered. You will obtain a `boost::signals2::connection` to manage the
     * newly created connection.
     *
     * **Returns**
     * This returns a **boost connection** that you could use to disconnect your callback function.
     *
     */
    boost::signals2::connection connect_sreeen_updates(boost::function<void ()> callback);

  protected:
    virtual void initialize();
    virtual void paint();

  private:
    void updated_camera();
    void load_window_icon();

    SDL_Window *window_;
    SDL_GLContext context_;
    std::string title_;

    bool error_;
    int error_log_;

    GLfloat max_filtering_;

    const algebraica::mat4f identity_matrix_;

    torero::camera::View *camera_;
    torero::gui::Controller *controller_;
    torero::model::Environment *environment_;
    torero::image::Manager *image_manager_;
    torero::text::Manager *text_manager_;
    torero::gui::Manager *gui_manager_;
    torero::model::Manager *model_manager_;
    torero::vehicle::Manager *vehicle_manager_;

    // signals
    boost::signals2::signal<void ()> signal_updated_camera_, signal_updated_screen_;
    std::vector<boost::signals2::signal<void ()> > signal_draw_;
  };
}

#endif // TORERO_CORE_H
