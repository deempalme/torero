#include "includes/skybox.h"

Skybox::Skybox(const char *folder_path, const char *file_extension,
               const mat4f *pv_matrix, Console *console) :
  folder_path_(folder_path),
  file_extension_(file_extension),
  pv_matrix_(pv_matrix),
  console_(console),
  is_ready_(false)
{
  error_signal.connect(bind(&Console::error_handler, console_, _1, _2));
  message_signal.connect(bind(&Console::message_handler, console_, _1, _2));
  ready.connect(bind(&Skybox::load_ready, this));

  stbi_set_flip_vertically_on_load(true);

  runner_ = boost::thread(bind(&Skybox::load_images, this));
  runner_.detach();

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  GLuint vertex_shader{glCreateShader(GL_VERTEX_SHADER)};
  const char *vertex_data{load_shader("resources/shaders/skybox.vert")};
  glShaderSource(vertex_shader, 1, &vertex_data, NULL);
  glCompileShader(vertex_shader);
  // check for shader compile errors
  GLint success;
  char info_log[512];
  GLsizei info_size;
  // check for shader compile errors
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(vertex_shader, 512, &info_size, info_log);
    error_signal("skybox\'s vertex shader compilation failed...\n" +
                 string(info_log, info_size), ERROR_MESSAGE);
  }
  // fragment shader
  GLuint fragment_shader{glCreateShader(GL_FRAGMENT_SHADER)};
  const char *fragment_data{load_shader("resources/shaders/skybox.frag")};
  glShaderSource(fragment_shader, 1, &fragment_data, NULL);
  glCompileShader(fragment_shader);
  // check for shader compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(fragment_shader, 512, &info_size, info_log);
    error_signal("Skybox\'s fragment shader compilation failed...\n" +
                 string(info_log, info_size), ERROR_MESSAGE);
  }

  // link shaders
  shader_program_ = glCreateProgram();
  glAttachShader(shader_program_, vertex_shader);
  glAttachShader(shader_program_, fragment_shader);
  glLinkProgram(shader_program_);
  // check for linking errors
  glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shader_program_, 512, &info_size, info_log);
    error_signal("Skybox\'s shader program linking failed...\n" +
                 string(info_log, info_size), ERROR_MESSAGE);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

Skybox::~Skybox(){
}

void Skybox::change_folder(const char *folder_path){
  if(protector_.try_lock()){
    folder_path_ = folder_path;
    protector_.unlock();

    runner_ = boost::thread(bind(&Skybox::load_images, this));
    runner_.detach();
  }else{
    message_signal("wait until the previous cubemap has been loaded", WARNING_MESSAGE);
  }
}

void Skybox::reload_data(){
  if(protector_.try_lock()){
    protector_.unlock();

    is_ready_ = false;
    runner_ = boost::thread(bind(&Skybox::load_images, this));
    runner_.detach();
  }else{
    message_signal("wait until the previous cubemap has been loaded", WARNING_MESSAGE);
  }
}

void Skybox::draw(){

}

const bool Skybox::is_ready(){
  return is_ready_;
}

void Skybox::load_images(){
  protector_.lock();
  // loading up image
  up_.data = stbi_loadf(string(folder_path_ + "/up" + file_extension_).c_str(),
                        &up_.width, &up_.height, &up_.components_size, 0);
  // loading down image
  down_.data = stbi_loadf(string(folder_path_ + "/dn" + file_extension_).c_str(),
                        &down_.width, &down_.height, &down_.components_size, 0);
  // loading left image
  left_.data = stbi_loadf(string(folder_path_ + "/lf" + file_extension_).c_str(),
                        &left_.width, &left_.height, &left_.components_size, 0);
  // loading right image
  right_.data = stbi_loadf(string(folder_path_ + "/rt" + file_extension_).c_str(),
                        &right_.width, &right_.height, &right_.components_size, 0);
  // loading front image
  front_.data = stbi_loadf(string(folder_path_ + "/ft" + file_extension_).c_str(),
                        &front_.width, &front_.height, &front_.components_size, 0);
  // loading back image
  back_.data = stbi_loadf(string(folder_path_ + "/bk" + file_extension_).c_str(),
                        &back_.width, &back_.height, &back_.components_size, 0);
  is_ready_ = up_.data && down_.data && left_.data && right_.data && front_.data && back_.data;
  protector_.unlock();
  ready();
}

void Skybox::load_ready(){
  if(is_ready_){
    glGenTextures(1, &textures_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures_id_);

    write_data_opengl(left_, 0);
    write_data_opengl(up_, 1);
    write_data_opengl(front_, 2);
    write_data_opengl(right_, 3);
    write_data_opengl(down_, 4);
    write_data_opengl(back_, 5);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }else{
    error_signal("Some/all files for the skybox were not found", ERROR_MESSAGE);
  }
}

const char *Skybox::load_shader(const char *file_path){
  string content;
  ifstream file;
  file.open(file_path, ios::in);

  if(file.is_open()){
    content.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content.c_str();
  }else{
    error_signal("Skybox shader was not found: " + string(file_path), ERROR_MESSAGE);
    return "";
  }
}

void Skybox::write_data_opengl(const ImageFile &image, const int level){
  switch(image.components_size){
  case 1:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RED, image.width,
                 image.height, 0, GL_RGB, GL_FLOAT, image.data);
    break;
  case 2:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RG, image.width,
                 image.height, 0, GL_RGB, GL_FLOAT, image.data);
    break;
  case 4:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGBA, image.width,
                 image.height, 0, GL_RGB, GL_FLOAT, image.data);
    break;
  default:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGB, image.width,
                 image.height, 0, GL_RGB, GL_FLOAT, image.data);
    break;
  }
  stbi_image_free(image.data);
}
