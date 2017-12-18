#include "include/skybox.h"
#include "include/core.h"
#include "include/shader.h"
// Image loader
#include "stb_image.h"
#include "stb_image_write.h"

namespace Toreo {
  Skybox::Skybox(const char *folder_path, const char *file_extension, Core *core) :
    folder_path_(folder_path),
    file_extension_(file_extension),
    core_(core),
    is_ready_(false),
    is_loaded_(false),
    irradiance_shader_(new Shader("resources/shaders/cubemap.vert",
                                  "resources/shaders/irradiance.frag")),
    prefilter_shader_(new Shader("resources/shaders/cubemap.vert",
                                 "resources/shaders/prefilter.frag")),
    sky_shader_(new Shader("resources/shaders/skybox.vert",
                           "resources/shaders/skybox.frag")),
    brdf_shader_(new Shader("resources/shaders/brdf.vert",
                            "resources/shaders/brdf.frag")),
    buffer_cube_(new Buffer()),
    buffer_squad_(new Buffer()),
    i_position_(0),
    i_normal_(1),
    i_uv_(2),
    stride_size_short_(3 * sizeof(GLfloat)),
    stride_size_medium_(5 * sizeof(GLfloat)),
    stride_size_long_(8 * sizeof(GLfloat))
  {
    if(folder_path_.front() != '/') folder_path_ = "/" + folder_path_;
    if(folder_path_.back() != '/') folder_path_ += "/";

    std::string first_path(folder_path_);

    if(!boost::filesystem::exists(boost::filesystem::path(folder_path_))){
      folder_path_ = boost::filesystem::current_path().string() + folder_path_;

      if(!boost::filesystem::exists(boost::filesystem::path(folder_path_)))
        std::cout << "The file: " << first_path << " was not found.\n" <<
                     "  Neither: " << folder_path_ << "\n";
    }
    // Irradiance shader
    // -----------------
    if(!irradiance_shader_->use())
      core_->message_handler(irradiance_shader_->error_log(), Visualizer::ERROR);

    // Prefilter shader
    // ----------------
    if(!prefilter_shader_->use())
      core_->message_handler(prefilter_shader_->error_log(), Visualizer::ERROR);

    // Skybox shader
    // -------------
    if(!sky_shader_->use())
      core_->message_handler(sky_shader_->error_log(), Visualizer::ERROR);

    sky_u_pv_ = sky_shader_->uniform_location("u_pv");
    sky_u_skybox_ = sky_shader_->uniform_location("u_skybox");

    signal_update_camera_ = core->signal_updated_camera()->
        connect(boost::bind(&Skybox::update_camera, this));
    signal_update_screen_ = core->signal_updated_screen()->
        connect(boost::bind(&Skybox::draw, this));

    runner_ = boost::thread(boost::bind(&Skybox::load_images, this));
    runner_.detach();
  }

  Skybox::~Skybox(){
    signal_update_camera_.disconnect();
    signal_update_screen_.disconnect();

    glDeleteTextures(1, &sky_texture_id_);
    glDeleteTextures(1, &irr_map_id_);
    glDeleteTextures(1, &pfr_map_id_);
    glDeleteTextures(1, &brdf_texture_id_);

    delete buffer_cube_;
    delete buffer_squad_;

    delete irradiance_shader_;
    delete prefilter_shader_;
    delete sky_shader_;
    delete brdf_shader_;
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

  void Skybox::bind_reflectance(){
    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irr_map_id_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pfr_map_id_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdf_texture_id_);
  }

  const bool Skybox::is_ready(){
    return is_ready_;
  }

  void Skybox::load_images(){
    protector_.lock();
    stbi_set_flip_vertically_on_load(false);
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
    stbi_set_flip_vertically_on_load(true);
    brdf_.data = stbi_load(std::string(folder_path_ + "brdf.png").c_str(),
                           &brdf_.width, &brdf_.height, &brdf_.components_size, 0);
    protector_.unlock();

    protector_.lock();
    is_ready_ = up_.data && down_.data && left_.data && right_.data
                && front_.data && back_.data && brdf_.data;
    protector_.unlock();
  }

  void Skybox::load_ready(){
    if(protector_.try_lock()){
      protector_.unlock();
      if(is_ready_){
        // pbr: setup framebuffer
        // ----------------------
        glGenFramebuffers(1, &frame_buffer_);
        glGenRenderbuffers(1, &render_buffer_);

        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, render_buffer_);

        prepare_cube();
        prepare_quad();

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

        write_data_opengl(right_, 0);
        write_data_opengl(left_, 1);
        write_data_opengl(up_, 2);
        write_data_opengl(down_, 3);
        write_data_opengl(back_, 4);
        write_data_opengl(front_, 5);

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        sky_shader_->use();
        sky_shader_->set_value(sky_u_skybox_, 0);

        // pbr: set up projection and view matrices for capturing data onto the 6 cubemap faces
        // ------------------------------------------------------------------------------------
        Algebraica::mat4f capture_projection;
        capture_projection.perspective(_PI2, 1.0f, 0.1f, 10.0f);
        Algebraica::mat4f capture_views[] = {
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f, 0.0f),
          Algebraica::vec3f(1.0f,  0.0f,  0.0f),
          Algebraica::vec3f(0.0f, -1.0f,  0.0f)),
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f, 0.0f),
          Algebraica::vec3f(-1.0f,  0.0f,  0.0f),
          Algebraica::vec3f(0.0f, -1.0f,  0.0f)),
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f, 0.0f),
          Algebraica::vec3f(0.0f,  1.0f,  0.0f),
          Algebraica::vec3f(0.0f,  0.0f,  1.0f)),
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f,  0.0f),
          Algebraica::vec3f(0.0f, -1.0f,  0.0f),
          Algebraica::vec3f(0.0f,  0.0f, -1.0f)),
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f, 0.0f),
          Algebraica::vec3f(0.0f,  0.0f,  1.0f),
          Algebraica::vec3f(0.0f, -1.0f,  0.0f)),
          Algebraica::mat4f::lookAt(Algebraica::vec3f(0.0f, 0.0f, 0.0f),
          Algebraica::vec3f(0.0f,  0.0f, -1.0f),
          Algebraica::vec3f(0.0f, -1.0f,  0.0f))
        };

        // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
        // --------------------------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &irr_map_id_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irr_map_id_);

        for(unsigned int i = 0; i < 6; ++i)
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                       GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
        // -----------------------------------------------------------------------------
        irradiance_shader_->use();
        irradiance_shader_->set_value(irradiance_shader_->uniform_location("u_skybox"), 0);
        irradiance_shader_->set_value(irradiance_shader_->uniform_location("u_projection"),
                                      capture_projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture_id_);

        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
        for(unsigned int i = 0; i < 6; ++i){
          irradiance_shader_->set_value(irradiance_shader_->uniform_location("u_view"),
                                        capture_views[i]);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                 GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irr_map_id_, 0);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          render_cube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
        // --------------------------------------------------------------------------------
        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &pfr_map_id_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pfr_map_id_);
        for(unsigned int i = 0; i < 6; ++i)
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
                       GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // be sure to set minifcation filter to mip_linear
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter
        // -----------------------------------------------------------------------------------------
        prefilter_shader_->use();
        prefilter_shader_->set_value(prefilter_shader_->uniform_location("u_skybox"), 0);
        prefilter_shader_->set_value(prefilter_shader_->uniform_location("u_projection"),
                                     capture_projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture_id_);

        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
        unsigned int maxMipLevels = 5;
        for(unsigned int mip = 0; mip < maxMipLevels; ++mip){
          // reisze framebuffer according to mip-level size.
          unsigned int mipWidth  = 128 * std::pow(0.5, mip);
          unsigned int mipHeight = 128 * std::pow(0.5, mip);
          glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
          glViewport(0, 0, mipWidth, mipHeight);

          float roughness = (float)mip / (float)(maxMipLevels - 1);
          prefilter_shader_->set_value(prefilter_shader_->uniform_location("u_roughness"),
                                       roughness);
          for(unsigned int i = 0; i < 6; ++i){
            prefilter_shader_->set_value(prefilter_shader_->uniform_location("u_view"),
                                         capture_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pfr_map_id_, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render_cube();
          }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // pbr: generate a 2D LUT from the BRDF equations used.
        // ----------------------------------------------------
        glActiveTexture(GL_TEXTURE2);
        glGenTextures(1, &brdf_texture_id_);

        // pre-allocate enough memory for the LUT texture.
        glBindTexture(GL_TEXTURE_2D, brdf_texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        switch(brdf_.components_size){
        case 1:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, brdf_.width,
                       brdf_.height, 0, GL_RED, GL_UNSIGNED_BYTE, brdf_.data);
          break;
        case 2:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, brdf_.width,
                       brdf_.height, 0, GL_RG, GL_UNSIGNED_BYTE, brdf_.data);
          break;
        case 4:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, brdf_.width,
                       brdf_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, brdf_.data);
          break;
        default:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, brdf_.width,
                       brdf_.height, 0, GL_RGB, GL_UNSIGNED_BYTE, brdf_.data);
          break;
        }

//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
//        // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//        // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
//        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
//        glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
//                               brdf_texture_id_, 0);

//        glViewport(0, 0, 512, 512);
//        brdf_shader_->use();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        render_quad();

//        unsigned char *Buff = new unsigned char[512*512*3];
//        glReadBuffer(GL_BACK);
//        glReadPixels(0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, Buff);
//        stbi_write_png("testing_bitch.png", 512, 512, 3, Buff, 3 * sizeof(float));

//        delete Buff;

//        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // initialize static shader uniforms before rendering
        // --------------------------------------------------
        sky_shader_->use();
        sky_shader_->set_value(sky_u_pv_, core_->camera_matrix_static_perspective_view());

        // then before rendering, configure the viewport to the original framebuffer's screen dimensions
        core_->restart_viewport();

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

    GLint offset{0};
    buffer_cube_->enable(i_position_);
    buffer_cube_->attributte_buffer(i_position_, _3D, offset, stride_size_long_);

    offset += stride_size_short_;
    buffer_cube_->enable(i_normal_);
    buffer_cube_->attributte_buffer(i_normal_, _3D, offset, stride_size_long_);

    offset += stride_size_short_;
    buffer_cube_->enable(i_uv_);
    buffer_cube_->attributte_buffer(i_uv_, _2D, offset, stride_size_long_);

    buffer_cube_->vertex_release();
  }

  void Skybox::prepare_quad(){
    float vertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      +1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    buffer_squad_->create();
    buffer_squad_->vertex_bind();
    buffer_squad_->allocate_array(&vertices, sizeof(vertices));

    buffer_squad_->enable(i_position_);
    buffer_squad_->attributte_buffer(i_position_, _3D, 0, stride_size_medium_);

    buffer_squad_->enable(1);
    buffer_squad_->attributte_buffer(1, _2D, stride_size_short_, stride_size_medium_);

    buffer_squad_->vertex_release();
  }

  void Skybox::render_cube(){
    // render Cube
    buffer_cube_->vertex_bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    buffer_cube_->vertex_release();
  }

  void Skybox::render_quad(){
    // render square
    buffer_squad_->vertex_bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    buffer_squad_->vertex_release();
  }
}
