#include "torero/core.h"
#include "torero/model_manager.h"
#include "torero/vehicle_manager.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  Core::Core(int argc, char **argv, const bool system_title) :
    MultiThreadManager(),
    argc_(argc),
    argv_(argv),
    window_(nullptr),
    width_(DEFAULT_WIDTH),
    height_(DEFAULT_HEIGHT),
    initial_width_(width_),
    initial_height_(height_),
    half_height_(DEFAULT_HEIGHT / 2),
    position_x_(0),
    position_y_(0),
    initial_position_x_(0),
    initial_position_y_(0),
    error_log_(0),
    error_(false),
    is_left_click_(false),
    is_right_click_(false),
    is_scroll_click_(false),
    window_moving_(false),
    old_x_(0),
    old_y_(0),
    is_inversed_(false),
    is_maximized_(false),
    maximization_(false),
    has_changed_(true),
    blocked_(false),
    is_window_paused_(false),
    frame_mover_(0),
    max_filtering_(0.0f),
    global_frame_(),
    vehicle_frame_(),
    vehicle_frame_yaw_(),
    navigation_frame_(),
    navigation_plus_frame_(),
    camera_(Algebraica::vec3f(-12.0f, 0.0f, 0.0f), Algebraica::vec3f(),
            Algebraica::vec3f(0.0f, 0.0f, 1.0f), &vehicle_frame_yaw_),
    vehicle_(nullptr),
    modeler_(nullptr),
    screen_conversor_(),
    cursor_normal_(nullptr),
    cursor_pointer_(nullptr),
    cursor_beam_(nullptr),
    cursor_cross_(nullptr),
    cursor_move_(nullptr),
    signal_draw_(Visualizer::DRAWING_ELEMENTS)
  {
    // glfw: initialize and configure
    // ------------------------------
    if(!glfwInit()){
      message_handler("GLFW initialization failed", Visualizer::Message::ERROR);
      error_log_ =  GLFW_NOT_LOADED;
      error_ = true;
    }else{
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_SAMPLES, 4);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_DECORATED, system_title);
      glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
      glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
      //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

      // glfw window creation
      // --------------------
      GLFWmonitor *monitor{glfwGetPrimaryMonitor()};

      const GLFWvidmode *screen{glfwGetVideoMode(monitor)};
      glfwWindowHint(GLFW_RED_BITS, screen->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, screen->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, screen->blueBits);

      window_ = glfwCreateWindow(width_, height_, "Torero", NULL, NULL);

      if(!window_){
        glfwTerminate();
        message_handler("GLFW failed creating a window", Visualizer::Message::ERROR);
        error_log_ = WINDOW_NOT_LOADED;
        error_ = true;
      }else{
        glfwGetMonitorPos(monitor, &position_x_, &position_y_);

        initial_position_x_ = position_x_ = position_x_ + (screen->width - width_)/2;
        initial_position_y_ = position_y_ = position_y_ + (screen->height - height_)/2;

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
          message_handler("Failed to initialize GLAD", Visualizer::Message::ERROR);
          error_log_ = GLAD_NOT_LOADED;
          error_ = true;
        }else{
          glfwSwapInterval(1);

          glfwSetFramebufferSizeCallback(window_, Core::callback_resize);
          glfwSetMouseButtonCallback(window_, Core::callback_mouse_click);
          glfwSetCursorPosCallback(window_, Core::callback_mouse_move);
          glfwSetScrollCallback(window_, Core::callback_mouse_scroll);
          glfwSetKeyCallback(window_, Core::callback_key_callback);

          signal_window_resize.connect(boost::bind(&Core::resize, this, _1, _2));
          signal_mouse_click.connect(boost::bind(&Core::event_mouse_click, this, _1, _2, _3));
          signal_mouse_move.connect(boost::bind(&Core::event_mouse_move, this, _1, _2));
          signal_mouse_scroll.connect(boost::bind(&Core::event_mouse_scroll, this, _1));
          signal_key_callback.connect(boost::bind(&Core::event_key_callback, this, _1, _2, _3, _4));

          initialize();
        }
      }
    }
  }

  boost::signals2::signal<void (int, int)> Core::signal_window_resize;
  boost::signals2::signal<void (int, int, int)> Core::signal_mouse_click;
  boost::signals2::signal<void (double, double)> Core::signal_mouse_move;
  boost::signals2::signal<void (double)> Core::signal_mouse_scroll;
  boost::signals2::signal<void (int, int, int, int)> Core::signal_key_callback;

  Core::~Core(){
    if(window_){
      glfwDestroyCursor(cursor_normal_);
      glfwDestroyCursor(cursor_pointer_);
      glfwDestroyCursor(cursor_beam_);
      glfwDestroyCursor(cursor_cross_);
      glfwDestroyCursor(cursor_move_);

      glfwDestroyWindow(window_);
      glfwTerminate();
    }
  }

  bool *Core::camera_blocker(){
    return &blocked_;
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective(){
    return camera_.perspective_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_inversed(){
    return camera_.inversed_perspective_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_view(){
    return camera_.pv_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_view_inversed(){
    return camera_.inversed_pv_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_view_static(){
    return camera_.static_pv_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_perspective_view_static_inversed(){
    return camera_.inversed_static_pv_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_view(){
    return camera_.view_matrix();
  }

  const Algebraica::mat4f &Core::camera_matrix_view_inversed(){
    return camera_.inversed_view_matrix();
  }

  const Algebraica::vec3f &Core::camera_position(){
    return camera_.camera_position();
  }

  const Algebraica::vec3f &Core::camera_relative_position(){
    return camera_.relative_camera_position();
  }

  void Core::camera_rotate(const Algebraica::quaternionF quaternion){
    Algebraica::quaternionF corrected_rotation(-quaternion[1], quaternion[2],
                                               -quaternion[0], quaternion[3]);
    camera_.rotate_camera(corrected_rotation);
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

  void Core::camera_set_zoom(const float zoom){
    camera_.set_zoom(zoom);
  }

  void Core::camera_translate(const float x, const float y, const float z){
    camera_.translate_camera(-y, z, -x);
  }

  void Core::camera_update(){
    camera_.update_view();
  }

  void Core::camera_view_isometric(){
    camera_.isometric_view();
  }

  void Core::camera_view_top(){
    camera_.top_view();
  }

  void Core::camera_zoom_in(){
    camera_.zooming();
  }

  void Core::camera_zoom_out(){
    camera_.zooming(false);
  }

  void Core::close(){
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }

  bool Core::closing(){
    return glfwWindowShouldClose(window_);
  }

  int Core::execute(const bool infinite_loop){
    if(!error_){
      glfwShowWindow(window_);
      if(infinite_loop)
        while(!glfwWindowShouldClose(window_)){

          if(glfwGetWindowAttrib(window_, GLFW_VISIBLE) && has_changed_){
            paint();
            has_changed_ = false;

            glfwSwapBuffers(window_);
          }
          if(!multithread_finished()){
            multithread_update_process();
            glfwWaitEventsTimeout(FREQUENCY);
          }else
            glfwWaitEvents();
        }
      return EXIT_SUCCESS;
    }else{
      return error_log_;
    }
  }

  int Core::execute(const int width, const int height, const std::string title,
                    const bool full_screen, const bool maximized, const bool infinite_loop){
    if(!error_){
      is_maximized_ = maximized;

      glfwSetWindowTitle(window_, title.c_str());

      const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
      initial_width_ = width_ = (width <= 0)? screen->width : width;
      initial_height_ = height_ = (height <= 0)? screen->height : height;

      initial_position_x_ = position_x_ = (screen->width - width_)/2;
      initial_position_y_ = (screen->height - height_)/2;

      if(full_screen){
        glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                             width_, height_, screen->refreshRate);
      }else if(maximized){
        glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                             screen->width, screen->height, screen->refreshRate);
        maximization_ = true;
      }else
        glfwSetWindowMonitor(window_, NULL, 0, 0, width_, height_, 0);

      int posx;
      glfwGetWindowPos(window_, &posx, &position_y_);

      initial_position_y_ = position_y_ = position_y_ + initial_position_y_;

      glfwSetWindowPos(window_, position_x_, position_y_);

      execute(infinite_loop);
    }else{
      return error_log_;
    }
  }

  const Algebraica::mat4f *Core::frame_global() const{
    return &global_frame_;
  }

  const Algebraica::mat4f *Core::frame_navigation() const{
    return &navigation_frame_;
  }

  const Algebraica::mat4f *Core::frame_navigation_plus() const{
    return &navigation_plus_frame_;
  }

  const Algebraica::mat4f *Core::frame_vehicle() const{
    return &vehicle_frame_;
  }

  const Algebraica::mat4f *Core::frame_vehicle_yaw() const{
    return &vehicle_frame_yaw_;
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

  const double Core::get_time(){
    return glfwGetTime();
  }

  const int *Core::height() const{
    return &height_;
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
    // Avoiding the rendering of all back faces
    glCullFace(GL_BACK);

    cursor_normal_ = glfwCreateStandardCursor(GLFW_CURSOR_NORMAL);
    cursor_pointer_ = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    cursor_beam_ = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    cursor_cross_ = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    cursor_move_ = load_mouse_icon("resources/images/mouse_move.png", 12, 12);

    if(!cursor_move_) cursor_move_ = glfwCreateStandardCursor(GLFW_CURSOR_NORMAL);

    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    camera_.set_resolution(window_width, window_height, width_, height_);
    camera_.set_function_callback(boost::bind(&Core::updated_camera, this));

    screen_conversor_.set_vehicle_frame(&vehicle_frame_);
    screen_conversor_.set_perspective_matrix(&camera_.perspective_matrix());
    screen_conversor_.set_view_matrix(&camera_.view_matrix());
//    screen_conversor_.set_screen_size(&width_, &height_);
  }

  GLFWcursor *Core::load_mouse_icon(const std::string path, const int x_offset, const int y_offset){
    GLFWcursor *cursor{nullptr};
    GLFWimage mouse_icon;
    int components;

    mouse_icon.pixels = stbi_load(path.c_str(), &mouse_icon.width,
                                  &mouse_icon.height, &components, 0);
    if(mouse_icon.pixels)
      cursor = glfwCreateCursor(&mouse_icon, x_offset, y_offset);

    stbi_image_free(mouse_icon.pixels);

    return cursor;
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

  void Core::maximize(){
    if(is_maximized_){
      glfwSetWindowMonitor(window_, NULL, initial_position_x_, initial_position_y_,
                           initial_width_, initial_height_, GLFW_DONT_CARE);
//      glfwRestoreWindow(window_);
    }else{
      const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
      glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                           screen->width, screen->height, screen->refreshRate);
      maximization_ = true;
//      glfwMaximizeWindow(window_);
    }
    is_maximized_ = !is_maximized_;
  }

  void Core::message_handler(const std::string text, const Visualizer::Message message_type){
    switch(message_type){
    case Visualizer::Message::ERROR:
      std::cout << "\n\033[1;41m Error: \033[0;1;38;5;174m ";
      break;
    case Visualizer::Message::WARNING:
      std::cout << "\n\033[1;30;48;5;11m Warning: \033[0;1;38;5;229m ";
      break;
    case Visualizer::Message::ATTENTION:
      std::cout << "\n\033[1;30;46m Attention: \033[0;1;38;5;195m ";
      break;
    default:
      std::cout << "\n\033[1;30;42m Message received: \033[0;1;38;5;193m ";
      break;
    }
    std::cout << text << "\033[0m\n" << std::endl;
  }

  void Core::minimize(const bool minimized){
    if(minimized)
      glfwIconifyWindow(window_);
    else
      glfwRestoreWindow(window_);
  }

  const int Core::move_frame(){
    const int action{frame_mover_};
    frame_mover_ = 0;
    return action;
  }

  bool *Core::mover(){
    return &window_moving_;
  }

  void Core::paint(){
    //clearing the screen of old information
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    signal_updated_screen_();

    for(int i = 0; i < Visualizer::DRAWING_ELEMENTS; ++i)
      signal_draw_[i]();
  }

  const bool Core::paused(){
    return is_window_paused_;
  }

  void Core::process_pending_events(){
    if(!multithread_finished()){
      multithread_update_process();
    }

    glfwPollEvents();
  }

  void Core::resize(const int width, const int height){
    glViewport(0, 0, width, height);
    width_ = width;
    height_ = height;

    if(maximization_){
      maximization_ = false;
    }else{
      initial_width_ = width_;
      initial_height_ = height_;
    }

    half_height_ = height / 2;

    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    camera_.set_resolution(window_width, window_height, width_, height_);

    has_changed_ = true;

    signal_resized_screen_(width, height);
  }

  void Core::restart_viewport(){
    glViewport(0, 0, width_, height_);
  }

  bool *Core::screen_changer(){
    return &has_changed_;
  }

  ScreenConversor *Core::screen_conversor(){
    return &screen_conversor_;
  }

  const GLfloat Core::screen_max_anisotropic_filtering(){
    return max_filtering_;
  }

  void Core::screen_paint(){
    paint();
  }

  void Core::screen_redraw(){
    paint();
    glfwSwapBuffers(window_);
  }

  void Core::screen_swap_buffers(){
    glfwSwapBuffers(window_);
  }

  void Core::set_cursor(const Visualizer::Cursor type){
    switch(type){
    case Visualizer::Cursor::HAND:
      glfwSetCursor(window_, cursor_pointer_);
    break;
    case Visualizer::Cursor::TEXT:
      glfwSetCursor(window_, cursor_beam_);
    break;
    case Visualizer::Cursor::CROSSHAIR:
      glfwSetCursor(window_, cursor_cross_);
    break;
    case Visualizer::Cursor::MOVE:
      glfwSetCursor(window_, cursor_move_);
    break;
    case Visualizer::Cursor::HIDDEN:
      glfwSetCursor(window_, NULL);
    break;
    default:
      glfwSetCursor(window_, cursor_normal_);
    break;
    }
  }

  bool Core::set_module(ModelManager *model_manager){
    const bool existing{modeler_};
    modeler_ = model_manager;
    return existing;
  }

  bool Core::set_module(VehicleManager *vehicle_manager){
    const bool existing{vehicle_};
    vehicle_ = vehicle_manager;

    vehicle_->set_vehicle_frame(&vehicle_frame_);
    vehicle_->set_vehicle_frame_yaw(&vehicle_frame_yaw_);
    vehicle_->set_navigation_frame(&navigation_frame_);
    vehicle_->set_navigation_plus_frame(&navigation_plus_frame_);

    return existing;
  }

  void Core::set_position(const int x, const int y){
    position_x_ = x;
    position_y_ = y;
    glfwSetWindowPos(window_, x, y);
  }

  void Core::set_title(const std::string title){
    glfwSetWindowTitle(window_, title.c_str());
  }

  void Core::set_size(const int width, const int height){
    const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
    initial_width_ = width_ = (width <= 0)? screen->width : width;
    initial_height_ = height_ = (height <= 0)? screen->height : height;

    initial_position_x_ = position_x_ = (screen->width - width_)/2;
    initial_position_y_ = position_y_ = (screen->height - height_)/2;

    glfwSetWindowSize(window_, width_, height_);
  }

  bool Core::visibility(){
    return glfwGetWindowAttrib(window_, GLFW_VISIBLE);
  }

  void Core::wait_for_events(){
    glfwWaitEvents();
  }

  void Core::wait_for_events(const double timeout){
    glfwWaitEventsTimeout(timeout);
  }

  const int *Core::width() const{
    return &width_;
  }

  boost::signals2::signal<void ()> *Core::syncronize(const Visualizer::Order object){
    return &signal_draw_[object];
  }

  boost::signals2::signal<void ()> *Core::signal_updated_camera(){
    return &signal_updated_camera_;
  }

  boost::signals2::signal<void ()> *Core::signal_updated_screen(){
    return &signal_updated_screen_;
  }

  boost::signals2::signal<void (int, int)> *Core::signal_resized_screen(){
    return &signal_resized_screen_;
  }

  boost::signals2::signal<void (int, int)> *Core::signal_moved_mouse(){
    return &signal_mouse_moved_;
  }

  boost::signals2::signal<void (int, int, int)> *Core::signal_clicked_mouse(){
    return &signal_clicked_mouse_;
  }

  boost::signals2::signal<void (int, int, int)> *Core::signal_released_mouse(){
    return &signal_released_mouse_;
  }

  void Core::callback_resize(GLFWwindow *window, int width, int height){
    Toreo::Core::signal_window_resize(width, height);
  }

  void Core::callback_mouse_click(GLFWwindow *window, int button, int action, int mods){
    Toreo::Core::signal_mouse_click(button, action, mods);
  }

  void Core::callback_mouse_move(GLFWwindow *window, double xpos, double ypos){
    Toreo::Core::signal_mouse_move(xpos, ypos);
  }

  void Core::callback_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset){
    Toreo::Core::signal_mouse_scroll(yoffset);
  }

  void Core::callback_key_callback(GLFWwindow *window, int key,
                                   int scancode, int action, int mods){
    Toreo::Core::signal_key_callback(key, scancode, action, mods);
  }

  void Core::event_key_callback(int key, int scancode, int action, int mods){
    if(action == GLFW_RELEASE || action == GLFW_REPEAT){
      if(key == GLFW_KEY_SPACE)
        is_window_paused_ = !is_window_paused_;
      else if(key == GLFW_KEY_LEFT)
        frame_mover_ = -1;
      else if(key == GLFW_KEY_RIGHT)
        frame_mover_ = 1;
    }
  }

  void Core::event_mouse_click(int button, int action, int mods){
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
    old_x_ = std::floor(posx);
    old_y_ = std::floor(posy);

    is_inversed_ = (old_y_ > half_height_)? true : false;

    if(action == GLFW_PRESS)
      signal_clicked_mouse_(old_x_, height_ - old_y_, mods);
    else if(action == GLFW_RELEASE){
      window_moving_ = false;
      signal_released_mouse_(old_x_, height_ - old_y_, mods);
    }
  }

  void Core::event_mouse_move(double xpos, double ypos){
    const int x_pos{INT(std::floor(xpos))};
    const int y_pos{INT(std::floor(ypos))};

    if((is_left_click_ || is_right_click_) && !blocked_){
      const int dx = (is_inversed_ && is_left_click_)? -x_pos + old_x_ : x_pos - old_x_;
      const int dy = (is_inversed_ && is_left_click_)? -y_pos + old_y_ : y_pos - old_y_;

      camera_.modify_camera(dx, dy, is_left_click_);

      old_x_ = x_pos;
      old_y_ = y_pos;

      has_changed_ = true;
    }

    if(is_left_click_ && window_moving_){
      glfwGetWindowPos(window_, &position_x_, &position_y_);

      position_x_ += x_pos - old_x_;
      position_y_ += y_pos - old_y_;

      glfwSetWindowPos(window_, position_x_, position_y_);
    }
    signal_mouse_moved_(x_pos, height_ - y_pos);
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
}
