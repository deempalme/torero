#include "torero/camera/view.h"
#include "torero/core.h"
#include "torero/trajectory/manager.h"
// Image loader
#include "stb_image.h"

namespace torero {
  namespace trajectory {
    Manager::Manager(torero::Core *core) :
      core_(core),
      shader_(new torero::gl::Shader(torero::k_trajectory_vertex, torero::k_trajectory_fragment,
                                     torero::k_trajectory_geometry)),
      u_point_light_(shader_->uniform_location("u_point_light")),
      u_point_light_color_(shader_->uniform_location("u_point_light_color")),
      u_directional_light_(shader_->uniform_location("u_directional_light")),
      u_directional_light_color_(shader_->uniform_location("u_directional_light_color")),
      u_camera_position_(shader_->uniform_location("u_f_camera_position")),
      trajectories_(0),
      solid_(nullptr),
      dotted_(nullptr),
      dashed_(nullptr),
      arrowed_(nullptr),
      connection_draw_(core->syncronize(torero::Order::Trajectories,
                                        boost::bind(&trajectory::Manager::draw_all, this)))
    {
      initialize();
    }

    Manager::~Manager(){
      for(torero::trajectory::Element &trajectory : trajectories_)
        if(trajectory.trajectory != nullptr){
          if(trajectory.connection.connected())
            trajectory.connection.disconnect();
          delete trajectory.trajectory;
        }

      connection_draw_.disconnect();
      connection_update_.disconnect();

      if(solid_) delete solid_;
      if(dotted_) delete dotted_;
      if(dashed_) delete dashed_;
      if(arrowed_) delete arrowed_;

      if(shader_)
        delete shader_;
    }

    TMid Manager::add(const std::vector<torero::trajectory::Trajectory> *trajectories,
                      const std::string name,
                      const algebraica::mat4f *transformation_matrix,
                      const torero::trajectory::LineType type,
                      const bool visible){
      torero::trajectory::Element trajectory = { new trajectory::Trajectories(shader_, trajectories),
                                                 type, name, visible, boost::signals2::connection() };
      if(transformation_matrix != nullptr)
        trajectory.trajectory->set_transformation_matrix(transformation_matrix);

      trajectories_.push_back(trajectory);
      return trajectories_.size() - 1;
    }

    bool Manager::change_input(TMid id,
                               const std::vector<torero::trajectory::Trajectory> *trajectory){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->change_input(trajectory);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_type(TMid id, const torero::trajectory::LineType type){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].type = type;
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_visibility(TMid id, const bool visible){
      if(trajectories_.size() > id){
        trajectories_[id].visibility = visible;
        return true;
      }else
        return false;
    }

    bool Manager::set_transformation_matrix(TMid id,
                                            const algebraica::mat4f *transformation_matrix){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->set_transformation_matrix(transformation_matrix);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::translate(TMid id, const float x, const float y, const float z){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->translate(-y, z, -x);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate(TMid id, const float pitch, const float yaw, const float roll){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->rotate(-pitch, yaw, -roll);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_x(TMid id, const float angle){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->rotate_in_z(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_y(TMid id, const float angle){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->rotate_in_x(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_z(TMid id, const float angle){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          trajectories_[id].trajectory->rotate_in_y(angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::update(TMid id){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr && trajectories_[id].visibility){
          trajectories_[id].trajectory->update();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::update_all(){
      for(torero::trajectory::Element &trajectory : trajectories_)
        if(trajectory.trajectory != nullptr && trajectory.visibility)
          trajectory.trajectory->update();
    }

    bool Manager::draw(TMid id){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr && trajectories_[id].visibility){
          switch(trajectories_[id].type){
            case torero::trajectory::Dotted:
              if(dotted_) dotted_->bind();
            break;
            case torero::trajectory::Dashed:
              if(dashed_) dashed_->bind();
            break;
            case torero::trajectory::Arrowed:
              if(arrowed_) arrowed_->bind();
            break;
            default:
              if(solid_) solid_->bind();
            break;
          }
          trajectories_[id].trajectory->draw();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      for(torero::trajectory::Element &trajectory : trajectories_)
        if(trajectory.trajectory != nullptr && trajectory.visibility){
          switch(trajectory.type){
            case torero::trajectory::Dotted:
              if(dotted_) dotted_->bind();
            break;
            case torero::trajectory::Dashed:
              if(dashed_) dashed_->bind();
            break;
            case torero::trajectory::Arrowed:
              if(arrowed_) arrowed_->bind();
            break;
            default:
              if(solid_) solid_->bind();
            break;
          }
          trajectory.trajectory->draw();
        }
    }

    bool Manager::delete_trajectory(TMid id){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          if(trajectories_[id].connection.connected())
            trajectories_[id].connection.disconnect();
          delete trajectories_[id].trajectory;
          trajectories_[id].trajectory = nullptr;
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::purge(){
      for(torero::trajectory::Element &trajectory : trajectories_)
        if(trajectory.trajectory != nullptr){
          if(trajectory.connection.connected())
            trajectory.connection.disconnect();
          delete trajectory.trajectory;
        }
      trajectories_.clear();
    }

    bool Manager::connect(TMid id, boost::signals2::signal<void ()> *signal){
      if(trajectories_.size() > id)
        if(trajectories_[id].trajectory != nullptr){
          if(trajectories_[id].connection.connected())
            trajectories_[id].connection.disconnect();
          trajectories_[id].connection =
              signal->connect(boost::bind(&trajectory::Manager::update, this, id));
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::connect_all(boost::signals2::signal<void ()> *signal){
      if(connection_update_.connected())
        connection_update_.disconnect();
      connection_update_ = signal->connect(boost::bind(&trajectory::Manager::update_all, this));
    }

    void Manager::initialize(){
      if(!shader_->use())
        std::cout << shader_->error_log() << std::endl;

      // lights
      // ------
      algebraica::vec3f lightPositions[4] = {
        algebraica::vec3f(-10.0f, 10.0f,-10.0f),
        algebraica::vec3f( 10.0f, 10.0f,-10.0f),
        algebraica::vec3f(-10.0f, 10.0f, 10.0f),
        algebraica::vec3f( 10.0f, 10.0f, 10.0f),
      };
      algebraica::vec3f lightColors[4] = {
        algebraica::vec3f(1.0f, 1.0f, 0.0f),
        algebraica::vec3f(1.0f, 0.0f, 0.0f),
        algebraica::vec3f(0.0f, 1.0f, 0.0f),
        algebraica::vec3f(0.0f, 0.0f, 1.0f)
      };

      algebraica::vec3f sun_direction(-0.70711f, 0.70711f, 0.4f), sun_color(1.0f, 1.0f, 1.0f);

      shader_->set_value(u_directional_light_, sun_direction);
      shader_->set_value(u_directional_light_color_, sun_color);
      shader_->set_values(u_point_light_, &lightPositions[0], 4);
      shader_->set_values(u_point_light_color_, &lightColors[0], 4);

      stbi_set_flip_vertically_on_load(true);
      torero::image::File t_texture;
      // Solid texture
      t_texture.data = stbi_load(torero::k_trajectory_solid, &t_texture.width,
                                 &t_texture.height, &t_texture.components_size, 0);
      if(t_texture.data) solid_ = new torero::gl::Texture(t_texture, torero::ShaderTexture::Albedo);
      stbi_image_free(t_texture.data);
      // Dotted texture
      t_texture.data = stbi_load(torero::k_trajectory_dotted, &t_texture.width,
                                 &t_texture.height, &t_texture.components_size, 0);
      if(t_texture.data) dotted_ = new torero::gl::Texture(t_texture, torero::ShaderTexture::Albedo);
      stbi_image_free(t_texture.data);
      // Dashed texture
      t_texture.data = stbi_load(torero::k_trajectory_dashed, &t_texture.width,
                                 &t_texture.height, &t_texture.components_size, 0);
      if(t_texture.data) dashed_ = new torero::gl::Texture(t_texture, torero::ShaderTexture::Albedo);
      stbi_image_free(t_texture.data);
      // Arrowed texture
      t_texture.data = stbi_load(torero::k_trajectory_arrowed, &t_texture.width,
                                 &t_texture.height, &t_texture.components_size, 0);
      if(t_texture.data) arrowed_ = new torero::gl::Texture(t_texture, torero::ShaderTexture::Albedo);
      stbi_image_free(t_texture.data);

      shader_->set_value(shader_->uniform_location("u_diffuse"), torero::ShaderTexture::Albedo);
    }

    void Manager::updated_camera(){
      shader_->use();
      shader_->set_value(u_camera_position_, core_->camera().position());
    }
  }
}
