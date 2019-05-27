#include "torero/camera/view.h"
#include "torero/core.h"
#include "torero/ground/manager.h"
#include "torero/terminal/printer.h"
// Image loader
#include "stb_image.h"

namespace torero {
  namespace ground {
    Manager::Manager(Core *core) :
      core_(core),
      ground_shader_(new torero::gl::Shader(torero::k_ground_vertex, torero::k_ground_fragment,
                                            torero::k_ground_geometry)),
      u_point_light_ground_(ground_shader_->uniform_location("u_point_light")),
      u_point_light_color_ground_(ground_shader_->uniform_location("u_point_light_color")),
      u_directional_light_ground_(ground_shader_->uniform_location("u_directional_light")),
      u_directional_light_color_ground_(ground_shader_->uniform_location("u_directional_light_color")),
      u_camera_position_ground_(ground_shader_->uniform_location("u_f_camera_position")),
      grounds_(0),
      grid_(nullptr),
      line_shader_(new torero::gl::Shader(torero::k_lines_vertex, torero::k_lines_fragment)),
      grid_visibility_(true),
      connection_camera_(core->connect_camera_updates(boost::bind(&ground::Manager::updated_camera,
                                                                  this))),
      connection_draw_(core->syncronize(torero::Order::Ground,
                                        boost::bind(&ground::Manager::draw_all, this)))
    {
      initialize();
    }

    Manager::~Manager(){
      for(torero::ground::Element &ground : grounds_)
        if(ground.ground != nullptr){
          ground.connection.disconnect();
          delete ground.ground;
        }

      connection_camera_.disconnect();
      connection_update_.disconnect();
      connection_draw_.disconnect();

      if(ground_shader_)
        delete ground_shader_;

      if(grid_)
        delete grid_;

      if(line_shader_)
        delete line_shader_;
    }

    GMid Manager::add(const std::vector<torero::ground::Grid> *ground,
                      const std::string name,
                      const float width,
                      const float length,
                      const unsigned int number_of_elements_through_width,
                      const unsigned int number_of_elements_through_length,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible,
                      const bool calculate_height,
                      const float maximum_height){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
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

    GMid Manager::add(const std::vector<torero::ground::Ground2D> *ground,
                      const std::string name,
                      const float width,
                      const float length,
                      const unsigned int number_of_elements_through_width,
                      const unsigned int number_of_elements_through_length,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);
      groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                      number_of_elements_through_length);
      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    GMid Manager::add(const std::vector<torero::ground::Ground3D> *ground,
                      const std::string name,
                      const float width,
                      const float length,
                      const unsigned int number_of_elements_through_width,
                      const unsigned int number_of_elements_through_length,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);
      groundy.ground->set_ground_size(width, length, number_of_elements_through_width,
                                      number_of_elements_through_length);
      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    GMid Manager::add(const std::vector<torero::ground::Free2D> *ground,
                      const std::string name,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);

      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    GMid Manager::add(const std::vector<torero::ground::Free3D> *ground,
                      const std::string name,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);

      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    GMid Manager::add(const std::vector<torero::ground::FreePolar2D> *ground,
                      const std::string name,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);

      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    GMid Manager::add(const std::vector<torero::ground::FreePolar3D> *ground,
                      const std::string name,
                      const algebraica::mat4f *transformation_matrix,
                      const bool ground_visible){
      torero::ground::Element groundy = { new ground::Ground(ground_shader_),
                                          name, ground_visible, boost::signals2::connection() };
      groundy.ground->change_input(ground);

      if(transformation_matrix != nullptr)
        groundy.ground->set_transformation_matrix(transformation_matrix);

      grounds_.push_back(groundy);
      return grounds_.size() - 1;
    }

    bool Manager::change_input(GMid id, const std::vector<torero::ground::Ground2D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::change_input(GMid id, const std::vector<torero::ground::Ground3D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::change_input(GMid id, const std::vector<torero::ground::Free2D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::change_input(GMid id, const std::vector<torero::ground::Free3D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::change_input(GMid id,
                               const std::vector<torero::ground::FreePolar2D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::change_input(GMid id,
                               const std::vector<torero::ground::FreePolar3D> *ground){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->change_input(ground);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::fog_visibility(GMid id, const bool visible){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->fog_visibility(visible);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::grid_add(const float width, const float length,
                           const unsigned int line_quantity_through_width,
                           const unsigned int line_quantity_through_length,
                           const algebraica::mat4f *transformation_matrix){
      const bool ok{!grid_};

      if(ok){
        grid_ = new ground::LineGrid(line_shader_, width, length,
                                     line_quantity_through_width, line_quantity_through_length);
        if(transformation_matrix)
          grid_->transformation_matrix(transformation_matrix);
      }
      return ok;
    }

    bool Manager::grid_color(const float r, const float g, const float b, const float alpha){
      const bool ok{grid_};

      if(ok) grid_->color(r, g, b, alpha);
      return ok;
    }

    bool Manager::grid_fog(const bool visible){
      const bool ok{grid_};

      if(ok) grid_->fog(visible);
      return ok;
    }

    bool Manager::grid_properties(const float width, const float length,
                                  const unsigned int line_quantity_through_width,
                                  const unsigned int line_quantity_through_length){
      const bool ok{grid_};

      if(ok) grid_->properties(width, length, line_quantity_through_width,
                               line_quantity_through_length);
      return ok;
    }

    bool Manager::grid_rotate(const float pitch, const float yaw, const float roll){
      const bool ok{grid_};

      if(ok) grid_->rotate(-pitch, yaw, -roll);
      return ok;
    }

    bool Manager::grid_translate(const float x, const float y, const float z){
      const bool ok{grid_};

      if(ok) grid_->translate(-y, z, -x);
      return ok;
    }

    bool Manager::grid_transformation_matrix(const algebraica::mat4f *transformation_matrix){
      const bool ok{grid_};

      if(ok) grid_->transformation_matrix(transformation_matrix);
      return ok;
    }

    bool Manager::grid_visibility(const bool visible){
      const bool ok{grid_};

      if(ok) grid_visibility_ = visible;
      return ok;
    }

    bool Manager::ground_size(GMid id, const float width, const float length,
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

    bool Manager::set_visibility(GMid id, const bool visible){
      if(grounds_.size() > id){
        grounds_[id].visibility = visible;
        return true;
      }else
        return false;
    }

    bool Manager::set_transformation_matrix(GMid id,
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

    bool Manager::translate(GMid id, const float x, const float y, const float z){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->translate(-y, z, -x);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate(GMid id, const float pitch, const float yaw, const float roll){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->rotate(-pitch, yaw, -roll);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_x(GMid id, const float angle){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->rotate_in_z(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_y(GMid id, const float angle){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->rotate_in_x(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_z(GMid id, const float angle){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          grounds_[id].ground->rotate_in_y(angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::update(GMid id){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr && grounds_[id].visibility){
          grounds_[id].ground->update();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::update_all(){
      for(torero::ground::Element &ground : grounds_)
        if(ground.ground != nullptr && ground.visibility)
          ground.ground->update();
    }

    bool Manager::draw(GMid id){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr && grounds_[id].visibility){
          grounds_[id].ground->draw();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      for(torero::ground::Element &ground : grounds_)
        if(ground.ground != nullptr && ground.visibility)
          ground.ground->draw();
      if(grid_ && grid_visibility_)
        grid_->draw();
    }

    bool Manager::delete_ground(GMid id){
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

    void Manager::purge(){
      for(torero::ground::Element &ground : grounds_)
        if(ground.ground != nullptr){
          if(ground.connection.connected())
            ground.connection.disconnect();
          delete ground.ground;
        }
      grounds_.clear();
    }

    bool Manager::connect(GMid id, boost::signals2::signal<void ()> *signal){
      if(grounds_.size() > id)
        if(grounds_[id].ground != nullptr){
          if(grounds_[id].connection.connected())
            grounds_[id].connection.disconnect();
          grounds_[id].connection =
              signal->connect(boost::bind(&ground::Manager::update, this, id));
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::connect_all(boost::signals2::signal<void ()> *signal){
      if(connection_update_.connected())
        connection_update_.disconnect();
      connection_update_ = signal->connect(boost::bind(&ground::Manager::update_all, this));
    }

    void Manager::updated_camera(){
      ground_shader_->use();
      ground_shader_->set_value(u_camera_position_ground_, core_->camera().position());
    }

    void Manager::initialize(){
      if(!ground_shader_->use())
        torero::terminal::Printer::message(ground_shader_->error_log(), torero::Message::Error);

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
        torero::terminal::Printer::message(line_shader_->error_log(), torero::Message::Error);

      updated_camera();
    }
  }
}
