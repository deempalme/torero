#include "include/core.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  Core::Core(int _argc, char **_argv) :
    argc_(_argc),
    argv_(_argv),
    window_(nullptr),
    width_(DEFAULT_WIDTH),
    height_(DEFAULT_HEIGHT),
    half_height_(DEFAULT_HEIGHT / 2),
    position_x_(0),
    position_y_(0),
    error_log_(0),
    error_(false),
    is_left_click_(false),
    is_right_click_(false),
    is_scroll_click_(false),
    old_x_(0),
    old_y_(0),
    is_inversed_(false),
    has_changed_(true),
    max_filtering_(0.0f),
    identity_matrix_(),
    fixed_frame_(&identity_matrix_),
    vehicle_frame_(&identity_matrix_),
    navigation_frame_(&identity_matrix_),
    camera_(Algebraica::vec3f(-12.0f, 0.0f, 5.0f), Algebraica::vec3f(),
            Algebraica::vec3f(0.0f, 0.0f, 1.0f), vehicle_frame_),
    signal_draw_(9)
  {
    // glfw: initialize and configure
    // ------------------------------
    if(!glfwInit()){
      message_handler("GLFW initialization failed", ERROR_MESSAGE);
      error_log_ =  GLFW_NOT_LOADED;
      error_ = true;
    }else{
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_SAMPLES, 4);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
      glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
      //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

      // glfw window creation
      // --------------------
      const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
      glfwWindowHint(GLFW_RED_BITS, screen->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, screen->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, screen->blueBits);
      glfwWindowHint(GLFW_REFRESH_RATE, screen->refreshRate);

      window_ = glfwCreateWindow(width_, height_, "Toreo", NULL, NULL);

      if(!window_){
        glfwTerminate();
        message_handler("GLFW failed creating a window", ERROR_MESSAGE);
        error_log_ = WINDOW_NOT_LOADED;
        error_ = true;
      }else{
        position_x_ = (screen->width - width_)/2;
        position_y_ = (screen->height - height_)/2;
        glfwSetWindowPos(window_, position_x_, position_y_);

        // ------------------------------------------------------------------------------------ //
        // ------------------------------- Loading window's icon ------------------------------ //
        // ------------------------------------------------------------------------------------ //
        load_window_icon();

        glfwMakeContextCurrent(window_);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
          glfwTerminate();
          message_handler("Failed to initialize GLAD", ERROR_MESSAGE);
          error_log_ = GLAD_NOT_LOADED;
          error_ = true;
        }else{
          glfwSwapInterval(1);

          glfwSetFramebufferSizeCallback(window_, callback_resize);
          glfwSetMouseButtonCallback(window_, callback_mouse_click);
          glfwSetCursorPosCallback(window_, callback_mouse_move);
          glfwSetScrollCallback(window_, callback_mouse_scroll);

          signal_window_resize.connect(boost::bind(&Core::resize, this, _1, _2));
          signal_mouse_click.connect(boost::bind(&Core::event_mouse_click, this, _1, _2));
          signal_mouse_move.connect(boost::bind(&Core::event_mouse_move, this, _1, _2));
          signal_mouse_scroll.connect(boost::bind(&Core::event_mouse_scroll, this, _1));

          initialize();
        }
      }
    }
  }

  boost::signals2::signal<void (int, int)> Core::signal_window_resize;
  boost::signals2::signal<void (int, int)> Core::signal_mouse_click;
  boost::signals2::signal<void (double, double)> Core::signal_mouse_move;
  boost::signals2::signal<void (double)> Core::signal_mouse_scroll;

  Core::~Core(){
    if(window_){
      glfwDestroyWindow(window_);
      glfwTerminate();
    }
  }

  void Core::camera_set_position(const float x, const float y, const float z){
    camera_.set_position(-y, z, -x);
  }

  void Core::camera_set_target(const float x, const float y, const float z){
    camera_.set_target(-y, z, -x);
  }

  void Core::camera_set_up(const float x, const float y, const float z){
    camera_.set_up(-y, z, -x);
  }

  void Core::camera_rotate(const float pitch, const float yaw, const float roll){
    camera_.rotate_camera(-pitch, yaw, -roll);
  }

  void Core::camera_translate(const float x, const float y, const float z){
    camera_.translate_camera(-y, z, -x);
  }

  void Core::camera_set_zoom(const float zoom){
    camera_.set_zoom(zoom);
  }

  void Core::camera_top_view(){
    camera_.top_view();
  }

  void Core::camera_isometric_view(){
    camera_.isometric_view();
  }

  const Algebraica::mat4f &Core::camera_matrix_view(){
    return camera_.view_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective(){
    return camera_.perspective_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_view(){
    return camera_.pv_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_static_perspective_view(){
    return camera_.static_pv_matrix();
  }

  const Algebraica::vec3f &Core::camera_position(){
    return camera_.camera_position();
  }

  const Algebraica::mat4f *Core::fixed_frame() const{
    return fixed_frame_;
  }

  const Algebraica::mat4f *Core::vehicle_frame() const{
    return vehicle_frame_;
  }

  const Algebraica::mat4f *Core::navigation_frame() const{
    return navigation_frame_;
  }

  void Core::set_window_title(const char *title){
    glfwSetWindowTitle(window_, title);
  }

  void Core::set_window_title(const std::string title){
    glfwSetWindowTitle(window_, title.c_str());
  }

  void Core::set_window_size(const int width, const int height){
    const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
    width_ = (width <= 0)? screen->width : width;
    height_ = (height <= 0)? screen->height : height;

    glfwSetWindowSize(window_, width_, height_);
  }

  void Core::set_window_position(const int x, const int y){
    position_x_ = x;
    position_y_ = y;
    glfwSetWindowPos(window_, x, y);
  }

  void Core::maximize_window(const bool maximized){
    if(maximized)
      glfwMaximizeWindow(window_);
    else
      glfwRestoreWindow(window_);
  }

  void Core::minimize_window(const bool minimized){
    if(minimized)
      glfwIconifyWindow(window_);
    else
      glfwRestoreWindow(window_);
  }

  void Core::full_screen(const bool make_full){
    if(make_full){
      const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
      glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                           width_, height_, screen->refreshRate);
    }else
      glfwSetWindowMonitor(window_, NULL, position_x_, position_y_,
                           width_, height_, GLFW_DONT_CARE);
  }

  void Core::redraw_screen(){
    paint();
  }

  void Core::restart_viewport(){
    glViewport(0, 0, width_, height_);
  }

  void Core::swap_buffers(){
    glfwSwapBuffers(window_);
  }

  void Core::wait_for_events(){
    glfwWaitEvents();
  }

  void Core::wait_for_events(const double timeout){
    glfwWaitEventsTimeout(timeout);
  }

  void Core::process_pending_events(){
    glfwPollEvents();
  }

  bool Core::window_visibility(){
    return glfwGetWindowAttrib(window_, GLFW_VISIBLE);
  }

  bool Core::window_closing(){
    return glfwWindowShouldClose(window_);
  }

  int Core::execute(const bool infinite_loop){
    if(!error_){
      glfwShowWindow(window_);
      if(infinite_loop)
        while(!glfwWindowShouldClose(window_)){

          if(glfwGetWindowAttrib(window_, GLFW_VISIBLE)){
            paint();

            glfwSwapBuffers(window_);
          }
          //glfwWaitEventsTimeout(0.1);
          glfwWaitEvents();
          //glfwPollEvents();
        }
      return EXIT_SUCCESS;
    }else{
      return error_log_;
    }
  }

  int Core::execute(const int width, const int height, const char *title,
                    const bool full_screen, const bool maximized, const bool infinite_loop){
    if(!error_){
      if(maximized)
        glfwMaximizeWindow(window_);

      glfwSetWindowTitle(window_, title);

      const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
      width_ = (width <= 0)? screen->width : width;
      height_ = (height <= 0)? screen->height : height;

      if(full_screen)
        glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                             width_, height_, screen->refreshRate);
      else
        glfwSetWindowSize(window_, width_, height_);

      glfwSetWindowPos(window_, (screen->width - width_)/2, (screen->height - height_)/2);

      execute(infinite_loop);
    }else{
      return error_log_;
    }
  }

  GLFWwindow *Core::get_window(){
    return window_;
  }

  const GLfloat Core::max_anisotropic_filtering(){
    return max_filtering_;
  }

  boost::signals2::signal<void ()> *Core::syncronize(Visualizer::Order object){
    return &signal_draw_.at(object);
  }

  boost::signals2::signal<void ()> *Core::signal_updated_camera(){
    return &signal_updated_camera_;
  }

  boost::signals2::signal<void ()> *Core::signal_updated_screen(){
    return &signal_updated_screen_;
  }

  void Core::message_handler(const std::string text, const int message_type){
    switch(message_type){
    case ERROR_MESSAGE:
      std::cout << "Error: ";
      break;
    case WARNING_MESSAGE:
      std::cout << "Warning: ";
      break;
    case ATTENTION_MESSAGE:
      std::cout << "Attention: ";
      break;
    default:
      std::cout << "Message received: ";
      break;
    }
    std::cout << text << std::endl;
  }

  void Core::initialize(){
    // configure global opengl state
    // -----------------------------
    // setting the background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // this line allows anti-aliasing to create fine edges
    glEnable(GL_MULTISAMPLE);
    // this line allows z-buffer to avoid rear objects to appear in front
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // these allow alpha transparency in the rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // detects the maximum anisotropic filtering samples
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_filtering_);
    max_filtering_ = (max_filtering_ > 8.0f)? 8.0f : max_filtering_;

    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    camera_.set_resolution(window_width, window_height, width_, height_);
    camera_.set_function_callback(boost::bind(&Core::updated_camera, this));
  }

  void Core::paint(){
    //clearing the screen of old information
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    signal_updated_screen_();

    for(int i = 0; i < 9; ++i)
      signal_draw_.at(i)();
  }

  void Core::resize(const int width, const int height){
    glViewport(0, 0, width, height);
    width_ = width;
    height_ = height;
    half_height_ = height / 2;

    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    camera_.set_resolution(window_width, window_height, width_, height_);

    has_changed_ = true;
  }

  void Core::event_mouse_click(int button, int action){
    if(button == GLFW_MOUSE_BUTTON_1)
      if(action == GLFW_PRESS)
        is_left_click_ = true;
      else
        is_left_click_ = false;
    if(button == GLFW_MOUSE_BUTTON_2)
      if(action == GLFW_PRESS)
        is_right_click_ = true;
      else
        is_right_click_ = false;
    if(button == GLFW_MOUSE_BUTTON_3)
      if(action == GLFW_PRESS)
        is_scroll_click_ = true;
      else
        is_scroll_click_ = false;

    double posx, posy;
    glfwGetCursorPos(window_, &posx, &posy);
    old_x_ = floor(posx);
    old_y_ = floor(posy);

    is_inversed_ = (old_y_ < half_height_)? true : false;
  }

  void Core::event_mouse_move(double xpos, double ypos){
    if(is_left_click_ || is_right_click_){
      int dx = (is_inversed_ && is_left_click_)? -xpos + old_x_ : xpos - old_x_;
      int dy = (is_inversed_ && is_left_click_)? -ypos + old_y_ : ypos - old_y_;

      camera_.modify_camera(dx, dy, is_left_click_);

      old_x_ = xpos;
      old_y_ = ypos;

      has_changed_ = true;
    }
  }

  void Core::event_mouse_scroll(double yoffset){
    if(yoffset > 0.0)
      camera_.zooming();
    else
      camera_.zooming(false);
    has_changed_ = true;
  }

  void Core::updated_camera(){
    signal_updated_camera_();
  }

  void Core::load_window_icon(){
    GLFWimage icon;
    int components_size;

    icon.pixels = stbi_load("resources/images/icon.png", &icon.width, &icon.height,
                            &components_size, 0);
    if(icon.pixels)
      glfwSetWindowIcon(window_, 1, &icon);

    stbi_image_free(icon.pixels);
  }
}

void callback_resize(GLFWwindow *window, int width, int height){
  Toreo::Core::signal_window_resize(width, height);
}

void callback_mouse_click(GLFWwindow *window, int button, int action, int mods){
  Toreo::Core::signal_mouse_click(button, action);
}

void callback_mouse_move(GLFWwindow *window, double xpos, double ypos){
  Toreo::Core::signal_mouse_move(xpos, ypos);
}

void callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset){
  Toreo::Core::signal_mouse_scroll(yoffset);
}
