#include "includes/model_manager.h"
#include "includes/core.h"

namespace Toreo {
  ModelManager::ModelManager(Core *core) :
    core_(core),
    model_shader_(new Shader("resources/shaders/PBR.vert",
                             "resources/shaders/PBR.frag")),
////    m_u_scene_model_(model_shader_->uniform_location("u_scene_model")),
//    m_u_object_model_(model_shader_->uniform_location("model")),
//    m_u_view_(model_shader_->uniform_location("view")),
//    m_u_projection_(model_shader_->uniform_location("projection")),
//    m_u_light_(model_shader_->uniform_location("lightPositions")),
//    m_u_light_color_(model_shader_->uniform_location("lightColors")),
////    m_u_light_size_(model_shader_->uniform_location("u_light_size")),
//    m_u_camera_(model_shader_->uniform_location("camPos")),
////    m_u_sun_(model_shader_->uniform_location("u_sun")),
////    m_u_sun_color_(model_shader_->uniform_location("u_sun_color")),
////    m_u_pbr_(model_shader_->uniform_location("u_pbr")),
////    m_u_fog_(model_shader_->uniform_location("u_fog")),
//    m_u_albedo_(model_shader_->uniform_location("albedoMap")),
//    m_u_normal_(model_shader_->uniform_location("normalMap")),
////    m_u_metallized_(model_shader_->uniform_location("u_metallized")),
//    m_u_metallic_(model_shader_->uniform_location("metallicMap")),
////    m_u_metallic_value_(model_shader_->uniform_location("u_metallic_value")),
////    m_u_roughed_(model_shader_->uniform_location("u_roughed")),
//    m_u_roughness_(model_shader_->uniform_location("roughnessMap")),
////    m_u_roughness_value_(model_shader_->uniform_location("u_roughness_value")),
//    m_u_ao_(model_shader_->uniform_location("aoMap")),
////    m_u_colored_(model_shader_->uniform_location("u_colored")),
////    m_u_color_(model_shader_->uniform_location("u_color")),
//    m_u_irradiance_(model_shader_->uniform_location("irradianceMap")),
//    m_u_prefilter_(model_shader_->uniform_location("prefilterMap")),
//    m_u_brdfLUT_(model_shader_->uniform_location("brdfLUT")),
    m_u_scene_model_(model_shader_->uniform_location("u_scene_model")),
    m_u_object_model_(model_shader_->uniform_location("u_object_model")),
    m_u_view_(model_shader_->uniform_location("u_view")),
    m_u_projection_(model_shader_->uniform_location("u_projection")),
    m_u_light_(model_shader_->uniform_location("u_light")),
    m_u_light_color_(model_shader_->uniform_location("u_light_color")),
    m_u_light_size_(model_shader_->uniform_location("u_light_size")),
    m_u_camera_(model_shader_->uniform_location("u_camera")),
    m_u_sun_(model_shader_->uniform_location("u_sun")),
    m_u_sun_color_(model_shader_->uniform_location("u_sun_color")),
    m_u_pbr_(model_shader_->uniform_location("u_pbr")),
    m_u_fog_(model_shader_->uniform_location("u_fog")),
    m_u_albedo_(model_shader_->uniform_location("u_albedo")),
    m_u_normal_(model_shader_->uniform_location("u_normal")),
    m_u_metallized_(model_shader_->uniform_location("u_metallized")),
    m_u_metallic_(model_shader_->uniform_location("u_metallic")),
    m_u_metallic_value_(model_shader_->uniform_location("u_metallic_value")),
    m_u_roughed_(model_shader_->uniform_location("u_roughed")),
    m_u_roughness_(model_shader_->uniform_location("u_roughness")),
    m_u_roughness_value_(model_shader_->uniform_location("u_roughness_value")),
    m_u_ao_(model_shader_->uniform_location("u_ao")),
    m_u_colored_(model_shader_->uniform_location("u_colored")),
    m_u_color_(model_shader_->uniform_location("u_color")),
    m_u_irradiance_(model_shader_->uniform_location("u_irradiance")),
    m_u_prefilter_(model_shader_->uniform_location("u_prefilter")),
    m_u_brdfLUT_(model_shader_->uniform_location("u_brdfLUT")),
    skybox_(new Skybox("resources/cubemap/", ".png", core)),
    skybox_visibility_(true),
    models_(0),
    sun_direction_(-0.70711f, 0.70711f, 0.866f),
    sun_color_(1.0f, 1.0f, 1.0f)
  {
    if(!model_shader_->use())
      std::cout << model_shader_->error_log() << std::endl;

    update_camera();
    update_vehicle_model();

    model_shader_->set_value(m_u_irradiance_, 0);
    model_shader_->set_value(m_u_prefilter_, 1);
    model_shader_->set_value(m_u_brdfLUT_, 2);
    model_shader_->set_value(m_u_albedo_, 3);
    model_shader_->set_value(m_u_normal_, 4);
    model_shader_->set_value(m_u_metallic_, 5);
    model_shader_->set_value(m_u_roughness_, 6);
    model_shader_->set_value(m_u_ao_, 7);

    model_shader_->set_value(m_u_sun_, sun_direction_);
    model_shader_->set_value(m_u_sun_color_, sun_color_);

    // lights
    // ------
    Algebraica::vec3f lightPositions[4] = {
      Algebraica::vec3f(-10.0f, 10.0f, -10.0f),
      Algebraica::vec3f( 10.0f, 10.0f, -10.0f),
      Algebraica::vec3f(-10.0f, 10.0f, 10.0f),
      Algebraica::vec3f( 10.0f, 10.0f, 10.0f),
    };
    Algebraica::vec3f lightColors[4] = {
      Algebraica::vec3f(300.0f, 300.0f, 300.0f),
      Algebraica::vec3f(300.0f, 300.0f, 300.0f),
      Algebraica::vec3f(300.0f, 300.0f, 300.0f),
      Algebraica::vec3f(300.0f, 300.0f, 300.0f)
    };

    model_shader_->set_values(m_u_light_, &lightPositions[0], 4);
    model_shader_->set_values(m_u_light_color_, &lightColors[0], 4);
    model_shader_->set_value(m_u_light_size_, 4);

    signal_updated_camera_ = core->signal_updated_camera()->connect(
          boost::bind(&ModelManager::update_camera, this));
    signal_updated_screen_ = core->signal_updated_screen()->connect(
          boost::bind(&ModelManager::draw_all, this));
  }

  ModelManager::~ModelManager(){
    for(Visualizer::Model3D model : models_)
      if(model.model)
        delete model.model;

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();
    if(signal_updated_screen_.connected())
      signal_updated_screen_.disconnect();

    delete model_shader_;
    delete skybox_;
  }

  MMid ModelManager::load_new_model(const char *folder_address){
    Visualizer::Model3D new_model;
    new_model.model = new ThreeDimensionalModelLoader(folder_address, model_shader_, core_);
    models_.push_back(new_model);
    return models_.size() - 1;
  }

  MMid ModelManager::load_new_model(const std::string folder_address){
    Visualizer::Model3D new_model;
    new_model.model = new ThreeDimensionalModelLoader(folder_address, model_shader_, core_);
    models_.push_back(new_model);
    return models_.size() - 1;
  }

  MMid ModelManager::load_new_model(const unsigned int model){
    Visualizer::Model3D new_model;
    new_model.model = new ThreeDimensionalModelLoader(model, model_shader_, core_);
    models_.push_back(new_model);
    return models_.size() - 1;
  }

  MMelement ModelManager::add(MMid id, const Algebraica::mat4f *transformation_matrix){
    if(models_.size() > id)
      if(models_[id].model){
        Visualizer::Model3DElement new_element;
        new_element.main = transformation_matrix;
        models_[id].elements.push_back(new_element);
        return models_[id].elements.size() - 1;
      }else
        return -1;
    else
      return -1;
  }

  bool ModelManager::colorize(MMid model_id, MMelement element_id, const bool colorize,
                              const int R, const int G, const int B, const int Alpha){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].colorize = colorize;
          models_[model_id].elements[element_id].R = R;
          models_[model_id].elements[element_id].G = G;
          models_[model_id].elements[element_id].B = B;
          models_[model_id].elements[element_id].A = Alpha;
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::metallize(MMid model_id, MMelement element_id, const bool metallize,
                               const float metallic_value){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].metallize = metallize;
          models_[model_id].elements[element_id].metallic = metallic_value;
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::roughen(MMid model_id, MMelement element_id, const bool roughen,
                             const float roughness_value){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].roughen = roughen;
          models_[model_id].elements[element_id].roughness = roughness_value;
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::set_visibility(MMid model_id, MMelement element_id, const bool visible){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].visibility = visible;
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::set_transformation_matrix(MMid model_id, MMelement element_id,
                                               const Algebraica::mat4f *transformation_matrix){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        models_[model_id].elements[element_id].main = transformation_matrix;
        return true;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::translate(MMid model_id, MMelement element_id,
                               const float x, const float y, const float z){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].secondary.translate(-y, z, -x);
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::rotate(MMid model_id, MMelement element_id,
                            const float pitch, const float yaw, const float roll){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].secondary.rotate(pitch, yaw, roll);
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::rotate_in_x(MMid model_id, MMelement element_id, const float x){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].secondary.rotate_x(x);
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::rotate_in_y(MMid model_id, MMelement element_id, const float y){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].secondary.rotate_y(y);
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::rotate_in_z(MMid model_id, MMelement element_id, const float z){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          models_[model_id].elements[element_id].secondary.rotate_z(z);
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::draw_element(MMid model_id, MMelement element_id){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        if(models_[model_id].elements[element_id].main){
          if(models_[model_id].elements[element_id].visibility){
            model_shader_->use();
            models_[model_id].model->pre_drawing();
            draw(&models_[model_id], &models_[model_id].elements[element_id]);
            models_[model_id].model->post_drawing();
          }
          return true;
        }else
          return false;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::draw_model(MMid model_id){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > 0){
        model_shader_->use();
        models_[model_id].model->pre_drawing();
        for(Visualizer::Model3DElement element : models_[model_id].elements)
          if(element.main && element.visibility)
            draw(&models_[model_id], &element);

        models_[model_id].model->post_drawing();
        return true;
      }else
        return false;
    else
      return false;
  }

  void ModelManager::draw_all(){
    model_shader_->use();
    skybox_->bind_reflectance();
    for(Visualizer::Model3D model : models_){
      if(model.model && model.elements.size() > 0){
        model.model->pre_drawing();
        for(Visualizer::Model3DElement element : model.elements)
          if(element.main && element.visibility)
            draw(&model, &element);

        model.model->post_drawing();
      }
      if(skybox_visibility_)
        skybox_->draw();
    }
  }

  bool ModelManager::delete_element(MMid model_id, MMelement element_id){
    if(models_.size() > model_id)
      if(models_[model_id].model && models_[model_id].elements.size() > element_id){
        models_[model_id].elements[element_id].main = nullptr;
        models_[model_id].elements[element_id].visibility = false;
        return true;
      }else
        return false;
    else
      return false;
  }

  bool ModelManager::delete_model(MMid id){
    if(models_.size() > id)
      if(models_[id].model){
        delete models_[id].model;
        models_[id].model = nullptr;
        models_[id].elements.clear();
        return true;
      }else
        return false;
    else
      return false;
  }

  void ModelManager::purge(){
    for(Visualizer::Model3D model : models_){
      if(model.model){
        delete model.model;
        model.elements.clear();
      }
    }
    models_.clear();
  }

  void ModelManager::skybox_visibility(const bool hidden){
    skybox_visibility_ = !hidden;
  }

  void ModelManager::sun_properties(const Algebraica::vec3f direction,
                                    const int R, const int G, const int B){
    sun_direction_(-direction.x(), direction.z(), -direction.y());
    sun_color_ = Algebraica::vec3f(R / 255.0f, G / 255.0f, B / 255.0f);
    model_shader_->use();
    model_shader_->set_value(m_u_sun_, sun_direction_);
    model_shader_->set_value(m_u_sun_color_, sun_color_);
  }

  void ModelManager::draw(Visualizer::Model3D *model, Visualizer::Model3DElement *element){
    model_shader_->set_value(m_u_colored_, element->colorize);
    if(element->colorize)
      model_shader_->set_value(m_u_color_,
                               Algebraica::vec4f(element->R, element->G, element->B, element->A));

    model_shader_->set_value(m_u_metallized_, element->metallize);
    if(element->metallize)
      model_shader_->set_value(m_u_metallic_value_, element->metallic);

    model_shader_->set_value(m_u_roughed_, element->roughen);
    if(element->roughen)
      model_shader_->set_value(m_u_roughness_value_, element->roughness);

    model_shader_->set_value(m_u_fog_, element->foggy);
    model_shader_->set_value(m_u_pbr_, element->pbr);

    model_shader_->set_value(m_u_scene_model_, *element->main);
    model_shader_->set_value(m_u_object_model_, element->secondary);

    model->model->draw();
  }

  void ModelManager::update_camera(){
    model_shader_->use();
    model_shader_->set_value(m_u_view_, core_->camera_matrix_view());
    model_shader_->set_value(m_u_projection_, core_->camera_matrix_perspective());
    model_shader_->set_value(m_u_camera_, core_->camera_position());
    //  model_shader_->set_value(m_u_irradiance_, );
    //  model_shader_->set_value(m_u_prefilter_, );
    //  model_shader_->set_value(m_u_brdfLUT_, );
  }

  void ModelManager::update_vehicle_model(){
    model_shader_->use();
      model_shader_->set_value(m_u_scene_model_, core_->vehicle_frame());
    //  model_shader_->set_value(m_u_light_, );
  }
  }