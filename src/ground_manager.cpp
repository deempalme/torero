#include "torero/ground_manager.h"
#include "torero/core.h"
// Image loader
#include "stb_image.h"

namespace torero {
  GroundManager::GroundManager(Core *core) :
    core_(core),
    ground_shader_(new Shader("resources/shaders/ground.vert",
                              "resources/shaders/ground.frag",
                              "resources/shaders/ground.geom")),
    u_projection_ground_(ground_shader_->uniform_location("u_projection")),
    u_view_ground_(ground_shader_->uniform_location("u_view")),
    u_pv_ground_(ground_shader_->uniform_location("u_pv")),
    u_point_light_ground_(ground_shader_->uniform_location("u_point_light")),
    u_point_light_color_ground_(ground_shader_->uniform_location("u_point_light_color")),
    u_directional_light_ground_(ground_shader_->uniform_location("u_directional_light")),
    u_directional_light_color_ground_(ground_shader_->uniform_location("u_directional_light_color")),
    u_camera_position_ground_(ground_shader_->uniform_location("u_camera_position")),
    grounds_(0),
    grid_(nullptr),
    line_shader_(new Shader("resources/shaders/lines.vert",
                            "resources/shaders/lines.frag")),
    u_view_line_(line_shader_->uniform_location("u_view")),
    u_perspective_line_(line_shader_->uniform_location("u_projection")),
    grid_visibility_(true),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&GroundManager::updated_camera, this))),
    signal_draw_all_(core->syncronize(torero::Order::Ground)->
                     connect(boost::bind(&GroundManager::draw_all, this)))
  {
    initialize();
  }

  GroundManager::~GroundManager(){
    for(torero::GroundElement &ground : grounds_)
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

    if(grid_)
      delete grid_;

    if(line_shader_)
      delete line_shader_;
  }

  GMid GroundManager::add(const std::vector<torero::GroundGrid> *ground,
                          const std::string name,
                          const float width,
                          const float length,
                          const unsigned int number_of_elements_through_width,
                          const unsigned int number_of_elements_through_length,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible,
                          const bool calculate_height,
                          const float maximum_height){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);
    groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                    number_of_elements_through_length);
    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    if(calculate_height)
      groundy.ground->calculate_height(true, maximum_height);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::Ground2D> *ground,
                          const std::string name,
                          const float width,
                          const float length,
                          const unsigned int number_of_elements_through_width,
                          const unsigned int number_of_elements_through_length,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);
    groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                    number_of_elements_through_length);
    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::Ground3D> *ground,
                          const std::string name,
                          const float width,
                          const float length,
                          const unsigned int number_of_elements_through_width,
                          const unsigned int number_of_elements_through_length,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);
    groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                    number_of_elements_through_length);
    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::FreeGround2D> *ground,
                          const std::string name,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);

    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::FreeGround3D> *ground,
                          const std::string name,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);

    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::FreePolarGround2D> *ground,
                          const std::string name,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);

    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  GMid GroundManager::add(const std::vector<torero::FreePolarGround3D> *ground,
                          const std::string name,
                          const algebraica::mat4f *transformation_matrix,
                          const bool ground_visible){
    torero::GroundElement groundy = { new Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
    groundy.ground->change_input(ground);

    if(transformation_matrix != nullptr)
      groundy.ground->set_transformation_matrix(transformation_matrix);

    grounds_.push_back(groundy);
    return grounds_.size() - 1;
  }

  bool GroundManager::change_input(GMid id, const std::vector<torero::Ground2D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id, const std::vector<torero::Ground3D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id, const std::vector<torero::FreeGround2D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id, const std::vector<torero::FreeGround3D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id,
                                   const std::vector<torero::FreePolarGround2D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::change_input(GMid id,
                                   const std::vector<torero::FreePolarGround3D> *ground){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->change_input(ground);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::fog_visibility(GMid id, const bool visible){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->fog_visibility(visible);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::grid_add(const float width, const float length,
                               const unsigned int line_quantity_through_width,
                               const unsigned int line_quantity_through_length,
                               const algebraica::mat4f *transformation_matrix){
    const bool ok{!grid_};

    if(ok){
      grid_ = new LineGrid(line_shader_, width, length,
                           line_quantity_through_width, line_quantity_through_length);
      if(transformation_matrix)
        grid_->transformation_matrix(transformation_matrix);
    }
    return ok;
  }

  bool GroundManager::grid_color(const float r, const float g, const float b, const float alpha){
    const bool ok{grid_};

    if(ok) grid_->color(r, g, b, alpha);
    return ok;
  }

  bool GroundManager::grid_fog(const bool visible){
    const bool ok{grid_};

    if(ok) grid_->fog(visible);
    return ok;
  }

  bool GroundManager::grid_properties(const float width, const float length,
                                      const unsigned int line_quantity_through_width,
                                      const unsigned int line_quantity_through_length){
    const bool ok{grid_};

    if(ok) grid_->properties(width, length, line_quantity_through_width,
                             line_quantity_through_length);
    return ok;
  }

  bool GroundManager::grid_rotate(const float pitch, const float yaw, const float roll){
    const bool ok{grid_};

    if(ok) grid_->rotate(-pitch, yaw, -roll);
    return ok;
  }

  bool GroundManager::grid_translate(const float x, const float y, const float z){
    const bool ok{grid_};

    if(ok) grid_->translate(-y, z, -x);
    return ok;
  }

  bool GroundManager::grid_transformation_matrix(const algebraica::mat4f *transformation_matrix){
    const bool ok{grid_};

    if(ok) grid_->transformation_matrix(transformation_matrix);
    return ok;
  }

  bool GroundManager::grid_visibility(const bool visible){
    const bool ok{grid_};

    if(ok) grid_visibility_ = visible;
    return ok;
  }

  bool GroundManager::ground_size(GMid id, const float width, const float length,
                                  const unsigned int number_of_elements_through_width,
                                  const unsigned int number_of_elements_through_length){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->set_ground_size(width, length,
                                                number_of_elements_through_width,
                                                number_of_elements_through_length);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::set_visibility(GMid id, const bool visible){
    if(grounds_.size() > id){
      grounds_[id].visibility = visible;
      return true;
    }else
      return false;
  }

  bool GroundManager::set_transformation_matrix(GMid id,
                                                const algebraica::mat4f *transformation_matrix){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::translate(GMid id, const float x, const float y, const float z){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate(GMid id, const float pitch, const float yaw, const float roll){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_x(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_y(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::rotate_in_z(GMid id, const float angle){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        grounds_[id].ground->rotate_in_y(angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool GroundManager::update(GMid id){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr && grounds_[id].visibility){
        grounds_[id].ground->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::update_all(){
    std::cout << "updating" << std::endl;
    for(torero::GroundElement &ground : grounds_)
      if(ground.ground != nullptr && ground.visibility)
        ground.ground->update();
  }

  bool GroundManager::draw(GMid id){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr && grounds_[id].visibility){
        grounds_[id].ground->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::draw_all(){
    for(torero::GroundElement &ground : grounds_)
      if(ground.ground != nullptr && ground.visibility)
        ground.ground->draw();
    if(grid_ && grid_visibility_)
      grid_->draw();
  }

  bool GroundManager::delete_ground(GMid id){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        if(grounds_[id].connection.connected())
          grounds_[id].connection.disconnect();
        delete grounds_[id].ground;
        grounds_[id].ground = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::purge(){
    for(torero::GroundElement &ground : grounds_)
      if(ground.ground != nullptr){
        if(ground.connection.connected())
          ground.connection.disconnect();
        delete ground.ground;
      }
    grounds_.clear();
  }

  bool GroundManager::connect(GMid id, boost::signals2::signal<void ()> *signal){
    if(grounds_.size() > id)
      if(grounds_[id].ground != nullptr){
        if(grounds_[id].connection.connected())
          grounds_[id].connection.disconnect();
        grounds_[id].connection =
            signal->connect(boost::bind(&GroundManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void GroundManager::connect_all(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&GroundManager::update_all, this));
  }

  void GroundManager::updated_camera(){
    ground_shader_->use();
    ground_shader_->set_value(u_view_ground_, core_->camera_matrix_view());
    ground_shader_->set_value(u_projection_ground_, core_->camera_matrix_perspective());
    ground_shader_->set_value(u_pv_ground_, core_->camera_matrix_perspective_view());
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

    ground_shader_->set_value(u_directional_light_ground_, sun_direction);
    ground_shader_->set_value(u_directional_light_color_ground_, sun_color);
    ground_shader_->set_values(u_point_light_ground_, &lightPositions[0], 4);
    ground_shader_->set_values(u_point_light_color_ground_, &lightColors[0], 4);

    if(!line_shader_->use())
      std::cout << line_shader_->error_log() << std::endl;

    updated_camera();
  }
}
