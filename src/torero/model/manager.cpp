#include "torero/model/manager.h"

#include "torero/camera/view.h"
#include "torero/core.h"
#include "torero/model/environment.h"
#include "torero/vehicle/manager.h"
#include "torero/terminal/printer.h"

namespace torero {
  namespace model {
    Manager::Manager(Core *core) :
      core_(core),
      pbr_shader_(new torero::gl::Shader(torero::k_pbr_vertex, torero::k_pbr_fragment)),
      u_object_rotation_(pbr_shader_->uniform_location("u_object_rotation")),
      u_object_translation_(pbr_shader_->uniform_location("u_object_translation")),
      u_frame_rotation_(pbr_shader_->uniform_location("u_frame_rotation")),
      u_frame_translation_(pbr_shader_->uniform_location("u_frame_translation")),
      u_quaternion_(pbr_shader_->uniform_location("u_quaternion")),
      u_quaternion_usage_(pbr_shader_->uniform_location("u_quaternion_usage")),
      u_metallized_(pbr_shader_->uniform_location("u_metallized")),
      u_metallic_value_(pbr_shader_->uniform_location("u_metallic_value")),
      u_roughed_(pbr_shader_->uniform_location("u_roughed")),
      u_roughness_value_(pbr_shader_->uniform_location("u_roughness_value")),
      u_colored_(pbr_shader_->uniform_location("u_colored")),
      u_color_(pbr_shader_->uniform_location("u_color")),
      u_light_(pbr_shader_->uniform_location("u_light")),
      u_light_color_(pbr_shader_->uniform_location("u_light_color")),
      models_(0)
    {
      if(!pbr_shader_->use())
        torero::terminal::Printer::error(pbr_shader_->error_log());

      pbr_shader_->set_value("u_irradiance", torero::ShaderTexture::Irradiance);
      pbr_shader_->set_value("u_prefilter", torero::ShaderTexture::Prefilter);
      pbr_shader_->set_value("u_brdfLUT", torero::ShaderTexture::BRDF);
      pbr_shader_->set_value("u_albedo", torero::ShaderTexture::Albedo);
      pbr_shader_->set_value("u_normal", torero::ShaderTexture::Normal);
      pbr_shader_->set_value("u_pbr_emission", torero::ShaderTexture::PBREmission);

      // lights
      // ------
      algebraica::vec3f light_positions[4] = {
        algebraica::vec3f(-10.0f, 10.0f, -10.0f),
        algebraica::vec3f( 10.0f, 10.0f, -10.0f),
        algebraica::vec3f(-10.0f, 10.0f, 10.0f),
        algebraica::vec3f( 10.0f, 10.0f, 10.0f)
      };
      algebraica::vec3f light_colors[4] = {
        algebraica::vec3f(1.0f, 1.0f, 1.0f) * 10.0f,
        algebraica::vec3f(1.0f, 1.0f, 1.0f) * 10.0f,
        algebraica::vec3f(1.0f, 1.0f, 1.0f) * 10.0f,
        algebraica::vec3f(1.0f, 1.0f, 1.0f) * 10.0f
      };

      pbr_shader_->set_values(u_light_, &light_positions[0], 4);
      pbr_shader_->set_values(u_light_color_, &light_colors[0], 4);

      core_->set_module(this);

      connection_draw_ = core->syncronize(torero::Order::Models,
                                          boost::bind(&model::Manager::draw_all, this));
    }

    Manager::~Manager(){
      for(torero::model::Model3D &model : models_)
        if(model.model)
          delete model.model;

      connection_draw_.disconnect();

      delete pbr_shader_;
    }

    MMid Manager::load_new_model(const std::string folder_address){
      torero::model::Model3D new_model;
      new_model.model = new torero::model::Loader(folder_address, pbr_shader_, core_);
      models_.push_back(new_model);
      return models_.size() - 1;
    }

    MMid Manager::load_new_model(const torero::model::Models model){
      if(model == torero::model::DB5){
        return load_db5();
      }else{
        torero::model::Model3D new_model;
        new_model.model = new torero::model::Loader(model, pbr_shader_, core_);
        new_model.type = model;
        models_.push_back(new_model);
        return models_.size() - 1;
      }
    }

    MMelement Manager::add(MMid id, const torero::Pose3D *object_frame){
      if(models_.size() > id)
        if(models_[id].model){
          torero::model::Element3D new_element;
          new_element.main = object_frame;
          switch (models_[id].type){
            case torero::model::CoordinateSystem:
              new_element.metallize = true;
              new_element.metallic = 0.0f;
              new_element.roughen = true;
              new_element.roughness = 0.5f;
            break;
            default: break;
          }
          models_[id].elements.push_back(new_element);
          return models_[id].elements.size() - 1;
        }else
          return ~0u;
      else
        return ~0u;
    }

    bool Manager::colorize(MMid model_id, MMelement element_id, const bool colorize,
                           const float R, const float G, const float B, const float Alpha){
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

    bool Manager::metallize(MMid model_id, MMelement element_id, const bool metallize,
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

    bool Manager::roughen(MMid model_id, MMelement element_id, const bool roughen,
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

    bool Manager::set_visibility(MMid model_id, MMelement element_id, const bool visible){
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

    bool Manager::set_transformation_matrix(MMid model_id, MMelement element_id,
                                            const torero::Pose3D *object_frame){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          models_[model_id].elements[element_id].main = object_frame;
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::translate(MMid model_id, MMelement element_id,
                            const float x, const float y, const float z){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          if(models_[model_id].elements[element_id].main){
            models_[model_id].elements[element_id].secondary.position.point.x -= y;
            models_[model_id].elements[element_id].secondary.position.point.y += z;
            models_[model_id].elements[element_id].secondary.position.point.z -= x;
            return true;
          }else
            return false;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_with_euler(MMid model_id, MMelement element_id,
                                    const float pitch, const float yaw, const float roll){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          if(models_[model_id].elements[element_id].main){
            const algebraica::quaternionF rotation(
                  models_[model_id].elements[element_id].secondary.orientation.axes.x,
                  models_[model_id].elements[element_id].secondary.orientation.axes.y,
                  models_[model_id].elements[element_id].secondary.orientation.axes.z,
                  models_[model_id].elements[element_id].secondary.orientation.axes.w);
            const algebraica::quaternionF new_rotation(
                  algebraica::quaternionF::euler_to_quaternion(-pitch, yaw, -roll) * rotation);
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.x;
            models_[model_id].elements[element_id].secondary.orientation.axes.y = new_rotation.y;
            models_[model_id].elements[element_id].secondary.orientation.axes.z = new_rotation.z;
            models_[model_id].elements[element_id].secondary.orientation.axes.w = new_rotation.w;
            return true;
          }else
            return false;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_with_quaternion(MMid model_id, MMelement element_id, const float x,
                                         const float y, const float z, const float w){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          if(models_[model_id].elements[element_id].main){
            const algebraica::quaternionF rotation(
                  models_[model_id].elements[element_id].secondary.orientation.axes.x,
                  models_[model_id].elements[element_id].secondary.orientation.axes.y,
                  models_[model_id].elements[element_id].secondary.orientation.axes.z,
                  models_[model_id].elements[element_id].secondary.orientation.axes.w);
            const algebraica::quaternionF new_rotation(
                  algebraica::quaternionF(x, y, z, w) * rotation);
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.x;
            models_[model_id].elements[element_id].secondary.orientation.axes.y = new_rotation.y;
            models_[model_id].elements[element_id].secondary.orientation.axes.z = new_rotation.z;
            models_[model_id].elements[element_id].secondary.orientation.axes.w = new_rotation.w;
            return true;
          }else
            return false;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_axes(MMid model_id, MMelement element_id, const float x,
                                 const float y, const float z, const float angle){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          if(models_[model_id].elements[element_id].main){
            const algebraica::quaternionF rotation(
                  models_[model_id].elements[element_id].secondary.orientation.axes.x,
                  models_[model_id].elements[element_id].secondary.orientation.axes.y,
                  models_[model_id].elements[element_id].secondary.orientation.axes.z,
                  models_[model_id].elements[element_id].secondary.orientation.axes.w);
            const algebraica::quaternionF new_rotation(
                  algebraica::quaternionF::from_axis_and_angle(x, y, z, angle) * rotation);
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.x;
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.y;
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.z;
            models_[model_id].elements[element_id].secondary.orientation.axes.x = new_rotation.w;
            return true;
          }else
            return false;
        }else
          return false;
      else
        return false;
    }

    bool Manager::draw_element(MMid model_id, MMelement element_id){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > element_id){
          if(models_[model_id].elements[element_id].main){
            if(models_[model_id].elements[element_id].visibility){
              pbr_shader_->use();
              core_->environment().bind_reflectance();
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

    bool Manager::draw_model(MMid model_id){
      if(models_.size() > model_id)
        if(models_[model_id].model && models_[model_id].elements.size() > 0){
          pbr_shader_->use();
          core_->environment().bind_reflectance();
          models_[model_id].model->pre_drawing();
          for(torero::model::Element3D &element : models_[model_id].elements)
            if(element.main && element.visibility)
              draw(&models_[model_id], &element);

          models_[model_id].model->post_drawing();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      pbr_shader_->use();
      core_->environment().bind_reflectance();
      for(torero::model::Model3D &model : models_){
        if(model.model && model.elements.size() > 0){
          model.model->pre_drawing();
          for(torero::model::Element3D &element : model.elements)
            if(element.main && element.visibility)
              draw(&model, &element);

          model.model->post_drawing();
        }
      }
      core_->environment().draw_skybox();
    }

    bool Manager::delete_element(MMid model_id, MMelement element_id){
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

    bool Manager::delete_model(MMid id){
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

    void Manager::purge(){
      for(torero::model::Model3D &model : models_){
        if(model.model){
          delete model.model;
          model.elements.clear();
        }
      }
      models_.clear();
    }

    void Manager::draw(torero::model::Model3D *model, torero::model::Element3D *element){
      pbr_shader_->set_value(u_colored_, element->colorize);
      if(element->colorize)
        pbr_shader_->set_value(u_color_,
                               algebraica::vec4f(element->R, element->G,
                                                 element->B, element->A)/255.0f);

      pbr_shader_->set_value(u_metallized_, element->metallize);
      if(element->metallize)
        pbr_shader_->set_value(u_metallic_value_, element->metallic);

      pbr_shader_->set_value(u_roughed_, element->roughen);
      if(element->roughen)
        pbr_shader_->set_value(u_roughness_value_, element->roughness);

      pbr_shader_->set_value(u_frame_translation_, u_frame_rotation_, *element->main);
      pbr_shader_->set_value(u_object_translation_, u_object_rotation_, element->secondary);

      model->model->draw();
    }

    MMid Manager::load_db5(){
      MMid first{models_.size()};
      torero::model::Model3D new_model;
      torero::model::Element3D element;

      // Body
      new_model.model = new torero::model::Loader(torero::model::DB5_Body, pbr_shader_, core_);
      new_model.type = torero::model::DB5_Body;
      element.main = core_->vehicle().vehicle_frame();
      new_model.elements.push_back(element);
      models_.push_back(new_model);

      // Tires
      new_model.model = new torero::model::Loader(torero::model::Tire, pbr_shader_, core_);
      new_model.type = torero::model::Tire;
      new_model.elements.clear();

      // Rear right
      element.secondary = torero::Pose3D{ {{0.72f, 0.0f, 0.0f}}, {} };
      new_model.elements.push_back(element);

      // Frontal right
      element.secondary = torero::Pose3D{ {{0.0f, 0.0f, -2.48f}}, {} };
      new_model.elements.push_back(element);

      // Frontal left
      //    tire.rotate_y(_PI);
      element.secondary = torero::Pose3D{ {{-1.44f, 0.0f, 0.0f}}, {} };
      new_model.elements.push_back(element);

      // Rear left
      element.secondary = torero::Pose3D{ {{0.0f, 0.0f, -2.48f}}, {} };
      new_model.elements.push_back(element);

      models_.push_back(new_model);

      // Accessories
      new_model.model = new torero::model::Loader(torero::model::DB5_Accessories,
                                                  pbr_shader_, core_);
      new_model.type = torero::model::DB5_Accessories;
      new_model.elements.clear();

      element.metallize = true;
      element.metallic = 1.0f;
      element.emitting = true;
      element.secondary = torero::Pose3D{};
      new_model.elements.push_back(element);
      models_.push_back(new_model);

      // Windows
      new_model.model = new torero::model::Loader(torero::model::DB5_Windows, pbr_shader_, core_);
      new_model.type = torero::model::DB5_Windows;
      new_model.elements.clear();

      element.metallize = true;
      element.metallic = 1.0f;
      element.roughen = true;
      element.roughness = 0.3f;
      element.secondary = torero::Pose3D{};
      new_model.elements.push_back(element);
      models_.push_back(new_model);

      return first;
    }
  }
}
