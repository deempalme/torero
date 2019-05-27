#include "torero/core.h"
#include "torero/file/manager.h"
#include "torero/model/environment.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"
#include "stb_image_write.h"

namespace torero {
  namespace model {
    Environment::Environment(torero::Core *core) :
      core_(core),
      environment_path_(),
      environment_exist_(false),
      environment_is_loaded_(false),
      brdf_texture_id_(0),
      irradiance_map_id_(0),
      prefilter_map_id_(0),
      skybox_path_(),
      skybox_exist_(false),
      sky_is_loaded_(false),
      skybox_visible_(true),
      sky_cubemap_id_(0),
      skybox_shader_(nullptr),
      buffer_cube_(false),
      buffer_quad_(nullptr),
      skybox_(),
      environment_(),
      brdf_(),
      environment_protector_(),
      sky_protector_()
    {
      prepare_cube();
      prepare_quad();
    }

    Environment::~Environment(){
      if(skybox_shader_) delete skybox_shader_;
      if(buffer_quad_) delete buffer_quad_;

      remove_environment();
      remove_skybox();
    }

    void Environment::bind_reflectance(){
      if(environment_is_loaded_){
        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Irradiance);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map_id_);
        glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Prefilter);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map_id_);
        glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::BRDF);
        glBindTexture(GL_TEXTURE_2D, brdf_texture_id_);
      }
    }

    void Environment::bind_skybox(){
      if(sky_is_loaded_){
        // skybox
        glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_cubemap_id_);
      }
    }

    void Environment::draw_skybox(){
      if(skybox_visible_ && sky_is_loaded_){
        skybox_shader_->use();
        glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky_cubemap_id_);

        buffer_cube_.vertex_bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        buffer_cube_.vertex_release();
      }
    }

    bool Environment::load_enviroment_map(std::string environment_map){
      if(!torero::file::Manager::check_path(&environment_map)){
        environment_exist_ = false;
        return false;
      }

      if(environment_path_ == environment_map)
        return true;

      remove_environment();

      environment_exist_ = true;
      environment_path_ = environment_map;
      environment_is_loaded_ = false;

      core_->multithread_add_process(boost::bind(&Environment::environment_run, this),
                                     boost::bind(&Environment::environment_ready, this),
                                     &environment_protector_);
      return true;
    }

    bool Environment::load_skybox(std::string skybox){
      if(skybox_path_ == skybox)
        return true;

      if(!torero::file::Manager::check_path(&skybox)){
        skybox_exist_ = false;
        return false;
      }

      remove_skybox();

      skybox_exist_ = true;
      skybox_path_ = skybox;
      sky_is_loaded_ = false;

      core_->multithread_add_process(boost::bind(&Environment::skybox_run, this),
                                     boost::bind(&Environment::skybox_ready, this),
                                     &sky_protector_);
      return true;
    }

    bool Environment::skybox_visibility(const bool visible){
      return sky_is_loaded_ && (skybox_visible_ = visible);
    }

    void Environment::remove_environment(){
      if(environment_is_loaded_){
        glDeleteTextures(1, &irradiance_map_id_);
        glDeleteTextures(1, &prefilter_map_id_);
        glDeleteTextures(1, &brdf_texture_id_);
      }
      environment_exist_ = false;
      environment_is_loaded_ = false;
      irradiance_map_id_ = prefilter_map_id_ = brdf_texture_id_ = 0;
    }

    void Environment::remove_skybox(){
      if(sky_is_loaded_)
        glDeleteTextures(1, &sky_cubemap_id_);
      skybox_exist_ = false;
      sky_is_loaded_ = false;
      sky_cubemap_id_ = 0;
    }

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PRIVATES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< //

    bool Environment::create_cubemap(GLuint *cubemap_id, torero::image::FileFloat *image){

      // Creating the generator shader
      torero::gl::Shader cubemap_creator_shader(torero::k_cubemap_vertex,
                                                torero::k_cubemap_fragment);

      if(cubemap_creator_shader.error()){
        torero::terminal::Printer::error(cubemap_creator_shader.error_log());
        return false;
      }

      cubemap_creator_shader.set_value("u_map", torero::ShaderTexture::Albedo);
      const GLint u_pv{cubemap_creator_shader.uniform_location("u_pv")};

      // Creating the render buffers
      GLuint frame, render;
      glGenFramebuffers(1, &frame);
      glGenRenderbuffers(1, &render);
      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      glBindRenderbuffer(GL_RENDERBUFFER, render);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                            torero::k_cubemap_resolution, torero::k_cubemap_resolution);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render);

      // Generating an empty cubemap that will be filled later
      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Cubemap);
      glGenTextures(1, cubemap_id);
      glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap_id);
      for(GLuint i = 0; i < 6; ++i){
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     torero::k_cubemap_resolution, torero::k_cubemap_resolution,
                     0, GL_RGB, GL_FLOAT, nullptr);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // Calculating the perspective * view matrices
      const algebraica::mat4f projection(algebraica::mat4f::Perspective(torero::k_90_degrees,
                                                                        1.0f, 0.1f, 10.0f));
      const algebraica::mat4f views[] = {
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 1.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f(-1.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 1.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f, 1.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f,-1.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f, 1.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f,-1.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) )
      };

      // Generating equirectangular texture
      GLuint equirectangular_id;
      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Albedo);
      glGenTextures(1, &equirectangular_id);
      glBindTexture(GL_TEXTURE_2D, equirectangular_id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, image->width, image->height, 0,
                   GL_RGB, GL_FLOAT, image->data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      stbi_image_free(image->data);

      // Generating the Skybox
      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      glViewport(0, 0, torero::k_cubemap_resolution, torero::k_cubemap_resolution);

      for(GLuint i = 0; i < 6; ++i){
        cubemap_creator_shader.set_value(u_pv, views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, *cubemap_id, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_cube();
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDeleteTextures(1, &equirectangular_id);
      glDeleteFramebuffers(1, &frame);
      glDeleteRenderbuffers(1, &render);

      // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
      glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap_id);
      glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

      core_->restart_viewport();

      return true;
    }

    bool Environment::precompute_brdf(std::string output_path){
      // Creating the generator shader
      torero::gl::Shader brdf_creator_shader(torero::k_brdf_vertex, torero::k_brdf_fragment);

      if(brdf_creator_shader.error()){
        torero::terminal::Printer::error(brdf_creator_shader.error_log());
        return false;
      }

      brdf_creator_shader.use();

      glViewport(0, 0, torero::k_brdf_resolution, torero::k_brdf_resolution);

      // Creating the render buffers
      GLuint frame, pbo;
      glGenFramebuffers(1, &frame);

      glGenBuffers(1, &pbo);
      const std::size_t data_size = torero::k_brdf_resolution * torero::k_brdf_resolution
                                    * sizeof(GLfloat) * 4;
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
      glBufferData(GL_PIXEL_PACK_BUFFER, data_size, nullptr, GL_STREAM_READ);

      glBindFramebuffer(GL_FRAMEBUFFER, frame);

      // Generating texture
      GLuint brdf_id;
      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::BRDF);
      glGenTextures(1, &brdf_id);
      glBindTexture(GL_TEXTURE_2D, brdf_id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, torero::k_brdf_resolution,
                   torero::k_brdf_resolution, 0, GL_BGRA, GL_FLOAT, nullptr);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_id, 0);

      // Generating the BRDF
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render square
      buffer_quad_->vertex_bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
      buffer_quad_->vertex_release();

      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_FLOAT, (GLvoid*)0);

      GLfloat *pixel_data = static_cast<GLfloat*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
      bool no_error{true};
      /*
    // Writing image
    std::cout << "wr: "
              << stbi_write_hdr(core_->check_path_existence(torero::k_brdf_path, true).c_str(),
                                torero::k_brdf_resolution, torero::k_brdf_resolution, 4, pixel_data)
              << " > error: " << stbi_failure_reason() << std::endl;
    std::cout << "after " << glGetError() << std::endl;
*/
      torero::file::Manager::check_path(&output_path);
      std::ofstream file(output_path.c_str(), std::ios_base::out | std::ios_base::binary);
      if(file.is_open()){
        file.write((char*)pixel_data, data_size);
        file.close();
      }else{
        torero::terminal::Printer::error("The file: " + output_path + " was not found");
        no_error = false;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDeleteFramebuffers(1, &frame);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
      glDeleteTextures(1, &brdf_id);
      glDeleteBuffers(1, &pbo);

      core_->restart_viewport();

      return no_error;
    }

    bool Environment::load_brdf(){
      // pbr: generate a 2D LUT from the BRDF equations used.
      // ----------------------------------------------------
      glActiveTexture(GL_TEXTURE0 + ShaderTexture::BRDF);
      glGenTextures(1, &brdf_texture_id_);

      // pre-allocate enough memory for the LUT texture.
      glBindTexture(GL_TEXTURE_2D, brdf_texture_id_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, brdf_.width, brdf_.height, 0,
                   GL_BGRA, GL_FLOAT, brdf_.data);
      free(brdf_.data);
    }

    void Environment::environment_ready(){

      if(!environment_.data || !environment_exist_ || environment_is_loaded_) return;

      // Irradiance shader
      // -----------------
      torero::gl::Shader *irradiance_shader = new torero::gl::Shader(torero::k_irradiance_vertex,
                                                                     torero::k_irradiance_fragment);
      if(!irradiance_shader->use()){
        torero::terminal::Printer::error(irradiance_shader->error_log());
        delete irradiance_shader;
        free(brdf_.data);
        stbi_image_free(environment_.data);
        return;
      }

      // Prefilter shader
      // ----------------
      torero::gl::Shader *prefilter_shader = new torero::gl::Shader(torero::k_prefilter_vertex,
                                                                    torero::k_prefilter_fragment);
      if(!prefilter_shader->use()){
        torero::terminal::Printer::error(prefilter_shader->error_log());
        delete irradiance_shader;
        delete prefilter_shader;
        free(brdf_.data);
        stbi_image_free(environment_.data);
        return;
      }

      GLuint environment_cubemap_id;
      // Creating cubemap
      // ----------------
      create_cubemap(&environment_cubemap_id, &environment_);

      // Creating the render buffers
      // ---------------------------
      GLuint frame, render;
      glGenFramebuffers(1, &frame);
      glGenRenderbuffers(1, &render);
      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      glBindRenderbuffer(GL_RENDERBUFFER, render);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                            torero::k_cubemap_resolution, torero::k_cubemap_resolution);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render);

      // Calculating the perspective * view matrices
      // -------------------------------------------
      const algebraica::mat4f projection(algebraica::mat4f::Perspective(torero::k_90_degrees,
                                                                        1.0f, 0.1f, 10.0f));
      const algebraica::mat4f views[] = {
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 1.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f(-1.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 1.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f, 1.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f,-1.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f, 1.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) ),
        projection * algebraica::mat4f::lookAt(algebraica::vec3f( 0.0f, 0.0f, 0.0f),
        algebraica::vec3f( 0.0f, 0.0f,-1.0f),
        algebraica::vec3f( 0.0f,-1.0f, 0.0f) )
      };

      // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
      // --------------------------------------------------------------------------------
      glGenTextures(1, &irradiance_map_id_);
      glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map_id_);
      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Irradiance);

      for(unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
                     GL_RGB, GL_FLOAT, nullptr);

      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      glBindRenderbuffer(GL_RENDERBUFFER, render);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

      // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
      // -----------------------------------------------------------------------------
      irradiance_shader->use();
      irradiance_shader->set_value("u_skybox", torero::ShaderTexture::Cubemap);
      GLint u_pv{irradiance_shader->uniform_location("u_pv")};

      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Cubemap);
      glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);

      glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      for(unsigned int i = 0; i < 6; ++i){
        irradiance_shader->set_value(u_pv, views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance_map_id_, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_cube();
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      delete irradiance_shader;

      // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
      // -------------------------------------------------------------------------------
      glGenTextures(1, &prefilter_map_id_);
      glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map_id_);
      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Prefilter);

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

      // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
      // ----------------------------------------------------------------------------------------------------
      prefilter_shader->use();
      prefilter_shader->set_value("u_skybox", torero::ShaderTexture::Cubemap);
      u_pv = prefilter_shader->uniform_location("u_pv");
      const GLint u_roughness{prefilter_shader->uniform_location("u_roughness")};

      glActiveTexture(GL_TEXTURE0 + torero::ShaderTexture::Cubemap);
      glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap_id);

      glBindFramebuffer(GL_FRAMEBUFFER, frame);
      const unsigned int max_mip_levels = 5;

      for(unsigned int mip = 0; mip < max_mip_levels; ++mip){
        // reisze framebuffer according to mip-level size.
        const unsigned int mip_width  = 128 * std::pow(0.5, mip);
        const unsigned int mip_height = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, render);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        glViewport(0, 0, mip_width, mip_height);

        const float roughness = (float)mip / (float)(max_mip_levels - 1);
        prefilter_shader->set_value(u_roughness, roughness);

        for(unsigned int i = 0; i < 6; ++i){
          prefilter_shader->set_value(u_pv, views[i]);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                 GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter_map_id_, mip);

          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          render_cube();
        }
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      delete prefilter_shader;

      // then before rendering, configure the viewport to the original framebuffer's screen dimensions
      core_->restart_viewport();

      // Deleting skybox texture
      glDeleteTextures(1, &environment_cubemap_id);
      // Deleting extra texture buffers
      glDeleteFramebuffers(1, &frame);
      glDeleteRenderbuffers(1, &render);
      delete buffer_quad_;
      buffer_quad_ = nullptr;

      load_brdf();

      environment_is_loaded_ = true;
    }

    void Environment::environment_run(){
      stbi_set_flip_vertically_on_load(1);

      environment_protector_.lock();
      // Loading HDR environment map
      environment_.data = stbi_loadf(environment_path_.c_str(),
                                     &environment_.width, &environment_.height,
                                     &environment_.components, 0);

      std::string brdf_path = torero::k_brdf_path;
      torero::file::Manager::check_path(&brdf_path);
      const std::size_t brdf_size(torero::k_brdf_resolution * torero::k_brdf_resolution
                                  * sizeof(float) * torero::k_brdf_components);

      // Loading BRDF image saved as binary file
      std::ifstream reader(brdf_path, std::ios_base::ate | std::ios_base::in | std::ios_base::binary);
      if(reader.is_open()){

        brdf_.data = reinterpret_cast<float*>(malloc(brdf_size));
        reader.read(reinterpret_cast<char*>(brdf_.data), static_cast<std::streamsize>(brdf_size));

        brdf_.components = torero::k_brdf_components;
        brdf_.width = torero::k_brdf_resolution;
        brdf_.height = torero::k_brdf_resolution;
        reader.close();
      }

      environment_protector_.unlock();
    }

    void Environment::skybox_ready(){
      if(!skybox_.data || !skybox_exist_ || sky_is_loaded_) return;

      create_cubemap(&sky_cubemap_id_, &skybox_);

      skybox_shader_ = new torero::gl::Shader(torero::k_skybox_vertex, torero::k_skybox_fragment);

      if(skybox_shader_->error()){
        torero::terminal::Printer::error(skybox_shader_->error_log());
        return;
      }

      skybox_shader_->set_value("u_skybox", torero::ShaderTexture::Cubemap);

      sky_is_loaded_ = true;
    }

    void Environment::skybox_run(){
      stbi_set_flip_vertically_on_load(1);
      sky_protector_.lock();

      // Loading HDR environment map
      skybox_.data = stbi_loadf(std::string(skybox_path_).c_str(), &skybox_.width,
                                &skybox_.height, &skybox_.components, 0);
      sky_protector_.unlock();
    }

    void Environment::prepare_cube(){
      GLfloat vertices[] = {
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

      buffer_cube_.create();
      buffer_cube_.vertex_bind();
      buffer_cube_.allocate_array(&vertices, sizeof(vertices));

      const GLsizei stride_size_long{sizeof(GLfloat) * 8};
      const GLsizei stride_size_short{sizeof(GLfloat) * 3};

      GLint offset{0};
      buffer_cube_.enable(torero::ShaderLocation::Position);
      buffer_cube_.attributte_buffer(torero::ShaderLocation::Position, _3D, offset, stride_size_long);

      offset += stride_size_short;
      buffer_cube_.enable(torero::ShaderLocation::Normal);
      buffer_cube_.attributte_buffer(torero::ShaderLocation::Normal, _3D, offset, stride_size_long);

      offset += stride_size_short;
      buffer_cube_.enable(torero::ShaderLocation::UV);
      buffer_cube_.attributte_buffer(torero::ShaderLocation::UV, _2D, offset, stride_size_long);

      buffer_cube_.vertex_release();
    }

    void Environment::prepare_quad(){
      GLfloat vertices[] = {
        // positions  // texture Coords
        -1.0f, +1.0f, 0.0f, 1.0f,
        -1.0f, +1.0f, 0.0f, 1.0f,
        +1.0f, +1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        +1.0f, -1.0f, 1.0f, 0.0f,
        +1.0f, -1.0f, 1.0f, 0.0f
      };

      const GLsizei stride_size_medium{sizeof(GLfloat) * 4};

      buffer_quad_ = new torero::gl::Buffer(true);
      buffer_quad_->vertex_bind();
      buffer_quad_->allocate_array(&vertices, sizeof(vertices));

      buffer_quad_->enable(ShaderLocation::Position);
      buffer_quad_->attributte_buffer(ShaderLocation::Position, _2D, 0, stride_size_medium);

      buffer_quad_->enable(ShaderLocation::UV);
      buffer_quad_->attributte_buffer(ShaderLocation::UV, _2D, sizeof(GLfloat) * 2, stride_size_medium);

      buffer_quad_->vertex_release();
    }

    void Environment::render_cube(){
      // render Cube
      buffer_cube_.vertex_bind();
      glDrawArrays(GL_TRIANGLES, 0, 36);
      buffer_cube_.vertex_release();
    }
  }
}
