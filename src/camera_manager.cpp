#include "torero/camera_manager.h"
#include "torero/core.h"

namespace Toreo {
  CameraManager::CameraManager(Core *core) :
    core_(core),
    shader_(new Shader("resources/shaders/camera.vert",
                       "resources/shaders/camera.frag")),
    u_pv_(shader_->uniform_location("u_pv")),
    cameras_(0),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&CameraManager::update_camera, this))),
    signal_draw_all_(core->syncronize(Visualizer::CAMERA)->
                     connect(boost::bind(&CameraManager::draw_all, this)))
  {
    shader_->set_value(u_pv_, core->camera_matrix_perspective_view());
  }

  CameraManager::~CameraManager(){
    for(Visualizer::CameraElement &camera : cameras_)
      if(camera.camera){
        if(camera.connection.connected())
          camera.connection.disconnect();
        delete camera.camera;
      }

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();

    if(signal_draw_all_.connected())
      signal_draw_all_.disconnect();

    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();

    if(shader_)
      delete shader_;
  }

  CMid CameraManager::add(const Visualizer::ImageFile *video_input,
                          const std::string name,
                          const Visualizer::Dimensionality number_of_dimensions,
                          const Algebraica::mat4f *transformation_matrix, const bool visible){
    Visualizer::CameraElement camera = { new CameraElement(shader_), name, visible };

    camera.camera->set_camera(video_input);
    camera.camera->set_dimensions(number_of_dimensions);
    camera.camera->set_transformation_matrix(transformation_matrix);

    cameras_.push_back(camera);
    return cameras_.size() - 1;
  }

  bool CameraManager::set_visibility(const CMid id, const bool visible){
    if(cameras_.size() > id){
      cameras_[id].visibility = visible;
      return true;
    }else
      return false;
  }

  bool CameraManager::set_camera_curvature(const CMid id, const float curvature){
    if(cameras_.size() > id){
      cameras_[id].camera->set_curvature(curvature);
      return true;
    }else
      return false;
  }

  bool CameraManager::set_transformation_matrix(const CMid id,
                                                const Algebraica::mat4f *transformation_matrix){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::translate(const CMid id, const float x, const float y, const float z){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::resize(const CMid id, const float width, const float height){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->resize(width, height);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::rotate(const CMid id, const float pitch, const float yaw, const float roll){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::rotate_in_x(const CMid id, const float angle){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::rotate_in_y(const CMid id, const float angle){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::rotate_in_z(const CMid id, const float angle){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        cameras_[id].camera->rotate_in_y(angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool CameraManager::update(const CMid id){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr && cameras_[id].visibility){
        cameras_[id].camera->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void CameraManager::update_all(){
    for(Visualizer::CameraElement &camera : cameras_)
      if(camera.camera != nullptr && camera.visibility)
        camera.camera->update();
  }

  bool CameraManager::draw(const CMid id){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr && cameras_[id].visibility){
        cameras_[id].camera->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void CameraManager::draw_all(){
    for(Visualizer::CameraElement &camera : cameras_)
      if(camera.camera != nullptr && camera.visibility)
        camera.camera->draw();
  }

  bool CameraManager::delete_camera(const CMid id){
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

  void CameraManager::purge(){
    for(Visualizer::CameraElement &camera : cameras_)
      if(camera.camera != nullptr){
        if(camera.connection.connected())
          camera.connection.disconnect();
        delete camera.camera;
      }
    cameras_.clear();
  }

  bool CameraManager::connect(const CMid id, boost::signals2::signal<void ()> *signal){
    if(cameras_.size() > id)
      if(cameras_[id].camera != nullptr){
        if(cameras_[id].connection.connected())
          cameras_[id].connection.disconnect();
        cameras_[id].connection =
            signal->connect(boost::bind(&CameraManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void CameraManager::connect_all(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&CameraManager::update_all, this));
  }

  void CameraManager::update_camera(){
    shader_->use();
    shader_->set_value(u_pv_, core_->camera_matrix_perspective_view());
  }

  bool CameraManager::load_3d_model(){

  }
}
