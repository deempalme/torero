#include "torero/covariance/manager.h"
#include "torero/core.h"

namespace torero {
  namespace covariance {
    Manager::Manager(torero::Core *core) :
      core_(core),
      shader_(new torero::gl::Shader(torero::k_covariance_vertex, torero::k_covariance_fragment)),
      plane_(new torero::gl::Buffer(true)),
      plane_data_size_(0),
      plane_type_size_(static_cast<GLsizei>(sizeof(torero::PointXY<float>))),
      matrices_(0),
      resolution_(20),
      size_(5.0),
      connection_draw_(core->syncronize(torero::Order::Covariance,
                                        boost::bind(&torero::covariance::Manager::draw_all,
                                                    this)))
    {
      if(!shader_->use())
        std::cout << shader_->error_log() << std::endl;

      prepare_plane();
    }

    Manager::~Manager(){
      for(torero::covariance::Element &matrix : matrices_)
        if(matrix.matrix != nullptr){
          if(matrix.connection.connected())
            matrix.connection.disconnect();
          delete matrix.matrix;
        }

      if(connection_draw_.connected())
        connection_draw_.disconnect();

      if(connection_update_.connected())
        connection_update_.disconnect();

      if(plane_)
        delete plane_;

      if(shader_)
        delete shader_;
    }

    CVMid Manager::add(const std::vector<torero::covariance::Covariance> *matrices,
                       const std::string name,
                       const algebraica::mat4f *transformation_matrix,
                       const bool visible){
      torero::covariance::Element matrix = {
        new torero::covariance::Covariances(shader_, matrices, plane_, plane_data_size_),
        name, visible, boost::signals2::connection()
      };
      if(transformation_matrix != nullptr)
        matrix.matrix->set_transformation_matrix(transformation_matrix);

      matrices_.push_back(matrix);
      return matrices_.size() - 1;
    }

    bool Manager::change_input(CVMid id,
                               const std::vector<torero::covariance::Covariance> *matrices){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->change_input(matrices);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::set_transformation_matrix(CVMid id,
                                            const algebraica::mat4f *transformation_matrix){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->set_transformation_matrix(transformation_matrix);
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::set_resolution(const int resolution, const double size)  {
      resolution_ = resolution;
      size_ = size;
      prepare_plane();
    }

    bool Manager::set_visibility(CVMid id, const bool visible){
      if(matrices_.size() > id){
        matrices_[id].visibility = visible;
        return true;
      }else
        return false;
    }

    bool Manager::translate(CVMid id, const float x, const float y, const float z){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->translate(-y, z, -x);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate(CVMid id, const float pitch, const float yaw, const float roll){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->rotate(-pitch, yaw, -roll);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_x(CVMid id, const float angle){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->rotate_in_z(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_y(CVMid id, const float angle){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->rotate_in_x(-angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::rotate_in_z(CVMid id, const float angle){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          matrices_[id].matrix->rotate_in_y(angle);
          return true;
        }else
          return false;
      else
        return false;
    }

    bool Manager::update(CVMid id){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr && matrices_[id].visibility){
          matrices_[id].matrix->update();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::update_all(){
      for(torero::covariance::Element &matrix : matrices_)
        if(matrix.matrix != nullptr && matrix.visibility)
          matrix.matrix->update();
    }

    bool Manager::draw(OMid id){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr && matrices_[id].visibility){
          matrices_[id].matrix->draw();
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::draw_all(){
      for(torero::covariance::Element &matrix : matrices_)
        if(matrix.matrix != nullptr && matrix.visibility)
          matrix.matrix->draw();
    }

    bool Manager::delete_covariance(CVMid id){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          if(matrices_[id].connection.connected())
            matrices_[id].connection.disconnect();
          delete matrices_[id].matrix;
          matrices_[id].matrix = nullptr;
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::purge(){
      for(torero::covariance::Element &matrix : matrices_)
        if(matrix.matrix != nullptr){
          if(matrix.connection.connected())
            matrix.connection.disconnect();
          delete matrix.matrix;
        }
      matrices_.clear();
    }

    bool Manager::connect(CVMid id, boost::signals2::signal<void ()> *signal){
      if(matrices_.size() > id)
        if(matrices_[id].matrix != nullptr){
          if(matrices_[id].connection.connected())
            matrices_[id].connection.disconnect();
          matrices_[id].connection =
              signal->connect(boost::bind(&torero::covariance::Manager::update, this, id));
          return true;
        }else
          return false;
      else
        return false;
    }

    void Manager::connect_all(boost::signals2::signal<void ()> *signal){
      if(connection_update_.connected())
        connection_update_.disconnect();
      connection_update_ = signal->connect(boost::bind(&torero::covariance::Manager::update_all,
                                                       this));
    }

    //  This is how triangle strips work; to create a row of rectangles it is necessary to define
    //  all points (+), first and last must be repeated, the ordering must be the following:
    //    1. Start with the row's top-left point
    //       1a. Repeat that point
    //    2. Follow the vertical line until the next point
    //    3. Follor the diagonal line until the next point
    //    4. Repeat the steps 3 and 4 until all points are defined
    //       4a. Repeat the last point
    //    5. If you have multiple rows, repeat all steps for each row
    //
    //   + –– + –– + –– + –– + –– + –– + –– +
    //   |  / |  / |  / |  / |  / |  / |  / |  <- One Row
    //   | /  | /  | /  | /  | /  | /  | /  |
    //   + –– + –– + –– + –– + –– + –– + –– +
    //   |  / |  / |  / |  / |  / |  / |  / |  <- Another Row
    //   | /  | /  | /  | /  | /  | /  | /  |
    //   + –– + –– + –– + –– + –– + –– + –– +
    //
    //  The total number of defined points is the following (R = resolution_):
    //     Total points = R * R
    //     Repeated initial and final points = 2 * R
    //     Redefined points = R * R - 2 * R
    //
    //     Total : (R * R) + (2 * R) + (R * R - 2 * R) = 2 * R * ( R + 1 - 1 ) = 2 * R * R
    //
    void Manager::prepare_plane(){

      const float separation{static_cast<float>(size_ / static_cast<double>(resolution_))};
      const float centerer{static_cast<float>(-size_ / 2.0)};
      const std::size_t total{static_cast<std::size_t>(2 * resolution_ * resolution_)};
      const int last_column{resolution_ - 1};
      std::vector<torero::PointXY<float>> lines(total);

      int column{0};
      int row{0};
      int toggler{0};
      for(std::size_t i = 0; i < total; ++i){
        lines[i].point.x = centerer + column * separation;
        lines[i].point.y = centerer + (toggler + row) * separation;

        // It is preferred to repeat the operations instead of saving the result in a
        // variable for each cycle, because, this will only be repeated twice every row
        if(column == 0){
          lines[++i].point.x = centerer + column * separation;
          lines[i].point.y   = centerer + (toggler + row) * separation;
        }
        if(column >= last_column){
          lines[++i].point.x = centerer + column * separation;
          lines[i].point.y   = centerer + (toggler + row) * separation;
          column = 0;
          toggler = -1;
          ++row;
        }
        if(toggler++ >= 1)
          toggler = 0;
      }

      plane_data_size_ = plane_type_size_ * static_cast<GLsizei>(lines.size());

      plane_->allocate_array(lines.data(), plane_data_size_);
      plane_->buffer_release();
      plane_->vertex_release();
    }
  }
}
