#ifndef TORERO_OBJECT_MANAGER_H
#define TORERO_OBJECT_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/objects.h"
#include "include/shader.h"
#include "include/texture.h"
#include "include/types.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace Toreo {
  class Core;

  class ObjectManager
  {
  public:
    explicit ObjectManager(Core *core);
    ~ObjectManager();

    // ------------------------------------------------------------------------------------ //
    // ------------------------------- OBJECTS MANAGEMENT --------------------------------- //
    // ------------------------------------------------------------------------------------ //
    // This will add a new point cloud with values Visualizer::pointXYZ, you must also define
    // a RGB color. It will return the point cloud's ID, it will be useful if you want to
    // modify properties or values of the created point cloud.
    OMid add_boxes(const std::vector<Visualizer::Object> *objects,
                   const std::string name,
                   const algebraica::mat4f *transformation_matrix = nullptr,
                   const bool visible = true);
    OMid add_circles(const std::vector<Visualizer::Object> *objects,
                     const std::string name,
                     const algebraica::mat4f *transformation_matrix = nullptr,
                     const bool visible = true);
    OMid add_cylinders(const std::vector<Visualizer::Object> *objects,
                       const std::string name,
                       const algebraica::mat4f *transformation_matrix = nullptr,
                       const bool visible = true);
    OMid add_squares(const std::vector<Visualizer::Object> *objects,
                     const std::string name,
                     const algebraica::mat4f *transformation_matrix = nullptr,
                     const bool visible = true);
    // This will change the input data for the Point cloud with ID = id
    bool change_input(OMid id, const std::vector<Visualizer::Object> *objects);
    // Sets the transformation matrix for the Point cloud with ID = id.
    bool set_transformation_matrix(OMid id, const algebraica::mat4f *transformation_matrix);
    // Changes the visibility of the point cloud,
    // returns false if point cloud with ID = id was not found.
    bool set_visibility(OMid id, const bool visible = true);
    // Translates the point cloud using X, Y and Z values
    bool translate(OMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    // Rotates the point cloud using Pitch, Yaw and Roll values
    bool rotate(OMid id, const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    // Performs a rotation in the point cloud around its X axis
    bool rotate_in_x(OMid id, const float angle);
    // Performs a rotation in the point cloud around its Y axis
    bool rotate_in_y(OMid id, const float angle);
    // Performs a rotation in the point cloud around its Z axis
    bool rotate_in_z(OMid id, const float angle);
    // Updates the data of the point cloud.
    // Returns false if the point cloud was not proerly created.
    bool update(OMid id);
    // Updates all the point cloud's data
    void update_all();
    // Draws the point cloud into the screen.
    // Returns false if the point cloud was not proerly created.
    bool draw(OMid id);
    // Draws all the point clouds
    void draw_all();
    // Deletes the point cloud width ID = id,
    // Returns false if point cloud is already deleted.
    bool delete_object(OMid id);
    // This will delete all point clouds and free memory, if you create and delete point clouds
    // dinamically call this method from time to time to increase the program performance,
    // all the previous point clouds' IDs will not work anymore
    void purge();
    // Connects the point cloud with ID = id to a signal for updating the data.
    // Returns false if the point cloud does not exists.
    // If you delete the point cloud with ID = id or purge all, you will need to call this
    // function again to a new id or new point cloud.
    bool connect(OMid id, boost::signals2::signal<void ()> *signal);
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
    void prepare_hollow_cylinder();
    void prepare_solid_cylinder();
    void prepare_hollow_box();
    void prepare_solid_box();
    void prepare_hollow_square();
    void prepate_solid_square();
    void prepare_hollow_circle();
    void prepare_solid_circle();
    void prepare_arrow();

    void updated_camera();

    Core *core_;

    Shader *shader_;
    Buffer *hollow_cylinder_, *hollow_box_, *solid_box_, *solid_cylinder_;
    Buffer *hollow_square_, *solid_square_, *hollow_circle_, *solid_circle_, *solid_arrow_;
    GLint u_pv_, u_camera_position_, u_point_light_, u_point_light_color_, u_ao_;
    GLint u_directional_light_, u_directional_light_color_;
    std::vector<Visualizer::ObjectElement> objects_;
    Texture *ao_cylinder_, *ao_box_, *ao_square_, *ao_circle_, *ao_arrow_;

    boost::signals2::connection signal_updated_camera_, signal_draw_all_;
    boost::signals2::connection signal_updated_all_;
  };
  }

#endif // TORERO_OBJECT_MANAGER_H
