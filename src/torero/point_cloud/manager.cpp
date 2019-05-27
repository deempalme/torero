#include "torero/camera/view.h"
#include "torero/core.h"
#include "torero/point_cloud/manager.h"
#include "torero/terminal/printer.h"

namespace torero {
  namespace point_cloud {
    Manager::Manager(torero::Core *core) :
      core_(core),
      shader_(new torero::gl::Shader(torero::k_point_cloud_vertex, torero::k_point_cloud_fragment)),
      point_clouds_(0),
      connection_draw_(core->syncronize(torero::Order::PointClouds,
                                        boost::bind(&point_cloud::Manager::draw_all, this)))
    {
      if(shader_->error())
        torero::terminal::Printer::error(shader_->error_log());
    }

    Manager::~Manager(){
      for(torero::point_cloud::Element &cloud : point_clouds_)
        if(cloud.point_cloud){
          if(cloud.connection.connected())
            cloud.connection.disconnect();
          delete cloud.point_cloud;
        }

      connection_draw_.disconnect();
      connection_update_.disconnect();

      if(shader_)
        delete shader_;
    }

    PCMid Manager::add(const std::vector<torero::PointXYZ> *point_cloud,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const float color_red, const float color_green,
                       const float color_blue, const bool visible,
                       const float point_size, const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection()};

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud(point_cloud);
      cloud.point_cloud->set_color_palette(torero::ColorRGB{color_red, color_green, color_blue});
      cloud.point_cloud->set_color_mode(torero::point_cloud::ColorMode::Solid);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    PCMid Manager::add(const std::vector<torero::PointXYZI> *point_cloud,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const torero::point_cloud::ColorMode color_mode,
                       const bool visible, const float point_size,
                       const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection()};

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud(point_cloud);
      cloud.point_cloud->set_color_mode(color_mode);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    PCMid Manager::add(const std::vector<torero::PointXYZRGB> *point_cloud,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const bool visible, const float point_size,
                       const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection()};

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud(point_cloud);
      cloud.point_cloud->set_color_mode(torero::point_cloud::ColorMode::Data);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    PCMid Manager::add(const std::vector<torero::PointXYZRGBI> *point_cloud,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const bool visible,
                       const float point_size,
                       const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection()};

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud(point_cloud);
      cloud.point_cloud->set_color_mode(torero::point_cloud::ColorMode::Data);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    PCMid Manager::add(const std::vector<torero::PointXYZRGBA> *point_cloud,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const bool visible, const float point_size,
                       const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection()};

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud(point_cloud);
      cloud.point_cloud->set_color_mode(torero::point_cloud::ColorMode::Data);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    PCMid Manager::add(const GLuint vertex_array_object_id,
                       const GLsizei *buffer_size,
                       const std::string &name,
                       const torero::Pose3D *coordinate_frame,
                       const torero::point_cloud::ColorMode color_mode,
                       const bool visible, const float point_size,
                       const float maximum_intensity_value){
      torero::point_cloud::Element cloud = { new torero::point_cloud::Cloud(shader_, point_size,
                                             maximum_intensity_value),
                                             name, visible, boost::signals2::connection() };

      cloud.point_cloud->set_coordinate_frame(coordinate_frame);
      cloud.point_cloud->set_cloud_buffer(vertex_array_object_id, buffer_size);
      cloud.point_cloud->set_color_mode(color_mode);

      point_clouds_.push_back(cloud);
      return point_clouds_.size() - 1;
    }

    bool Manager::set_visibility(PCMid id, const bool visible){
      if(point_clouds_.size() > id){
        point_clouds_[id].visibility = visible;
        return true;
      }else
        return false;
    }

    bool Manager::set_colormap(PCMid id, const torero::ColorRGB color){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->set_color_palette(color);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_colormap(PCMid id, const std::vector<torero::ColorRGB> &colors){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->set_color_palette(colors);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_color_mode(PCMid id, const torero::point_cloud::ColorMode color_mode){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->set_color_mode(color_mode);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_coordiante_frame(PCMid id,
                                       const torero::Pose3D *coordinate_frame){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->set_coordinate_frame(coordinate_frame);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_pose(PCMid id, torero::Pose3D *pose){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->set_pose(pose);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::translate(PCMid id, const float x, const float y, const float z){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->translate(x, y, z);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate(PCMid id, const float pitch, const float yaw, const float roll){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->rotate(-pitch, yaw, -roll);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_x(PCMid id, const float angle){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->rotate_in_z(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_y(PCMid id, const float angle){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->rotate_in_x(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_z(PCMid id, const float angle){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          point_clouds_[id].point_cloud->rotate_in_y(angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::update(PCMid id){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr && point_clouds_[id].visibility){
          point_clouds_[id].point_cloud->update();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::update_all(){
      for(torero::point_cloud::Element &cloud : point_clouds_)
        if(cloud.point_cloud != nullptr && cloud.visibility)
          cloud.point_cloud->update();
    }

    bool Manager::draw(PCMid id){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr && point_clouds_[id].visibility){
          point_clouds_[id].point_cloud->draw();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      for(torero::point_cloud::Element &cloud : point_clouds_)
        if(cloud.point_cloud != nullptr && cloud.visibility)
          cloud.point_cloud->draw();
    }

    bool Manager::delete_cloud(PCMid id){
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

    void Manager::purge(){
      for(torero::point_cloud::Element &cloud : point_clouds_)
        if(cloud.point_cloud != nullptr){
          if(cloud.connection.connected())
            cloud.connection.disconnect();
          delete cloud.point_cloud;
        }
      point_clouds_.clear();
    }

    bool Manager::connect(PCMid id, boost::signals2::signal<void ()> *signal){
      if(point_clouds_.size() > id)
        if(point_clouds_[id].point_cloud != nullptr){
          if(point_clouds_[id].connection.connected())
            point_clouds_[id].connection.disconnect();
          point_clouds_[id].connection =
              signal->connect(boost::bind(&point_cloud::Manager::update, this, id));
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::connect_all(boost::signals2::signal<void ()> *signal){
      if(connection_update_.connected())
        connection_update_.disconnect();
      connection_update_ = signal->connect(boost::bind(&point_cloud::Manager::update_all, this));
    }
  }
}
