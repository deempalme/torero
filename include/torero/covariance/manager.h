#ifndef TORERO_COVARIANCE_MANAGER_H
#define TORERO_COVARIANCE_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/covariance/covariance.h"
#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
#include "torero/gl/texture.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  namespace covariance {
    class Manager
    {
    public:
      explicit Manager(Core *core);
      ~Manager();

      // ------------------------------------------------------------------------------------ //
      // -------------------------- COVARIANCE MATRICES MANAGEMENT -------------------------- //
      // ------------------------------------------------------------------------------------ //
      CVMid add(const std::vector<torero::covariance::Covariance> *matrices,
                const std::string name,
                const algebraica::mat4f *transformation_matrix = nullptr,
                const bool visible = true);
      // This will change the input data for the Point cloud with ID = id
      bool change_input(CVMid id, const std::vector<torero::covariance::Covariance> *matrices);
      // Sets the transformation matrix for the Point cloud with ID = id.
      bool set_transformation_matrix(CVMid id, const algebraica::mat4f *transformation_matrix);
      /*
     * ### Sets the resolution and size of the graphics matrix
     *
     * The multivariate normal distribution's graphic is composed by a squared **matrix of
     * points** used to calculate the **probability density**, this functions sets the number
     * of rows and columns defined by the value of **resolution**, and sets the matrix's
     * physical dimension defined by the value in meters of **size**
     *
     * @param const int resolution : Number of lines per side
     * @param const double size : Side's dimension in meters
     *
     */
      void set_resolution(const int resolution = 20, const double size = 5.0);
      // Changes the visibility of the point cloud,
      // returns false if point cloud with ID = id was not found.
      bool set_visibility(CVMid id, const bool visible = true);
      // Translates the point cloud using X, Y and Z values
      bool translate(CVMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      // Rotates the point cloud using Pitch, Yaw and Roll values
      bool rotate(CVMid id, const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      // Performs a rotation in the point cloud around its X axis
      bool rotate_in_x(CVMid id, const float angle);
      // Performs a rotation in the point cloud around its Y axis
      bool rotate_in_y(CVMid id, const float angle);
      // Performs a rotation in the point cloud around its Z axis
      bool rotate_in_z(CVMid id, const float angle);
      // Updates the data of the point cloud.
      // Returns false if the point cloud was not proerly created.
      bool update(CVMid id);
      // Updates all the point cloud's data
      void update_all();
      // Draws the point cloud into the screen.
      // Returns false if the point cloud was not proerly created.
      bool draw(CVMid id);
      // Draws all the point clouds
      void draw_all();
      // Deletes the point cloud width ID = id,
      // Returns false if point cloud is already deleted.
      bool delete_covariance(CVMid id);
      // This will delete all point clouds and free memory, if you create and delete point clouds
      // dinamically call this method from time to time to increase the program performance,
      // all the previous point clouds' IDs will not work anymore
      void purge();
      // Connects the point cloud with ID = id to a signal for updating the data.
      // Returns false if the point cloud does not exists.
      // If you delete the point cloud with ID = id or purge all, you will need to call this
      // function again to a new id or new point cloud.
      bool connect(CVMid id, boost::signals2::signal<void ()> *signal);
      /*
     * ### Connecting the function update_all() to an external signal
     *
     * This function connects the member `update_all()` to an external signal and
     * updates the data of all point clouds every time the signal is **triggered**.
     * It disconnects any previous connection with the same member function.
     *
     * **Parameters**
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     */
      void connect_all(boost::signals2::signal<void ()> *signal);

    private:
      void prepare_plane();

      void updated_camera();

      torero::Core *core_;

      torero::gl::Shader *shader_;
      torero::gl::Buffer *plane_;
      GLsizei plane_data_size_;
      const GLsizei plane_type_size_;
      std::vector<torero::covariance::Element> matrices_;

      int resolution_;
      double size_;

      boost::signals2::connection connection_draw_;
      boost::signals2::connection connection_update_;
    };
  }
}
#endif // TORERO_COVARIANCE_MANAGER_H
