#include "include/skybox.h"
#include "include/core.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  Skybox::Skybox(const std::string up, const std::string down,
                 const std::string left, const std::string right,
                 const std::string front, const std::string back,
                 Core *core) :
    up_path_(up),
    dn_path_(down),
    lf_path_(left),
    rt_path_(right),
    ft_path_(front),
    bk_path_(back),
    core_(core),
    is_ready_(false),
    is_loaded_(false),
    sky_shader_(new Shader("resources/shaders/skybox.vert",
                           "resources/shaders/skybox.frag")),
    buffer_cube_(new Buffer())
  {
    check_path(&up_path_);
    check_path(&dn_path_);
    check_path(&lf_path_);
    check_path(&rt_path_);
    check_path(&ft_path_);
    check_path(&bk_path_);

    // Skybox shader
    // -------------
    if(!sky_shader_->use())
      core_->message_handler(sky_shader_->error_log(), Visualizer::ERROR);

    sky_u_pv_ = sky_shader_->uniform_location("u_pv");
    sky_u_skybox_ = sky_shader_->uniform_location("u_skybox");

    signal_update_camera_ = core->signal_updated_camera()
                            ->connect(boost::bind(&Skybox::update_camera, this));
    signal_update_screen_ = core->syncronize(Visualizer::SKYBOX)
                            ->connect(boost::bind(&Skybox::draw, this));

    runner_ = boost::thread(boost::bind(&Skybox::load_images, this));
    runner_.detach();
  }

  Skybox::~Skybox(){
    signal_update_camera_.disconnect();
    signal_update_screen_.disconnect();

    glDeleteTextures(1, &sky_texture_id_);

    delete buffer_cube_;
    delete sky_shader_;
  }

  void Skybox::draw(){
    if(is_loaded_){
      sky_shader_->use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture_id_);

      buffer_cube_->vertex_bind();
      glDrawArrays(GL_TRIANGLES, 0, 36);
      buffer_cube_->vertex_release();
    }else{
      load_ready();
    }
  }

  const bool Skybox::is_ready(){
    if(is_loaded_){
      return is_ready_;
    }else{
      if(protector_.try_lock()){
        protector_.unlock();
        return is_ready_;
      }else
        return false;
    }
  }

  void Skybox::check_path(std::string *path){
    if(path->front() != '/')
      *path = "/" + *path;

    std::string first_path(*path);

    if(!boost::filesystem::exists(boost::filesystem::path(*path))){
      *path = boost::filesystem::current_path().string() + *path;

      if(!boost::filesystem::exists(boost::filesystem::path(*path)))
        core_->message_handler("The file: " + first_path + " was not found.\n" +
                               "  Neither: " + *path + "\n", Visualizer::ERROR);
    }
  }

  void Skybox::load_images(){
    protector_.lock();
    stbi_set_flip_vertically_on_load(false);
    // loading up image
    up_.data = stbi_load(std::string(up_path_).c_str(), &up_.width,
                         &up_.height, &up_.components_size, 0);
    // loading down image
    down_.data = stbi_load(std::string(dn_path_).c_str(), &down_.width,
                           &down_.height, &down_.components_size, 0);
    // loading left image
    left_.data = stbi_load(std::string(lf_path_).c_str(), &left_.width,
                           &left_.height, &left_.components_size, 0);
    // loading right image
    right_.data = stbi_load(std::string(rt_path_).c_str(), &right_.width,
                            &right_.height, &right_.components_size, 0);
    // loading front image
    front_.data = stbi_load(std::string(ft_path_).c_str(), &front_.width,
                            &front_.height, &front_.components_size, 0);
    // loading back image
    back_.data = stbi_load(std::string(bk_path_).c_str(), &back_.width,
                           &back_.height, &back_.components_size, 0);
    protector_.unlock();

    protector_.lock();
    is_ready_ = true;
    protector_.unlock();
  }

  void Skybox::load_ready(){
    if(protector_.try_lock()){
      protector_.unlock();
      if(is_ready_){
        // pbr: setup cubemap to render to and attach to framebuffer
        // ---------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &sky_texture_id_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture_id_);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // enable pre-filter mipmap sampling (combatting visible dots artifact)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                        core_->max_anisotropic_filtering());

        write_data_opengl(right_, 0);
        write_data_opengl(left_, 1);
        write_data_opengl(up_, 2);
        write_data_opengl(down_, 3);
        write_data_opengl(back_, 4);
        write_data_opengl(front_, 5);

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        prepare_cube();

        sky_shader_->use();
        sky_shader_->set_value(sky_u_skybox_, 0);

        // initialize static shader uniforms before rendering
        // --------------------------------------------------
        sky_shader_->use();
        sky_shader_->set_value(sky_u_pv_, core_->camera_matrix_static_perspective_view());

        is_loaded_ = true;

      }else{
        core_->message_handler("Some/all files for the skybox were not found", Visualizer::ERROR);
      }
    }
  }

  void Skybox::update_camera(){
    if(is_ready_){
      sky_shader_->use();
      sky_shader_->set_value(sky_u_pv_, core_->camera_matrix_static_perspective_view());
    }
  }

  void Skybox::write_data_opengl(const Visualizer::ImageFile &image, const int level){
    switch(image.components_size){
    case 1:
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RED, image.width,
                   image.height, 0, GL_RED, GL_UNSIGNED_BYTE, image.data);
      break;
    case 2:
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RG, image.width,
                   image.height, 0, GL_RG, GL_UNSIGNED_BYTE, image.data);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGBA, image.width,
                   image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
      break;
    default:
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + level, 0, GL_RGB, image.width,
                   image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
      break;
    }
    stbi_image_free(image.data);
  }

  void Skybox::prepare_cube(){
    float vertices[] = {
      // back face
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      +1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
      +1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
      +1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
      // front face
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
      +1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
      +1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
      +1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
      // left face
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      // right face
      +1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
      +1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
      +1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
      +1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
      +1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
      +1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
      // bottom face
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
      +1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
      +1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
      +1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
      -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
      // top face
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
      +1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
      +1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
      +1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
      -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };

    buffer_cube_->create();
    buffer_cube_->vertex_bind();
    buffer_cube_->allocate_array(&vertices, sizeof(vertices));

    GLsizei stride_size{sizeof(GLfloat) * 8};
    GLsizei offset_3d{sizeof(GLfloat) * 3};

    GLint offset{0};
    buffer_cube_->enable(0);
    buffer_cube_->attributte_buffer(0, _3D, offset, stride_size);

    offset += offset_3d;
    buffer_cube_->enable(1);
    buffer_cube_->attributte_buffer(1, _3D, offset, stride_size);

    offset += offset_3d;
    buffer_cube_->enable(2);
    buffer_cube_->attributte_buffer(2, _2D, offset, stride_size);

    buffer_cube_->vertex_release();
  }
}
