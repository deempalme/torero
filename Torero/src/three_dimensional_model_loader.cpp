#include "includes/three_dimensional_model_loader.h"

Console *ThreeDimensionalModelLoader::console_ = nullptr;
GLFWwindow *ThreeDimensionalModelLoader::window_ = nullptr;
GLuint ThreeDimensionalModelLoader::shader_program_ = 0;

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const char *folder_address) :
  folder_address_(folder_address),
  is_ready_(false),
  data_size_(0),
  buffer_data_(0)
{
  error_signal.connect(bind(&Console::error_handler, console_, _1, _2));
  message_signal.connect(bind(&Console::message_handler, console_, _1, _2));
  ready.connect(bind(&ThreeDimensionalModelLoader::model_ready, this));

  runner_ = boost::thread(bind(&ThreeDimensionalModelLoader::initialize, this));
  runner_.detach();
}

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const string folder_address) :
  folder_address_(folder_address.c_str()),
  is_ready_(false),
  data_size_(0),
  buffer_data_(0)
{
  error_signal.connect(bind(&Console::error_handler, console_, _1, _2));
  message_signal.connect(bind(&Console::message_handler, console_, _1, _2));
  ready.connect(bind(&ThreeDimensionalModelLoader::model_ready, this));

  runner_ = boost::thread(bind(&ThreeDimensionalModelLoader::initialize, this));
  runner_.detach();
}

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const unsigned int model) :
  is_ready_(false),
  data_size_(0),
  buffer_data_(0)
{
  switch(model){
  case DB5_BODY:
    folder_address_ = "resources/models3D/db5_body";
    break;
  case DB5_WINDOWS:
    folder_address_ = "resources/models3D/db5_windows";
    break;
  case DB5_ACCESSORIES:
    folder_address_ = "resources/models3D/db5_accessories";
    break;
  case DB5_INTERIOR:
    folder_address_ = "resources/models3D/db5_interior";
    break;
  case CHASSIS:
    folder_address_ = "resources/models3D/chassis";
    break;
  case AXIS:
    folder_address_ = "resources/models3D/axis";
    break;
  case STEERING:
    folder_address_ = "resources/models3D/steering";
    break;
  case TIRE:
    folder_address_ = "resources/models3D/tire";
    break;
  case COORDINATE_SYSTEM:
    folder_address_ = "resources/models3D/coordinate_system";
    break;
  }

  error_signal.connect(bind(&Console::error_handler, console_, _1, _2));
  message_signal.connect(bind(&Console::message_handler, console_, _1, _2));
  ready.connect(bind(&ThreeDimensionalModelLoader::model_ready, this));

  runner_ = boost::thread(bind(&ThreeDimensionalModelLoader::initialize, this));
  runner_.detach();
}

ThreeDimensionalModelLoader::~ThreeDimensionalModelLoader(){
}

const bool ThreeDimensionalModelLoader::is_ready(){
  return is_ready_;
}

void ThreeDimensionalModelLoader::set_shader_program(const GLuint shader_program){
  shader_program_ = shader_program;
}

void ThreeDimensionalModelLoader::set_console(Console *console){
  console_ = console;
}

void ThreeDimensionalModelLoader::set_window(GLFWwindow *window){
  window_ = window;
}

void ThreeDimensionalModelLoader::initialize(){
//  protector_.lock();
  vector<vec3f> position, normal;
  vector<vec2f> texture;
  vec3f tvector;
  vec2f ttexture;
  vector<unsigned int> vertex_indices, texture_indices, normal_indices;
  unsigned int vertex_index[3], texture_index[3], normal_index[3];
  string line;

  protector_.lock();
  ifstream file;
  file.open(string(folder_address_) + "/model.obj", ios::in);

  if(file.is_open()){
    while(getline(file, line)){

      if(line == "" || line[0] == '#')
        continue;

      const char *line_c = line.c_str();

      if(line[0] == 'v' && line[1] == ' '){
        sscanf(line_c, "%*s %f %f %f", &tvector[0], &tvector[1], &tvector[2]);
        position.push_back(tvector);
      }else if(line[0] == 'v' && line[1] == 't'){
        sscanf(line_c, "%*s %f %f", &ttexture[0], &ttexture[1]);
        texture.push_back(ttexture);
      }else if(line[0] == 'v' && line[1] == 'n'){
        sscanf(line_c, "%*s %f %f %f", &tvector[0], &tvector[1], &tvector[2]);
        normal.push_back(tvector);
      }else if(line[0] == 'f'){
        sscanf(line_c, "%*s %u/%u/%u %u/%u/%u %u/%u/%u",
               &vertex_index[0], &texture_index[0], &normal_index[0],
            &vertex_index[1], &texture_index[1], &normal_index[1],
            &vertex_index[2], &texture_index[2], &normal_index[2]);
        for(int i = 0; i < 3; i++){
          vertex_indices.push_back(vertex_index[i]);
          texture_indices.push_back(texture_index[i]);
          normal_indices.push_back(normal_index[i]);
        }
      }
    }
    file.close();

    buffer_data_.resize(vertex_indices.size());
    // For each vertex of each triangle
    for(int i = 0; i < vertex_indices.size(); i++){
      buffer_data_[i].position = position[vertex_indices[i] - 1];
      buffer_data_[i].normal = normal[normal_indices[i] - 1];
      if(texture_indices[i] > 0)
        buffer_data_[i].texture = texture[texture_indices[i] - 1];
      else
        buffer_data_[i].texture = vec2f();
    }

    // writing the data into openGL buffers
    data_size_ = static_cast<GLsizei>(vertex_indices.size() * sizeof(SimpleShaderData));

    cout << "data size:" << vertex_indices.size() << endl;

//    protector_.lock();
    is_ready_ = true;
    ready();
    protector_.unlock();

    cout << "ready called" << endl;
  }else{
    protector_.lock();
    error_signal(string("file not fount:" + string(folder_address_)).c_str(), FILE_NOT_FOUND);
    is_ready_ = false;
    protector_.unlock();
  }
}

void ThreeDimensionalModelLoader::model_ready(){
  glfwMakeContextCurrent(window_);
  glUseProgram(shader_program_);
  i_position_ = glGetAttribLocation(shader_program_, "i_position");
  i_texture_  = glGetAttribLocation(shader_program_, "i_texture");
  i_normal_   = glGetAttribLocation(shader_program_, "i_normal");

  u_colored_      = glGetUniformLocation(shader_program_, "u_colored");
  u_color_        = glGetUniformLocation(shader_program_, "u_color");
  u_metallized_   = glGetUniformLocation(shader_program_, "u_metallized");
  u_metallic_     = glGetUniformLocation(shader_program_, "u_metallic");
  u_roughed_      = glGetUniformLocation(shader_program_, "u_roughed");
  u_roughness_    = glGetUniformLocation(shader_program_, "u_roughness");
  u_is_interior_  = glGetUniformLocation(shader_program_, "u_is_interior");

  glGenVertexArrays(1, &vertex_array_);
  glGenBuffers(1, &vertex_buffer_);

  glBindVertexArray(vertex_array_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, data_size_, buffer_data_.data(), GL_STATIC_DRAW);

  buffer_data_.clear();
  console_->model_ready(this);
}
