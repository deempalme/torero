#include "includes/console.h"

Console::Console(int _argc, char **_argv) :
  argc_(_argc),
  argv_(_argv),
  window_(nullptr),
  width_(0),
  height_(0),
  is_main_open_(false),
  gui_(nullptr),
  max_filtering_(0.0f),
  textures_id_(0)
{
}

Console::~Console(){
  if(window_){
    glfwDestroyWindow(window_);
    glfwTerminate();
    for(Model3D model : models_)
      if(model.model)
        delete model.model;
  }
}

int Console::execute(const int width, const int height, const char* title,
                     const bool full_screen, const bool maximized){
  if(!is_main_open_){
    if(!glfwInit()){
      cout << "GLFW initialization failed" << endl;
      return GLFW_NOT_LOADED;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, (maximized)? GLFW_TRUE : GLFW_FALSE);

    // intializing DevIL image loader
    ilInit();

    const GLFWvidmode* screen = glfwGetVideoMode(glfwGetPrimaryMonitor());

    width_ = (width <= 0)? screen->width : width;
    height_ = (height <= 0)? screen->height : height;

    if(full_screen)
      window_ = glfwCreateWindow(width_, height_, title, glfwGetPrimaryMonitor(), NULL);
    else
      window_ = glfwCreateWindow(width_, height_, title, NULL, NULL);

    if(!window_){
      glfwTerminate();
      cout << "GLFW failed creating a window" << endl;
      return WINDOW_NOT_LOADED;
    }

    // ------------------------------------------------------------------------------------ //
    // ------------------------------- Loading window's icon ------------------------------ //
    // ------------------------------------------------------------------------------------ //
    load_icon();

    glfwMakeContextCurrent(window_);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
      glfwTerminate();
      cout << "Failed to initialize GLAD : " << endl;
      return GLAD_NOT_LOADED;
    }
    glfwSwapInterval(1);

    resize_signal.connect(bind(&Console::resizeGL, this, _1, _2));

    initializeGL();
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

//    mat4f testing;
//    testing.rotate(0, _PI, 0);
//    cout << testing << endl;
//    testing.translate(2, 3, 1);
//    cout << testing << endl;

    while(!glfwWindowShouldClose(window_)){
      paintGL();

//    if(glfwGetWindowAttrib(window_, GLFW_VISIBLE)){
//      // window is visible
//    }

      glfwSwapBuffers(window_);
      glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window_);
    glfwTerminate();

    is_main_open_ = true;

    return EXIT_SUCCESS;
  }else{
    return EXISTING_WINDOW;
  }
}

void Console::error_handler(const string text, const int error_type){
  cout << "Error:" << error_type << " - " << text << endl;
}

void Console::message_handler(const string text, const int message_type){
  cout << "Message received:" << message_type << " - " << text << endl;
}

void Console::model_ready(ThreeDimensionalModelLoader *model){
  cout << "Model ready... " << model->is_ready() << endl;
}

signals2::signal<void (int, int)> Console::resize_signal;

void Console::initializeGL(){
  // setting the background color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // this line allows anti-aliasing to create fine edges
  glEnable(GL_MULTISAMPLE);
  // this line allows z-buffer to avoid objects behind others appear in front
  glEnable(GL_DEPTH_TEST);
  // this allows textures
  glEnable(GL_TEXTURE_2D);
  // these allow alpha transparency in the rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // detects the maximum anisotropic filtering samples
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_filtering_);
  max_filtering_ = (max_filtering_ > 8)? 8 : max_filtering_;


  const char *vertexShaderSource = "#version 420\n"
                                   "in vec3 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                   "}\0";
  const char *fragmentShaderSource = "#version 420\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                     "}\n\0";

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // link shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
    -0.5f, -0.5f, 0.0f, // left
    0.5f, -0.5f, 0.0f, // right
    0.0f,  0.5f, 0.0f  // top
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

//  load_texture("resources/images/icon.png", shaderProgram, true);
//  load_cubemap("resources/cubemap/", shaderProgram, true);

  ThreeDimensionalModelLoader::set_console(this);
  ThreeDimensionalModelLoader::set_window(window_);
  ThreeDimensionalModelLoader::set_shader_program(shaderProgram);
  ThreeDimensionalModelLoader *tire = new ThreeDimensionalModelLoader(TIRE);
  Model3D model{tire, mat4f()};
  models_.push_back(model);
}

void Console::paintGL(){
  //clearing the screen of old information
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw our first triangle
  glUseProgram(shaderProgram);
  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  glDrawArrays(GL_TRIANGLES, 0, 3);
  // glBindVertexArray(0); // no need to unbind it every time
}

void Console::resizeGL(const int width, const int height){
  glViewport(0, 0, width, height);
  width_ = width;
  height_ = height;
}

bool Console::create_shader_programs(){

}

bool Console::load_textures(){

}

int Console::load_texture(const char *file_address, const GLuint shader, const bool alpha){
  int id{-1};

  glUseProgram(shader);
  ILuint image_id{ilGenImage()};
  ilBindImage(image_id);

  if(ilLoadImage(file_address)){
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // applies anisotropic filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_filtering_);
    // loads the image data into open gl
    if(alpha)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                   ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH),
                   ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
    glGenerateMipmap(GL_TEXTURE_2D);

    id = textures_id_.size();
    textures_id_.push_back(texture_id);

    ilDeleteImage(image_id);
  }else
    cout << "Error: Failed to load texture:" << file_address << endl;
  return id;
}

int Console::load_cubemap(const string folder_address, const GLuint shader, const bool alpha){
  int id{-1};

  glUseProgram(shader);
  ILuint image_id{ilGenImage()};
  ilBindImage(image_id);

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  for(GLuint i = 0; i < 6; i++){
    string file_name;
    switch(i){
    case 1: file_name = "up.png"; break;
    case 2: file_name = "ft.png"; break;
    case 3: file_name = "rt.png"; break;
    case 4: file_name = "dn.png"; break;
    case 5: file_name = "bk.png"; break;
    default: file_name = "lf.png"; break;
    }

    file_name = folder_address + file_name;

    if(ilLoadImage(file_name.c_str())){
      if(alpha)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
      else
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH),
                     ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
      ilDeleteImage(image_id);
    }else
      cout << "Error: Failed to load texture:" << file_name << endl;
  }

  id = textures_id_.size();
  textures_id_.push_back(texture_id);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return id;
}

void Console::load_icon(){
  ILuint icon_id{ilGenImage()};
  ilBindImage(icon_id);

  ilLoadImage("resources/images/icon.png");

  GLFWimage icon = {ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetData()};
  glfwSetWindowIcon(window_, 1, &icon);

  ilDeleteImage(icon_id);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
  Console::resize_signal(width, height);
}
