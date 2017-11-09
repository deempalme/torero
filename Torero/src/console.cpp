#include "includes/console.h"
#include "includes/skybox.h"
#include "includes/three_dimensional_model_loader.h"

Console::Console(int _argc, char **_argv) :
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
  gui_(nullptr),
  skybox_(nullptr),
  max_filtering_(0.0f),
  textures_id_(0),
  fixed_frame_(),
  vehicle_frame_(),
  navigation_frame_(),
  camera_(Algebraica::vec3f(-12.0f, 0.0f, 5.0f), Algebraica::vec3f(),
          Algebraica::vec3f(0.0f, 0.0f, 1.0f), &vehicle_frame_)
{
  if(!glfwInit()){
    message_handler("GLFW initialization failed", ERROR_MESSAGE);
    error_log_ =  GLFW_NOT_LOADED;
    error_ = true;
  }else{

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

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
      load_icon();

      glfwMakeContextCurrent(window_);
      if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        glfwTerminate();
        message_handler("Failed to initialize GLAD", ERROR_MESSAGE);
        error_log_ = GLAD_NOT_LOADED;
        error_ = true;
      }else{
        glfwSwapInterval(1);

        resize_signal.connect(boost::bind(&Console::resizeGL, this, _1, _2));
        mouse_move_signal.connect(boost::bind(&Console::mouse_move_event, this, _1, _2));
        mouse_click_signal.connect(boost::bind(&Console::mouse_click_event, this, _1, _2));
        mouse_scroll_signal.connect(boost::bind(&Console::mouse_scroll_event, this, _1));

        initializeGL();
        glfwSetFramebufferSizeCallback(window_, resize_callback);
        glfwSetCursorPosCallback(window_, mouse_move_callback);
        glfwSetMouseButtonCallback(window_, mouse_click_callback);
        glfwSetScrollCallback(window_, mouse_scroll_callback);
      }
    }
  }
}

Console::~Console(){
  if(window_){
    for(Visualizer::Model3D model : models_)
      if(model.model)
        delete model.model;

    if(skybox_)
      delete skybox_;

    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

void Console::camera_set_position(const float x, const float y, const float z){
  camera_.set_position(x, y, z);
}

void Console::camera_set_target(const float x, const float y, const float z){
  camera_.set_target(x, y, z);
}

void Console::camera_set_up(const float x, const float y, const float z){
  camera_.set_up(x, y, z);
}

void Console::camera_rotate(const float pitch, const float yaw, const float roll){
  camera_.rotate_camera(pitch, yaw, roll);
}

void Console::camera_translate(const float x, const float y, const float z){
  camera_.translate_camera(x, y, z);
}

void Console::camera_set_zoom(const float zoom){
  camera_.set_zoom(zoom);
}

void Console::camera_top_view(){
  camera_.top_view();
}

void Console::camera_isometric_view(){
  camera_.isometric_view();
}

const Algebraica::mat4f &Console::view_matrix(){
  return camera_.view_matrix();
}

const Algebraica::mat4f &Console::perspective_matrix(){
  return camera_.perspective_matrix();
}

const Algebraica::mat4f &Console::pv_matrix(){
  return camera_.pv_matrix();
}

const Algebraica::mat4f &Console::static_pv_matrix(){
  return camera_.static_pv_matrix();
}

const Algebraica::mat4f *Console::fixed_frame(){
  return &fixed_frame_;
}

const Algebraica::mat4f *Console::vehicle_frame(){
  return &vehicle_frame_;
}

const Algebraica::mat4f *Console::navigation_frame(){
  return &navigation_frame_;
}

void Console::set_window_title(const char *title){
  glfwSetWindowTitle(window_, title);
}

void Console::set_window_title(const std::string title){
  glfwSetWindowTitle(window_, title.c_str());
}

void Console::set_window_size(const int width, const int height){
  const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
  width_ = (width <= 0)? screen->width : width;
  height_ = (height <= 0)? screen->height : height;

  glfwSetWindowSize(window_, width_, height_);
}

void Console::set_window_position(const int x, const int y){
  position_x_ = x;
  position_y_ = y;
  glfwSetWindowPos(window_, x, y);
}

void Console::maximize_window(const bool maximized){
  if(maximized)
    glfwMaximizeWindow(window_);
  else
    glfwRestoreWindow(window_);
}

void Console::minimize_window(const bool minimized){
  if(minimized)
    glfwIconifyWindow(window_);
  else
    glfwRestoreWindow(window_);
}

void Console::full_screen(const bool make_full){
  if(make_full){
    const GLFWvidmode *screen{glfwGetVideoMode(glfwGetPrimaryMonitor())};
    glfwSetWindowMonitor(window_, glfwGetPrimaryMonitor(), 0, 0,
                         width_, height_, screen->refreshRate);
  }else
    glfwSetWindowMonitor(window_, NULL, position_x_, position_y_,
                         width_, height_, GLFW_DONT_CARE);
}

void Console::redraw_screen(){
  paintGL();
}

void Console::wait_for_events(){
  glfwWaitEvents();
}

void Console::wait_for_events(const double timeout){
  glfwWaitEventsTimeout(timeout);
}

void Console::process_pending_events(){
  glfwPollEvents();
}

int Console::execute(const bool infinite_loop){
  if(!error_){
    glfwShowWindow(window_);
    if(infinite_loop)
      while(!glfwWindowShouldClose(window_)){

        if(glfwGetWindowAttrib(window_, GLFW_VISIBLE)){
          paintGL();
        }

        glfwSwapBuffers(window_);
        glfwWaitEventsTimeout(0.1);
        //glfwPollEvents();
      }
    return EXIT_SUCCESS;
  }else{
    return error_log_;
  }
}

int Console::execute(const int width, const int height, const char* title,
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

boost::signals2::signal<void ()> *Console::updated_camera_signal(){
  return &updated_camera_signal_;
}

boost::signals2::signal<void ()> *Console::updated_screen_signal(){
  return &updated_screen_signal_;
}

void Console::message_handler(const std::string text, const int message_type){
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

void Console::model_ready(ThreeDimensionalModelLoader *model){
  std::cout << "Model ready... " << model->is_ready() << std::endl;
}

boost::signals2::signal<void (int, int)> Console::resize_signal;
boost::signals2::signal<void (double, double)> Console::mouse_move_signal;
boost::signals2::signal<void (int, int)> Console::mouse_click_signal;
boost::signals2::signal<void (double)> Console::mouse_scroll_signal;

void Console::initializeGL(){
  // setting the background color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // this line allows anti-aliasing to create fine edges
  glEnable(GL_MULTISAMPLE);
  // this line allows z-buffer to avoid rear objects to appear in front
  glEnable(GL_DEPTH_TEST);
  // this allows textures
  glEnable(GL_TEXTURE_2D);
  // these allow alpha transparency in the rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // detects the maximum anisotropic filtering samples
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_filtering_);
  max_filtering_ = (max_filtering_ > 8)? 8 : max_filtering_;

  /*
  ThreeDimensionalModelLoader::set_console(this);
  ThreeDimensionalModelLoader::set_window(window_);
  ThreeDimensionalModelLoader::set_shader_program(shaderProgram);
  ThreeDimensionalModelLoader *tire = new ThreeDimensionalModelLoader(TIRE);
  Model3D model{tire, mat4f()};
  models_.push_back(model);
  */

//  skybox_ = new Skybox("resources/cubemap/", ".png", &matrix_projection_and_static_view_, this, window_);

  int window_width, window_height;
  glfwGetWindowSize(window_, &window_width, &window_height);
  camera_.set_resolution(window_width, window_height, width_, height_);
  camera_.set_function_callback(boost::bind(&Console::updated_camera, this));
}

void Console::paintGL(){
  //clearing the screen of old information
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(skybox_){
    skybox_->draw();
  }

  updated_screen_signal_();
}

void Console::resizeGL(const int width, const int height){
  glViewport(0, 0, width, height);
  width_ = width;
  height_ = height;
  half_height_ = height / 2;

  int window_width, window_height;
  glfwGetWindowSize(window_, &window_width, &window_height);
  camera_.set_resolution(window_width, window_height, width_, height_);
}

void Console::mouse_click_event(int button, int action){
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

void Console::mouse_move_event(double xpos, double ypos){
  if(is_left_click_ || is_right_click_){
    int dx = (is_inversed_ && is_left_click_)? -xpos + old_x_ : xpos - old_x_;
    int dy = (is_inversed_ && is_left_click_)? -ypos + old_y_ : ypos - old_y_;

    camera_.modify_camera(dx, dy, is_left_click_);

    old_x_ = xpos;
    old_y_ = ypos;
  }
}

void Console::mouse_scroll_event(double yoffset){
  if(yoffset > 0.0)
    camera_.zooming();
  else
    camera_.zooming(false);
}

void Console::updated_camera(){
  updated_camera_signal_();
}

void Console::load_icon(){
  GLFWimage icon;
  int components_size;

  icon.pixels = stbi_load("resources/images/icon.png", &icon.width, &icon.height,
                          &components_size, 0);

  glfwSetWindowIcon(window_, 1, &icon);

  stbi_image_free(icon.pixels);
}

void resize_callback(GLFWwindow *window, int width, int height){
  Console::resize_signal(width, height);
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods){
  Console::mouse_click_signal(button, action);
}

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos){
  Console::mouse_move_signal(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset){
  Console::mouse_scroll_signal(yoffset);
}
