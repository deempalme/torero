#include "torero/camera/manager.h"
#include "torero/core.h"

namespace torero {
  namespace camera {
    Manager::Manager(torero::Core *core) :
      core_(core),
      shader_(new torero::gl::Shader(torero::k_camera_vertex, torero::k_camera_fragment)),
      cameras_(0),
      connection_draw_(core->syncronize(torero::Order::Camera,
                                        boost::bind(&camera::Manager::draw_all, this)))
    {}

    Manager::~Manager(){
      for(torero::camera::Component &camera : cameras_)
        if(camera.camera){
          if(camera.connection.connected())
            camera.connection.disconnect();
          delete camera.camera;
        }

      if(connection_draw_.connected())
        connection_draw_.disconnect();

      if(connection_update_.connected())
        connection_update_.disconnect();

      if(shader_)
        delete shader_;
    }

    CMid Manager::add(const torero::image::File *video_input,
                      const std::string name,
                      const torero::camera::Dimensionality number_of_dimensions,
                      const algebraica::mat4f *transformation_matrix, const bool visible){
      torero::camera::Component camera = { new camera::Camera(shader_), name, visible,
                                           boost::signals2::connection() };

      camera.camera->set_camera(video_input);
      camera.camera->set_dimensions(number_of_dimensions);
      camera.camera->set_transformation_matrix(transformation_matrix);

      cameras_.push_back(camera);
      return cameras_.size() - 1;
    }

    bool Manager::set_visibility(const CMid id, const bool visible){
      if(cameras_.size() > id){
        cameras_[id].visibility = visible;
        return true;
      }else
        return false;
    }

    bool Manager::set_camera_curvature(const CMid id, const float curvature){
      if(cameras_.size() > id){
        cameras_[id].camera->set_curvature(curvature);
        return true;
      }else
        return false;
    }

    bool Manager::set_transformation_matrix(const CMid id,
                                            const algebraica::mat4f *transformation_matrix){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->set_transformation_matrix(transformation_matrix);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::translate(const CMid id, const float x, const float y, const float z){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->translate(-y, z, -x);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::resize(const CMid id, const float width, const float height){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->resize(width, height);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate(const CMid id, const float pitch, const float yaw, const float roll){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->rotate(-pitch, yaw, -roll);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_x(const CMid id, const float angle){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->rotate_in_z(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_y(const CMid id, const float angle){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->rotate_in_x(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_z(const CMid id, const float angle){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          cameras_[id].camera->rotate_in_y(angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::update(const CMid id){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr && cameras_[id].visibility){
          cameras_[id].camera->update();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::update_all(){
      for(torero::camera::Component &camera : cameras_)
        if(camera.camera != nullptr && camera.visibility)
          camera.camera->update();
    }

    bool Manager::draw(const CMid id){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr && cameras_[id].visibility){
          cameras_[id].camera->draw();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      for(torero::camera::Component &camera : cameras_)
        if(camera.camera != nullptr && camera.visibility)
          camera.camera->draw();
    }

    bool Manager::delete_camera(const CMid id){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          if(cameras_[id].connection.connected())
            cameras_[id].connection.disconnect();
          delete cameras_[id].camera;
          cameras_[id].camera = nullptr;
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::purge(){
      for(torero::camera::Component &camera : cameras_)
        if(camera.camera != nullptr){
          if(camera.connection.connected())
            camera.connection.disconnect();
          delete camera.camera;
        }
      cameras_.clear();
    }

    bool Manager::connect(const CMid id, boost::signals2::signal<void ()> *signal){
      if(cameras_.size() > id)
        if(cameras_[id].camera != nullptr){
          if(cameras_[id].connection.connected())
            cameras_[id].connection.disconnect();
          cameras_[id].connection =
              signal->connect(boost::bind(&camera::Manager::update, this, id));
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::connect_all(boost::signals2::signal<void ()> *signal){
      if(connection_update_.connected())
        connection_update_.disconnect();
      connection_update_ = signal->connect(boost::bind(&camera::Manager::update_all, this));
    }
  }
}
