#include "include/ground_manager.h"
#include "include/core.h"
// Image loader
#include "stb_image.h"

namespace Toreo {
  GroundManager::GroundManager(Core *core) :
    core_(core),
    ground_shader_(new Shader("resources/shaders/ground.vert",
                              "resources/shaders/ground.frag",
                              "resources/shaders/ground.geom")),
    u_projection_ground_(ground_shader_->uniform_location("u_projection")),
    u_view_ground_(ground_shader_->uniform_location("u_view")),
    u_point_light_ground_(ground_shader_->uniform_location("u_point_light")),
    u_point_light_color_ground_(ground_shader_->uniform_location("u_point_light_color")),
    u_directional_light_ground_(ground_shader_->uniform_location("u_directional_light")),
    u_directional_light_color_ground_(ground_shader_->uniform_location("u_directional_light_color")),
    u_camera_position_ground_(ground_shader_->uniform_location("u_camera_position")),
    line_shader_(new Shader("resources/shaders/lines.vert",
                            "resources/shaders/lines.frag")),
    u_view_line_(line_shader_->uniform_location("u_view")),
    u_perspective_line_(line_shader_->uniform_location("u_projection")),
    grounds_(0),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&GroundManager::updated_camera, this))),
    signal_draw_all_(core->syncronize(Visualizer::GROUND)->
                     connect(boost::bind(&GroundManager::draw_all, this)))
  {
    initialize();
  }

  GroundManager::~GroundManager(){
    for(Visualizer::GroundElement ground : grounds_)
      if(ground.ground != nullptr){
        if(ground.connection.connected())
          ground.connection.disconnect();
        delete ground.ground;
      }

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();

    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();

    if(signal_draw_all_.connected())
      signal_draw_all_.disconnect();

    if(ground_shader_)
      delete ground_shader_;

    if(line_shader_)
      delete line_shader_;
  }

  GMid GroundManager::add(const std::vector<Visualizer::Ground2D> *ground,
                          const std::string name,
                          const float width,
                          const float length,
                          const unsigned int number_of_elements_through_width,
                          const unsigned int number_of_elements_through_length,
                          const unsigned int line_quantity_in_width,
                          const unsigned int line_quantity_in_length,
                          const Algebraica::mat4f *transformation_matrix,
                          const bool ground_visible,
                          const bool lines_visible){
    Visualizer::GroundElement groundy = { new Ground(ground_shader_, line_shader_, ground),
                                          name, ground_visible };
    groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                    number_of_elements_through_length);
    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    groundy.ground->set_lines(line_quantity_in_width, line_quantity_in_length);
    groundy.ground->line_visibility(lines_visible);
    groundy.ground->update();

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<Visualizer::Ground3D> *ground,
                          const std::string name,
                          const float width,
                          const float length,
                          const unsigned int number_of_elements_through_width,
                          const unsigned int number_of_elements_through_length,
                          const unsigned int line_quantity_in_width,
                          const unsigned int line_quantity_in_length,
                          const Algebraica::mat4f *transformation_matrix,
                          const bool ground_visible,
                          const bool lines_visible){
    Visualizer::GroundElement groundy = { new Ground(ground_shader_, line_shader_, ground),
                                          name, ground_visible };
    groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                    number_of_elements_through_length);
    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    groundy.ground->set_lines(line_quantity_in_width, line_quantity_in_length);
    groundy.ground->line_visibility(lines_visible);
    groundy.ground->update();

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  bool GroundManager::change_input(GMid id, const std::vector<Visualizer::Ground2D> *ground){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id, const std::vector<Visualizer::Ground3D> *ground){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::fog_visibility(GMid id, const bool visible){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->fog_visibility(visible);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::ground_size(GMid id, const float width, const float length,
                                  const unsigned int number_of_elements_through_width,
                                  const unsigned int number_of_elements_through_length){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->set_ground_size(width, length,
                                                number_of_elements_through_width,
                                                number_of_elements_through_length);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::lines_settings(GMid id, const unsigned int line_quantity_in_width,
                                     const unsigned int line_quantity_in_length){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->set_lines(line_quantity_in_width, line_quantity_in_length);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::line_color(GMid id, const float r, const float g,
                                 const float b, const float alpha){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->line_color(r, g, b, alpha);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::line_visibility(GMid id, const bool visible){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->line_visibility(visible);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::set_visibility(GMid id, const bool visible){
    if(grounds_.size() > id){
      grounds_.at(id).visibility = visible;
      return true;
    }else
      return false;
  }

  bool GroundManager::set_transformation_matrix(GMid id,
                                                const Algebraica::mat4f *transformation_matrix){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::translate(GMid id, const float x, const float y, const float z){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate(GMid id, const float pitch, const float yaw, const float roll){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_x(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_y(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_z(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        grounds_.at(id).ground->rotate_in_y(angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::update(GMid id){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr && grounds_.at(id).visibility){
        grounds_.at(id).ground->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::update_all(){
    for(Visualizer::GroundElement ground : grounds_)
      if(ground.ground != nullptr && ground.visibility)
        ground.ground->update();
  }

  bool GroundManager::draw(GMid id){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr && grounds_.at(id).visibility){
        grounds_.at(id).ground->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::draw_all(){
    for(Visualizer::GroundElement ground : grounds_)
      if(ground.ground != nullptr && ground.visibility)
        ground.ground->draw();
  }

  bool GroundManager::delete_ground(GMid id){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        if(grounds_.at(id).connection.connected())
          grounds_.at(id).connection.disconnect();
        delete grounds_.at(id).ground;
        grounds_.at(id).ground = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::purge(){
    for(Visualizer::GroundElement ground : grounds_)
      if(ground.ground != nullptr){
        if(ground.connection.connected())
          ground.connection.disconnect();
        delete ground.ground;
      }
    grounds_.clear();
  }

  bool GroundManager::connect(GMid id, boost::signals2::signal<void ()> *signal){
    if(grounds_.size() > id)
      if(grounds_.at(id).ground != nullptr){
        if(grounds_.at(id).connection.connected())
          grounds_.at(id).connection.disconnect();
        grounds_.at(id).connection =
            signal->connect(boost::bind(&GroundManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::connect_update(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&GroundManager::update_all, this));
  }

  void GroundManager::updated_camera(){
    ground_shader_->use();
    ground_shader_->set_value(u_view_ground_, core_->camera_matrix_view());
    ground_shader_->set_value(u_projection_ground_, core_->camera_matrix_perspective());
    ground_shader_->set_value(u_camera_position_ground_, core_->camera_position());
    line_shader_->use();
    line_shader_->set_value(u_view_line_, core_->camera_matrix_view());
    line_shader_->set_value(u_perspective_line_, core_->camera_matrix_perspective());
  }

  void GroundManager::initialize(){
    if(!ground_shader_->use())
      std::cout << ground_shader_->error_log() << std::endl;

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

    ground_shader_->set_value(u_directional_light_ground_, sun_direction);
    ground_shader_->set_value(u_directional_light_color_ground_, sun_color);
    ground_shader_->set_values(u_point_light_ground_, &lightPositions[0], 4);
    ground_shader_->set_values(u_point_light_color_ground_, &lightColors[0], 4);

    if(!line_shader_->use())
      std::cout << line_shader_->error_log() << std::endl;

    updated_camera();
  }
}
