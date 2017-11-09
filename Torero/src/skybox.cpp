#include "includes/skybox.h"
#include "includes/console.h"
#include "includes/shader.h"

Skybox::Skybox(const char *folder_path, const char *file_extension,
               Algebraica::mat4f *const pv_matrix, Console *console, GLFWwindow *window) :
  folder_path_(folder_path),
  file_extension_(file_extension),
  pv_matrix_(pv_matrix),
  console_(console),
  window_(window),
  is_ready_(false),
  stride_size_(3 * sizeof(GLfloat)),
  offset_pointer_(0)
{
  message_signal.connect(boost::bind(&Console::message_handler, console_, _1, _2));
  ready.connect(boost::bind(&Skybox::load_ready, this));

  stbi_set_flip_vertically_on_load(true);

  runner_ = boost::thread(boost::bind(&Skybox::load_images, this));
  runner_.detach();

  // build and compile our shader program
  if(!shader_program_.create("resources/shaders/skybox.vert", "resources/shaders/skybox.frag"))
    message_signal(shader_program_.error_log(), ERROR_MESSAGE);
  // ------------------------------------
  i_position_ = shader_program_.attribute_location("i_position");
  u_pv_ = shader_program_.uniform_location("u_pv");
  u_skybox_ = shader_program_.uniform_location("u_skybox");
}

void Skybox::change_folder(const char *folder_path){
  if(protector_.try_lock()){
    folder_path_ = folder_path;
    protector_.unlock();

    runner_ = boost::thread(boost::bind(&Skybox::load_images, this));
    runner_.detach();
  }else{
    message_signal("wait until the previous cubemap has been loaded", WARNING_MESSAGE);
  }
}

void Skybox::reload_data(){
  if(protector_.try_lock()){
    protector_.unlock();

    is_ready_ = false;
    runner_ = boost::thread(boost::bind(&Skybox::load_images, this));
    runner_.detach();
  }else{
    message_signal("wait until the previous cubemap has been loaded", WARNING_MESSAGE);
  }
}

void Skybox::draw(){
  if(is_ready_){
    glfwMakeContextCurrent(window_);
    shader_program_.use();

    glEnable(GL_TEXTURE_CUBE_MAP);
    glActiveTexture(GL_TEXTURE0 + texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

    shader_program_.set_value(u_pv_, pv_matrix_);
    vertex_array_.use();

    glVertexAttribPointer(i_position_, THREE_DIMENSIONAL, GL_FLOAT, GL_FALSE,
                          stride_size_, offset_pointer_);

    glEnableVertexAttribArray(i_position_);
    glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_SHORT, NULL);
    glDisableVertexAttribArray(i_position_);

    glDisable(GL_TEXTURE_CUBE_MAP);
  }
}

const bool Skybox::is_ready(){
  return is_ready_;
}

void Skybox::load_images(){
  protector_.lock();
  // loading up image
  up_.data = stbi_load(std::string(folder_path_ + "up" + file_extension_).c_str(),
                       &up_.width, &up_.height, &up_.components_size, 0);
  // loading down image
  down_.data = stbi_load(std::string(folder_path_ + "dn" + file_extension_).c_str(),
                         &down_.width, &down_.height, &down_.components_size, 0);
  // loading left image
  left_.data = stbi_load(std::string(folder_path_ + "lf" + file_extension_).c_str(),
                         &left_.width, &left_.height, &left_.components_size, 0);
  // loading right image
  right_.data = stbi_load(std::string(folder_path_ + "rt" + file_extension_).c_str(),
                          &right_.width, &right_.height, &right_.components_size, 0);
  // loading front image
  front_.data = stbi_load(std::string(folder_path_ + "ft" + file_extension_).c_str(),
                          &front_.width, &front_.height, &front_.components_size, 0);
  // loading back image
  back_.data = stbi_load(std::string(folder_path_ + "bk" + file_extension_).c_str(),
                         &back_.width, &back_.height, &back_.components_size, 0);
  protector_.unlock();
  ready();
}

void Skybox::load_ready(){
  is_ready_ = up_.data && down_.data && left_.data && right_.data && front_.data && back_.data;

  if(is_ready_){
    glfwMakeContextCurrent(window_);
    shader_program_.use();

    // cube coordinates for the skybox creation
    GLfloat cube_vertices[24] = {
      -1.0,  1.0,  1.0,
      -1.0, -1.0,  1.0,
       1.0, -1.0,  1.0,
       1.0,  1.0,  1.0,
      -1.0,  1.0, -1.0,
      -1.0, -1.0, -1.0,
       1.0, -1.0, -1.0,
       1.0,  1.0, -1.0,
    };

    // generate vertex array
    vertex_array_.create();

    // Generate an array buffer
    vertex_array_.allocate_array(&cube_vertices, 24 * sizeof(GLfloat), GL_STATIC_DRAW);

    // indices for the cube vertices
    GLushort cube_indices[36] = {
      0, 0, 1, 3, 2, 2,
      3, 3, 2, 7, 6, 6,
      7, 7, 6, 4, 5, 5,
      4, 4, 5, 0, 1, 1,
      0, 0, 3, 4, 7, 7,
      1, 1, 2, 5, 6, 6
    };

    // Generate a buffer for the indices
    vertex_array_.allocate_element(&cube_indices, 36 * sizeof(GLushort), GL_STATIC_DRAW);

    glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &texture_id_);
    glUniform1i(u_skybox_, texture_id_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

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
    glDisable(GL_TEXTURE_CUBE_MAP);

  }else{
    message_signal("Some/all files for the skybox were not found", ERROR_MESSAGE);
  }
}

void Skybox::write_data_opengl(const Visualizer::ImageFile &image, const int level){
  switch(image.components_size){
  case 1:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RED, image.width,
                 image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    break;
  case 2:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RG, image.width,
                 image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    break;
  case 4:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGBA, image.width,
                 image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    break;
  default:
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGB, image.width,
                 image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    break;
  }
  stbi_image_free(image.data);
}
