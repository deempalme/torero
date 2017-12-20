#include "include/trajectory_manager.h"
#include "include/core.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  TrajectoryManager::TrajectoryManager(Core *core) :
    core_(core),
    shader_(new Shader("resources/shaders/trajectory.vert",
                       "resources/shaders/trajectory.frag",
                       "resources/shaders/trajectory.geom")),
    u_pv_(shader_->uniform_location("u_pv")),
    u_point_light_(shader_->uniform_location("u_point_light")),
    u_point_light_color_(shader_->uniform_location("u_point_light_color")),
    u_directional_light_(shader_->uniform_location("u_directional_light")),
    u_directional_light_color_(shader_->uniform_location("u_directional_light_color")),
    u_camera_position_(shader_->uniform_location("u_camera_position")),
    trajectories_(0),
    solid_(nullptr),
    dotted_(nullptr),
    dashed_(nullptr),
    arrowed_(nullptr),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&TrajectoryManager::updated_camera, this))),
    signal_draw_all_(core->syncronize(Visualizer::TRAJECTORIES)->
                     connect(boost::bind(&TrajectoryManager::draw_all, this)))
  {
    initialize();
  }

  TrajectoryManager::~TrajectoryManager(){
    for(Visualizer::TrajectoryElement trajectory : trajectories_)
      if(trajectory.trajectory != nullptr){
        if(trajectory.connection.connected())
          trajectory.connection.disconnect();
        delete trajectory.trajectory;
      }

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();

    if(signal_draw_all_.connected())
      signal_draw_all_.disconnect();

    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();

    if(signal_draw_all_.connected())
      signal_draw_all_.disconnect();

    if(solid_) delete solid_;
    if(dotted_) delete dotted_;
    if(dashed_) delete dashed_;
    if(arrowed_) delete arrowed_;

    if(shader_)
      delete shader_;
  }

  TMid TrajectoryManager::add(const std::vector<Visualizer::Trajectory> *trajectories,
                              const std::string name,
                              const Algebraica::mat4f *transformation_matrix,
                              const Visualizer::LineType type,
                              const bool visible){
    Visualizer::TrajectoryElement trajectory = { new Trajectory(shader_, trajectories),
                                                 type, name, visible };
    if(transformation_matrix != nullptr)
      trajectory.trajectory->set_transformation_matrix(transformation_matrix);

    trajectories_.push_back(trajectory);
    return trajectories_.size() - 1;
  }

  bool TrajectoryManager::change_input(TMid id,
                                       const std::vector<Visualizer::Trajectory> *trajectory){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->change_input(trajectory);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::set_type(TMid id, const Visualizer::LineType type){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).type = type;
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::set_visibility(TMid id, const bool visible){
    if(trajectories_.size() > id){
      trajectories_.at(id).visibility = visible;
      return true;
    }else
      return false;
  }

  bool TrajectoryManager::set_transformation_matrix(TMid id,
                                                    const Algebraica::mat4f *transformation_matrix){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::translate(TMid id, const float x, const float y, const float z){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::rotate(TMid id, const float pitch, const float yaw, const float roll){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::rotate_in_x(TMid id, const float angle){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::rotate_in_y(TMid id, const float angle){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::rotate_in_z(TMid id, const float angle){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        trajectories_.at(id).trajectory->rotate_in_y(angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool TrajectoryManager::update(TMid id){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr && trajectories_.at(id).visibility){
        trajectories_.at(id).trajectory->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void TrajectoryManager::update_all(){
    for(Visualizer::TrajectoryElement trajectory : trajectories_)
      if(trajectory.trajectory != nullptr && trajectory.visibility)
        trajectory.trajectory->update();
  }

  bool TrajectoryManager::draw(TMid id){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr && trajectories_.at(id).visibility){
        switch(trajectories_.at(id).type){
        case Visualizer::DOTTED:
          if(dotted_) dotted_->use();
          break;
        case Visualizer::DASHED:
          if(dashed_) dashed_->use();
          break;
        case Visualizer::ARROWED:
          if(arrowed_) arrowed_->use();
          break;
        default:
          if(solid_) solid_->use();
          break;
        }
        trajectories_.at(id).trajectory->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void TrajectoryManager::draw_all(){
    for(Visualizer::TrajectoryElement trajectory : trajectories_)
      if(trajectory.trajectory != nullptr && trajectory.visibility){
        switch(trajectory.type){
        case Visualizer::DOTTED:
          if(dotted_) dotted_->use();
          break;
        case Visualizer::DASHED:
          if(dashed_) dashed_->use();
          break;
        case Visualizer::ARROWED:
          if(arrowed_) arrowed_->use();
          break;
        default:
          if(solid_) solid_->use();
          break;
        }
        trajectory.trajectory->draw();
      }
  }

  bool TrajectoryManager::delete_trajectory(TMid id){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        if(trajectories_.at(id).connection.connected())
          trajectories_.at(id).connection.disconnect();
        delete trajectories_.at(id).trajectory;
        trajectories_.at(id).trajectory = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void TrajectoryManager::purge(){
    for(Visualizer::TrajectoryElement trajectory : trajectories_)
      if(trajectory.trajectory != nullptr){
        if(trajectory.connection.connected())
          trajectory.connection.disconnect();
        delete trajectory.trajectory;
      }
    trajectories_.clear();
  }

  bool TrajectoryManager::connect(TMid id, boost::signals2::signal<void ()> *signal){
    if(trajectories_.size() > id)
      if(trajectories_.at(id).trajectory != nullptr){
        if(trajectories_.at(id).connection.connected())
          trajectories_.at(id).connection.disconnect();
        trajectories_.at(id).connection =
            signal->connect(boost::bind(&TrajectoryManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void TrajectoryManager::connect_all(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&TrajectoryManager::update_all, this));
  }

  void TrajectoryManager::updated_camera(){
    shader_->use();
    shader_->set_value(u_pv_, core_->camera_matrix_perspective_view());
  }

  void TrajectoryManager::initialize(){
    if(!shader_->use())
      std::cout << shader_->error_log() << std::endl;

    // lights
    // ------
    Algebraica::vec3f lightPositions[4] = {
      Algebraica::vec3f(-10.0f, 10.0f,-10.0f),
      Algebraica::vec3f( 10.0f, 10.0f,-10.0f),
      Algebraica::vec3f(-10.0f, 10.0f, 10.0f),
      Algebraica::vec3f( 10.0f, 10.0f, 10.0f),
    };
    Algebraica::vec3f lightColors[4] = {
      Algebraica::vec3f(1.0f, 1.0f, 0.0f),
      Algebraica::vec3f(1.0f, 0.0f, 0.0f),
      Algebraica::vec3f(0.0f, 1.0f, 0.0f),
      Algebraica::vec3f(0.0f, 0.0f, 1.0f)
    };

    Algebraica::vec3f sun_direction(-0.70711f, 0.70711f, 0.4f), sun_color(1.0f, 1.0f, 1.0f);

    shader_->set_value(u_directional_light_, sun_direction);
    shader_->set_value(u_directional_light_color_, sun_color);
    shader_->set_values(u_point_light_, &lightPositions[0], 4);
    shader_->set_values(u_point_light_color_, &lightColors[0], 4);

    stbi_set_flip_vertically_on_load(true);
    Visualizer::ImageFile t_texture;
    // Solid texture
    t_texture.data = stbi_load(std::string("resources/models3D/trajectory/solid.png").c_str(),
                               &t_texture.width, &t_texture.height, &t_texture.components_size, 0);
    if(t_texture.data) solid_ = new Texture(8, core_->max_anisotropic_filtering(), &t_texture);
    stbi_image_free(t_texture.data);
    // Dotted texture
    t_texture.data = stbi_load(std::string("resources/models3D/trajectory/dotted.png").c_str(),
                               &t_texture.width, &t_texture.height, &t_texture.components_size, 0);
    if(t_texture.data) dotted_ = new Texture(8, core_->max_anisotropic_filtering(), &t_texture);
    stbi_image_free(t_texture.data);
    // Dashed texture
    t_texture.data = stbi_load(std::string("resources/models3D/trajectory/dashed.png").c_str(),
                               &t_texture.width, &t_texture.height, &t_texture.components_size, 0);
    if(t_texture.data) dashed_ = new Texture(8, core_->max_anisotropic_filtering(), &t_texture);
    stbi_image_free(t_texture.data);
    // Arrowed texture
    t_texture.data = stbi_load(std::string("resources/models3D/trajectory/arrowed.png").c_str(),
                               &t_texture.width, &t_texture.height, &t_texture.components_size, 0);
    if(t_texture.data) arrowed_ = new Texture(8, core_->max_anisotropic_filtering(), &t_texture);
    stbi_image_free(t_texture.data);

    shader_->set_value(u_pv_, core_->camera_matrix_perspective_view());
    shader_->set_value(shader_->uniform_location("u_diffuse"), 8);
  }
}
