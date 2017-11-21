#include "includes/point_cloud_manager.h"
#include "includes/core.h"

namespace Toreo {
  PointCloudManager::PointCloudManager(Core *core) :
    core_(core),
    point_cloud_shader_(new Shader("resources/shaders/point_cloud.vert",
                                   "resources/shaders/point_cloud.frag")),
    pc_u_pv_(point_cloud_shader_->uniform_location("u_pv")),
    point_clouds_(0),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&PointCloudManager::updated_camera, this))),
    signal_updated_screen_(core->signal_updated_screen()->
                           connect(boost::bind(&PointCloudManager::draw_all, this)))
  {
    point_cloud_shader_->set_value(pc_u_pv_, core->camera_matrix_perspective_view());
  }

  PointCloudManager::~PointCloudManager(){
    for(Visualizer::PointCloudElement cloud : point_clouds_)
      if(cloud.point_cloud != nullptr){
        if(cloud.connection.connected())
          cloud.connection.disconnect();
        delete cloud.point_cloud;
      }

    if(signal_updated_camera_.connected())
      signal_updated_camera_.disconnect();

    if(signal_updated_screen_.connected())
      signal_updated_screen_.disconnect();

    if(point_cloud_shader_)
      delete point_cloud_shader_;
  }


  PCMid PointCloudManager::add(const std::vector<Visualizer::pointXYZ> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix,
                               const float color_red, const float color_green,
                               const float color_blue, const bool visible,
                               const float point_size, const float maximum_intensity_value){
    Visualizer::PointCloudElement cloud = { new PointCloud(point_cloud_shader_, point_cloud,
                                            Algebraica::vec3f(color_red, color_green, color_blue),
                                            point_size, maximum_intensity_value), name, visible };
    if(transformation_matrix != nullptr)
      cloud.point_cloud->set_transformation_matrix(transformation_matrix);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<Visualizer::pointXYZI> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix,
                               const Visualizer::ColorMode color_mode,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    Visualizer::PointCloudElement cloud = { new PointCloud(point_cloud_shader_, point_cloud,
                                            color_mode, point_size, maximum_intensity_value),
                                            name, visible };
    if(transformation_matrix != nullptr)
      cloud.point_cloud->set_transformation_matrix(transformation_matrix);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<Visualizer::pointXYZRGB> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    Visualizer::PointCloudElement cloud = { new PointCloud(point_cloud_shader_, point_cloud,
                                            point_size, maximum_intensity_value), name, visible };
    if(transformation_matrix != nullptr)
      cloud.point_cloud->set_transformation_matrix(transformation_matrix);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<Visualizer::pointXYZRGBA> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    Visualizer::PointCloudElement cloud = { new PointCloud(point_cloud_shader_, point_cloud,
                                            point_size, maximum_intensity_value), name, visible };
    if(transformation_matrix != nullptr)
      cloud.point_cloud->set_transformation_matrix(transformation_matrix);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  bool PointCloudManager::change_input(PCMid id,
                                       const std::vector<Visualizer::pointXYZ> *point_cloud){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->change_input(point_cloud);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::change_input(PCMid id,
                                       const std::vector<Visualizer::pointXYZI> *point_cloud){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->change_input(point_cloud);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::change_input(PCMid id,
                                       const std::vector<Visualizer::pointXYZRGB> *point_cloud){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->change_input(point_cloud);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::change_input(PCMid id,
                                       const std::vector<Visualizer::pointXYZRGBA> *point_cloud){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->change_input(point_cloud);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::set_visibility(PCMid id, const bool visible){
    if(point_clouds_.size() > id){
      point_clouds_[id].visibility = visible;
      return true;
    }else
      return false;
  }

  bool PointCloudManager::set_colormap(PCMid id, const Algebraica::vec3f *colors,
                                       const unsigned int quantity){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->set_colormap(colors, quantity);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::set_color_mode(PCMid id, const Visualizer::ColorMode color_mode){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->set_color_mode(color_mode);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::set_transformation_matrix(PCMid id,
                                                    const Algebraica::mat4f *transformation_matrix){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->set_transformation_matrix(transformation_matrix);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::translate(PCMid id, const float x, const float y, const float z){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->translate(x, y, z);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate(PCMid id, const float pitch, const float yaw, const float roll){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate(pitch, yaw, roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_x(PCMid id, const float x){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_x(x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_y(PCMid id, const float y){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_y(y);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_z(PCMid id, const float z){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_z(z);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::update(PCMid id){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr && point_clouds_[id].visibility){
        point_clouds_[id].point_cloud->update();
        return true;
      }else
        return false;
    else
      return false;
  }

  void PointCloudManager::update_all(){
    for(Visualizer::PointCloudElement cloud : point_clouds_)
      if(cloud.point_cloud != nullptr && cloud.visibility)
        cloud.point_cloud->update();
  }

  bool PointCloudManager::draw(PCMid id){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr && point_clouds_[id].visibility){
        point_clouds_[id].point_cloud->draw();
        return true;
      }else
        return false;
    else
      return false;
  }

  void PointCloudManager::draw_all(){
    for(Visualizer::PointCloudElement cloud : point_clouds_)
      if(cloud.point_cloud != nullptr && cloud.visibility)
        cloud.point_cloud->draw();
  }

  bool PointCloudManager::delete_cloud(PCMid id){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        if(point_clouds_[id].connection.connected())
          point_clouds_[id].connection.disconnect();
        delete point_clouds_[id].point_cloud;
        point_clouds_[id].point_cloud = nullptr;
        return true;
      }else
        return false;
    else
      return false;
  }

  void PointCloudManager::purge(){
    for(Visualizer::PointCloudElement cloud : point_clouds_)
      if(cloud.point_cloud != nullptr){
        if(cloud.connection.connected())
          cloud.connection.disconnect();
        delete cloud.point_cloud;
      }
    point_clouds_.clear();
  }

  bool PointCloudManager::connect(PCMid id, boost::signals2::signal<void ()> *signal){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        if(point_clouds_[id].connection.connected())
          point_clouds_[id].connection.disconnect();
        point_clouds_[id].connection =
            signal->connect(boost::bind(&PointCloudManager::update, this, id));
        return true;
      }else
        return false;
    else
      return false;
  }

  void PointCloudManager::updated_camera(){
    point_cloud_shader_->use();
    point_cloud_shader_->set_value(pc_u_pv_, core_->camera_matrix_perspective_view());
  }
}
