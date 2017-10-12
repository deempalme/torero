#include "headers/console.h"

Console::Console(int _argc, char **_argv) :
  argc_(_argc),
  argv_(_argv),
  window_(nullptr),
  gui_(nullptr),
  width_(0),
  height_(0)
{
}

Console::~Console(){
  if(window_){
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

int Console::execute(){
  if(!glfwInit()){
    cout << "GLFW initialization failed" << endl;
    return GLFW_NOT_LOADED;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

  // intializing DevIL image loader
  ilInit();

//   window_ = glfwCreateWindow(1350, 600, "Torero", glfwGetPrimaryMonitor(), NULL);
  // non full screen
  window_ = glfwCreateWindow(600, 600, "Torero", NULL, NULL);
//  window_ = glfwCreateWindow(1350, 600, "Torero", NULL, NULL);
  if(!window_){
    glfwTerminate();
    cout << "GLFW failed creating a window" << endl;
    return WINDOW_NOT_LOADED;
  }

  // ------------------------------------------------------------------------------------ //
  // ------------------------------- Loading window's icon ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  ILuint icon_id{ilGenImage()};
  ilBindImage(icon_id);

  ilLoadImage("images/icon.png");

  GLFWimage icon = {ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetData()};
  glfwSetWindowIcon(window_, 1, &icon);

  ilDeleteImage(icon_id);

  // ------------------------------------------------------------------------------------ //
  // ---------------------------- Ending loading window's icon -------------------------- //
  // ------------------------------------------------------------------------------------ //

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
  return EXIT_SUCCESS;
}

signals2::signal<void (int, int)> Console::resize_signal;

void Console::initializeGL(){
  //setting the background color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //this line allows anti-aliasing to create fine edges
  glEnable(GL_MULTISAMPLE);
  //this line allows z-buffer to avoid objects behind others appear in front
  glEnable(GL_DEPTH_TEST);
  //this allows textures
  glEnable(GL_TEXTURE_2D);
  //these allow alpha transparency in the rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const char *vertexShaderSource = "#version 130\n"
                                   "in vec3 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                   "}\0";
  const char *fragmentShaderSource = "#version 130\n"
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
  width_ = width;
  height_ = height;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
  glViewport(0, 0, width, height);
  Console::resize_signal(width, height);
}
