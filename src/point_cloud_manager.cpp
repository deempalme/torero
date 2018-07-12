#include "torero/point_cloud_manager.h"
#include "torero/core.h"

namespace torero {
  PointCloudManager::PointCloudManager(Core *core) :
    core_(core),
    shader_(new Shader("resources/shaders/point_cloud.vert",
                       "resources/shaders/point_cloud.frag")),
    u_pv_(shader_->uniform_location("u_pv")),
    point_clouds_(0),
    signal_updated_camera_(core->signal_updated_camera()->
                           connect(boost::bind(&PointCloudManager::updated_camera, this))),
    signal_draw_all_(core->syncronize(torero::Order::PointClouds)->
                     connect(boost::bind(&PointCloudManager::draw_all, this)))
  {
    shader_->set_value(u_pv_, core->camera_matrix_perspective_view());
  }

  PointCloudManager::~PointCloudManager(){
    for(torero::PointCloudElement &cloud : point_clouds_)
      if(cloud.point_cloud){
        if(cloud.connection.connected())
          cloud.connection.disconnect();
        delete cloud.point_cloud;
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


  PCMid PointCloudManager::add(const std::vector<torero::PointXYZ> *point_cloud,
                               const std::string name,
                               const algebraica::mat4f *transformation_matrix,
                               const float color_red, const float color_green,
                               const float color_blue, const bool visible,
                               const float point_size, const float maximum_intensity_value){
    torero::PointCloudElement cloud = { new PointCloud(shader_, point_size,
                                        maximum_intensity_value), name, visible,
                                        boost::signals2::connection()};

    cloud.point_cloud->set_transformation_matrix(transformation_matrix);
    cloud.point_cloud->set_cloud(point_cloud);
    cloud.point_cloud->set_color_palette(torero::ColorRGB{color_red, color_green, color_blue});
    cloud.point_cloud->set_color_mode(torero::ColorMode::Solid);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<torero::PointXYZI> *point_cloud,
                               const std::string name,
                               const algebraica::mat4f *transformation_matrix,
                               const torero::ColorMode color_mode,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    torero::PointCloudElement cloud = { new PointCloud(shader_, point_size,
                                            maximum_intensity_value), name, visible,
                                        boost::signals2::connection()};

    cloud.point_cloud->set_transformation_matrix(transformation_matrix);
    cloud.point_cloud->set_cloud(point_cloud);
    cloud.point_cloud->set_color_mode(color_mode);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<torero::PointXYZRGB> *point_cloud,
                               const std::string name,
                               const algebraica::mat4f *transformation_matrix,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    torero::PointCloudElement cloud = { new PointCloud(shader_, point_size,
                                            maximum_intensity_value), name, visible,
                                        boost::signals2::connection()};

    cloud.point_cloud->set_transformation_matrix(transformation_matrix);
    cloud.point_cloud->set_cloud(point_cloud);
    cloud.point_cloud->set_color_mode(torero::ColorMode::Data);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<torero::PointXYZRGBI> *point_cloud,
                               const std::string name,
                               const algebraica::mat4f *transformation_matrix,
                               const bool visible,
                               const float point_size,
                               const float maximum_intensity_value){
    torero::PointCloudElement cloud = { new PointCloud(shader_, point_size,
                                            maximum_intensity_value), name, visible,
                                        boost::signals2::connection()};

    cloud.point_cloud->set_transformation_matrix(transformation_matrix);
    cloud.point_cloud->set_cloud(point_cloud);
    cloud.point_cloud->set_color_mode(torero::ColorMode::Data);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  PCMid PointCloudManager::add(const std::vector<torero::PointXYZRGBA> *point_cloud,
                               const std::string name,
                               const algebraica::mat4f *transformation_matrix,
                               const bool visible, const float point_size,
                               const float maximum_intensity_value){
    torero::PointCloudElement cloud = { new PointCloud(shader_, point_size,
                                            maximum_intensity_value), name, visible,
                                        boost::signals2::connection()};

    cloud.point_cloud->set_transformation_matrix(transformation_matrix);
    cloud.point_cloud->set_cloud(point_cloud);
    cloud.point_cloud->set_color_mode(torero::ColorMode::Data);

    point_clouds_.push_back(cloud);
    return point_clouds_.size() - 1;
  }

  bool PointCloudManager::set_visibility(PCMid id, const bool visible){
    if(point_clouds_.size() > id){
      point_clouds_[id].visibility = visible;
      return true;
    }else
      return false;
  }

  bool PointCloudManager::set_colormap(PCMid id, const torero::ColorRGB color){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->set_color_palette(color);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::set_colormap(PCMid id, const std::vector<torero::ColorRGB> &colors){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->set_color_palette(colors);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::set_color_mode(PCMid id, const torero::ColorMode color_mode){
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
                                                    const algebraica::mat4f *transformation_matrix){
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
        point_clouds_[id].point_cloud->translate(-y, z, -x);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate(PCMid id, const float pitch, const float yaw, const float roll){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate(-pitch, yaw, -roll);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_x(PCMid id, const float angle){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_z(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_y(PCMid id, const float angle){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_x(-angle);
        return true;
      }else
        return false;
    else
      return false;
  }

  bool PointCloudManager::rotate_in_z(PCMid id, const float angle){
    if(point_clouds_.size() > id)
      if(point_clouds_[id].point_cloud != nullptr){
        point_clouds_[id].point_cloud->rotate_in_y(angle);
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
    for(torero::PointCloudElement &cloud : point_clouds_)
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
    for(torero::PointCloudElement &cloud : point_clouds_)
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
    for(torero::PointCloudElement &cloud : point_clouds_)
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

  void PointCloudManager::connect_all(boost::signals2::signal<void ()> *signal){
    if(signal_updated_all_.connected())
      signal_updated_all_.disconnect();
    signal_updated_all_ = signal->connect(boost::bind(&PointCloudManager::update_all, this));
  }

  void PointCloudManager::updated_camera(){
    shader_->use();
    shader_->set_value(u_pv_, core_->camera_matrix_perspective_view());
  }
}
