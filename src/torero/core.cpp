#include "torero/core.h"
#include "torero/camera/view.h"
#include "torero/gui/controller.h"
#include "torero/gui/manager.h"
#include "torero/file/manager.h"
#include "torero/image/manager.h"
#include "torero/model/environment.h"
#include "torero/model/manager.h"
#include "torero/terminal/printer.h"
#include "torero/text/manager.h"
#include "torero/vehicle/manager.h"
// Image loader
#include "stb_image.h"

namespace torero {
  Core::Core(int /*argc*/, char** /*argv*/, const bool system_title) :
    torero::gui::Cursors(this),
    torero::event::Handler(this),
    torero::thread::Manager(),
    window_(nullptr),
    context_(nullptr),
    title_(),
    error_(false),
    error_log_(0),
    max_filtering_(0.0f),
    identity_matrix_(),
    camera_(new torero::camera::View()),
    controller_(new torero::gui::Controller(this)),
    environment_(nullptr),
    image_manager_(new torero::image::Manager(this)),
    text_manager_(nullptr),
    gui_manager_(nullptr),
    model_manager_(nullptr),
    vehicle_manager_(nullptr),
    signal_draw_(torero::k_drawing_elements)
  {
    // Avoiding main() redefinition
    SDL_SetMainReady();
    // SDL: initialize and configure
    // ------------------------------
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
      torero::terminal::Printer::error("SDL initialization failed");
      error_log_ =  torero::WindowErrors::SDL2_NotLoaded;
      error_ = true;
    }else{
      SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
      SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, SDL_TRUE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

      // SDL window creation
      // --------------------
      const int number_of_monitors{SDL_GetNumVideoDisplays()};
      SDL_DisplayMode monitor_mode;
      int mouse_x, mouse_y;
      SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

      for(int i = 0; i < number_of_monitors; ++i){
        if(SDL_GetDisplayBounds(i, &display_properties_) != 0) sdl_error();

        if(mouse_x > display_properties_.x
           && mouse_x < (display_properties_.x + display_properties_.w)
           && mouse_y > display_properties_.y
           && mouse_y < (display_properties_.y + display_properties_.h)){
          SDL_GetDesktopDisplayMode(i, &monitor_mode);
          break;
        }
      }
      window_ = SDL_CreateWindow("Torero", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 torero::k_default_width, torero::k_default_height,
                                 SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

      if(!window_){
        torero::terminal::Printer::error("SDL failed creating a window");
        SDL_Quit();
        error_log_ = torero::WindowErrors::WindowNotLoaded;
        error_ = true;
      }else{
        SDL_SetWindowDisplayMode(window_, &monitor_mode);
        SDL_SetWindowBordered(window_, system_title ? SDL_TRUE : SDL_FALSE);

        const int x = display_properties_.x + (display_properties_.w - torero::k_default_width)/2;
        const int y = display_properties_.y + (display_properties_.h - torero::k_default_height)/2;

        SDL_SetWindowPosition(window_, x, y);

        // ------------------------------------------------------------------------------------ //
        // ------------------------------- Loading window's icon ------------------------------ //
        // ------------------------------------------------------------------------------------ //
        load_window_icon();

        // ------------------------------------------------------------------------------------ //
        // ----------------------------------- Loading Vulkan --------------------------------- //
        // ------------------------------------------------------------------------------------ //
        /*
        unsigned int extension_count;
        SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, nullptr);
        std::vector<const char*> extension_names(extension_count);
        SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, extension_names.data());

        std::vector<const char*> layer_names{};
        VkApplicationInfo app_info{};
        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &app_info;
        info.enabledLayerCount = layer_names.size();
        info.ppEnabledLayerNames = layer_names.data();
        info.enabledExtensionCount = extension_names.size();
        info.ppEnabledExtensionNames = extension_names.data();

        VkInstance instance;
        VkResult result = VkCreateInstance(&info, nullptr, &instance);

        if(result != VK_SUCCESS){
          // do some error shcking
        }

        VkSurfaceKHR surface;
        if(!SDL_Vulkan_CreateSurface(window_, instance, &surface)){
          // failed to create surface
        }
        */

        if(!(context_ = SDL_GL_CreateContext(window_))){
          torero::terminal::Printer::error("OpenGL context creation failed");
          error_log_ =  torero::WindowErrors::OpenGLNotLoaded;
          error_ = true;
        }else{
          SDL_GL_MakeCurrent(window_, context_);

          // glad: load all OpenGL function pointers
          // ---------------------------------------
          if(!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress))){
            torero::terminal::Printer::error("Failed to initialize GLAD");
            error_log_ = torero::WindowErrors::GLAD_NotLoaded;
            error_ = true;
          }else{
            SDL_GL_SetSwapInterval(1);

            text_manager_ = new torero::text::Manager(this);

            initialize();
          }
        }
      }
    }
  }

  Core::~Core(){
    delete camera_;
    delete controller_;
    if(environment_) delete environment_;
    delete image_manager_;
    delete text_manager_;

    if(window_){
      SDL_GL_DeleteContext(context_);
      SDL_DestroyWindow(window_);
      SDL_Quit();
    }
  }

  torero::camera::View &Core::camera(){
    return *camera_;
  }

  torero::gui::Controller &Core::controller(){
    return *controller_;
  }

  torero::model::Environment &Core::environment(){
    return *environment_;
  }

  torero::image::Manager &Core::image_manager(){
    return *image_manager_;
  }

  torero::text::Manager &Core::text_manager(){
    return *text_manager_;
  }

  torero::vehicle::Manager &Core::vehicle(){
    return *vehicle_manager_;
  }

  void Core::close(){
    closing_ = true;
  }

  bool Core::closing(){
    return closing_;
  }

  int Core::execute(const bool infinite_loop){
    if(!error_){
      SDL_ShowWindow(window_);
      if(infinite_loop)
        while(!closing_){

          // Checks if the windows is visible and there are at least a change
          if(!hidden_ && has_changed_){
            paint();
            has_changed_ = false;

            SDL_GL_SwapWindow(window_);
          }
          if(!multithread_all_finished()){
            multithread_update_process();
            wait_for_events(torero::k_frequency);
          }else
            wait_for_events();
        }
      return EXIT_SUCCESS;
    }else{
      return error_log_;
    }
  }

  int Core::execute(int width, int height, const std::string title, const bool full_screen,
                    const bool maximized, const bool infinite_loop){
    if(!error_){
      SDL_SetWindowTitle(window_, title.c_str());

      width = (width <= 0)? display_properties_.w : width;
      height = (height <= 0)? display_properties_.h : height;

      const int x = display_properties_.x + (display_properties_.w - width)/2;
      const int y = display_properties_.y + (display_properties_.h - height)/2;

      SDL_SetWindowSize(window_, width, height);
      SDL_SetWindowPosition(window_, x, y);

      if(full_screen){
        SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);
      }else if(maximized){
        SDL_MaximizeWindow(window_);
      }

      return execute(infinite_loop);
    }else{
      return error_log_;
    }
  }

  void Core::full_screen(const bool make_full){
    if(make_full){
      SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);
    }else{
      SDL_SetWindowFullscreen(window_, 0);
      SDL_RestoreWindow(window_);
    }
  }

  uint32_t Core::get_time(){
    return SDL_GetTicks();
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
    torero::gl::Texture::set_max_filtering(max_filtering_);
    // Avoiding the rendering of all back faces
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    camera_->resolution(window_properties_.w, window_properties_.h,
                        window_properties_.w, window_properties_.h);
    camera_->set_function_callback(boost::bind(&Core::updated_camera, this));
  }

  void Core::load_window_icon(){
    int width, height, format;
    std::string icon_path(torero::k_icon_default_path);
    torero::file::Manager::check_path(&icon_path);

    unsigned char *data = stbi_load(icon_path.c_str(), &width, &height, &format, STBI_rgb_alpha);

    if(!data){
      torero::terminal::Printer::error("Loading icon failed: " + std::string(stbi_failure_reason()));
      return;
    }

    uint32_t red_mask{0x000000ff}, green_mask{0x0000ff00};
    uint32_t blue_mask{0x00ff0000}, alpha_mask{0xff000000};

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    red_mask = 0xff000000;
    green_mask = 0x00ff0000;
    blue_mask = 0x0000ff00;
    alpha_mask = 0x000000ff;
#endif

    SDL_Surface *icon = SDL_CreateRGBSurfaceFrom((void*)data, width, height, 32,
                                                 STBI_rgb_alpha * width, red_mask,
                                                 green_mask, blue_mask, alpha_mask);

    if(icon)
      SDL_SetWindowIcon(window_, icon);
    else{
      torero::terminal::Printer::error("Creating icon failed");
      stbi_image_free(data);
      return;
    }

    SDL_FreeSurface(icon);
    stbi_image_free(data);
  }

  void Core::maximize(const bool maximize){
    if(maximize)
      SDL_MaximizeWindow(window_);
    else
      SDL_RestoreWindow(window_);
  }

  void Core::minimize(const bool minimize){
    if(minimize)
      SDL_MinimizeWindow(window_);
    else
      SDL_RestoreWindow(window_);
  }

  void Core::paint(){
    //clearing the screen of old information
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    signal_updated_screen_();

    for(std::size_t i = 0; i < torero::k_drawing_elements; ++i)
      signal_draw_[i]();
  }

  void Core::process_pending_events(){
    if(!multithread_all_finished()){
      multithread_update_process();
    }

    poll_events();
  }

  void Core::restart_viewport(){
    glViewport(0, 0, window_properties_.w, window_properties_.h);
  }

  void Core::reload_screen(const bool change){
    has_changed_ = change;
  }

  void Core::sdl_error(){
    std::cout << "\n\033[1;41m SDL error: \033[0;1;38;5;174m ";
    std::cout << SDL_GetError() << "\033[0m" << std::endl;
  }

  void Core::set_module(torero::gui::Manager */*gui_manager*/,
                        torero::model::Environment *&environment,
                        torero::text::Manager *&text_manager){
    if(!environment_){
      environment_ = new torero::model::Environment(this);
      environment_->load_skybox();
      environment_->load_enviroment_map();
    }
    text_manager = text_manager_;
    environment = environment_;
  }

  const torero::model::Environment &Core::set_module(torero::model::Manager */*model_manager*/){
    if(!environment_){
      environment_ = new torero::model::Environment(this);
      environment_->load_skybox();
      environment_->load_enviroment_map();
    }
    return *environment_;
  }

  void Core::set_module(torero::vehicle::Manager *vehicle_manager){
    vehicle_manager_ = vehicle_manager;
  }

  void Core::screen_clear(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  const GLfloat Core::screen_max_filtering(){
    return max_filtering_;
  }

  void Core::screen_paint(){
    paint();
  }

  void Core::screen_redraw(){
    paint();
    SDL_GL_SwapWindow(window_);
  }

  void Core::screen_swap_buffers(){
    SDL_GL_SwapWindow(window_);
  }

  torero::PointXY<int> Core::position(){
    return torero::PointXY<int>{ window_properties_.x, window_properties_.y };
  }

  void Core::position(const int x, const int y){
    SDL_SetWindowPosition(window_, x, y);
  }

  torero::PointXY<int> Core::size(){
    return torero::PointXY<int>{display_properties_.w, display_properties_.h};
  }

  void Core::size(const int width, const int height){
    SDL_SetWindowSize(window_, width, height);
    SDL_RestoreWindow(window_);

    has_changed_ = true;
  }

  void Core::sleep(const uint32_t milliseconds){
    SDL_Delay(milliseconds);
  }

  void Core::title(const std::string &title){
    title_ = title;
    SDL_SetWindowTitle(window_, title.c_str());
  }

  const std::string &Core::title(){
    return title_;
  }

  bool Core::visibility(){
    return !hidden_;
  }

  void Core::visibility(const bool hide){
    if(hide)
      SDL_HideWindow(window_);
    else
      SDL_ShowWindow(window_);
  }

  boost::signals2::connection Core::syncronize(const torero::Order object,
                                               boost::function<void ()> callback){
    return signal_draw_[static_cast<std::size_t>(object)].connect(callback);
  }

  boost::signals2::connection Core::connect_camera_updates(boost::function<void ()> callback){
    return signal_updated_camera_.connect(callback);
  }

  boost::signals2::connection Core::connect_sreeen_updates(boost::function<void ()> callback){
    return signal_updated_screen_.connect(callback);
  }

  void Core::updated_camera(){
    signal_updated_camera_();
  }
}

