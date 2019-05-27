#ifndef TORERO_TRAJECTORY_MANAGER_H
#define TORERO_TRAJECTORY_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/shader.h"
#include "torero/gl/texture.h"
#include "torero/trajectory/trajectories.h"

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

  namespace trajectory {
    class Manager
    {
    public:
      /*
     * ### Constructor
     *
     * This function will create the **Trajectory manager** *class*, it requires the **Core**
     * *class* already implemented, you will get an error at the creation if `core` was not
     * previously defined.
     *
     * **Arguments**
     * {Core*} core = Address to a **Core** *object*.
     *
     * **Errors**
     * This will throw an error if **Core** *object* was not previouly created.
     *
     */
      explicit Manager(torero::Core *core);
      ~Manager();

      // ------------------------------------------------------------------------------------ //
      // ------------------------------ TRAJECTORY MANAGEMENT ------------------------------- //
      // ------------------------------------------------------------------------------------ //
      /*
     * ### Adding a new 3D trajectory
     *
     * This will add a new trajectory with values type `torero::Trajectory`, you must also
     * define a **RGB color** to color all the points. It will return the trajectory's **ID**,
     * this will be useful if you want to modify properties or values of the created trajectory.
     *
     * **Arguments**
     * {const std::vector<torero::Trajectory>*} trajectories = Address to the trajectory data
     * (see data types for more information).
     * {const std::string} name = Title to display for this trajectory.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const torero::LineType} type = Type of line's appearance (see the description of
     * data types for more information).
     * {const bool} visible = Visibility of this trajectory.
     *
     * **Returns**
     * {TMid} Trajectory identification number (use it for future modifications)
     *
     */
      TMid add(const std::vector<torero::trajectory::Trajectory> *trajectories,
               const std::string name,
               const algebraica::mat4f *transformation_matrix = nullptr,
               const torero::trajectory::LineType type = torero::trajectory::Solid,
               const bool visible = true);
      /*
     * ### Changing the trajectory data input
     *
     * This function changes the data input for a different **trajectory**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const std::vector<torero::Trajectory>*} trajectories = new address to a 3D trajectory's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool change_input(TMid id, const std::vector<torero::trajectory::Trajectory> *trajectories);
      /*
     * ### Changing the line type of a trajectory
     *
     * This function changes the line type of the trajectory (appearance) with *identification
     * number* = `id`.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const torero::LineType} type = Type of line's appearance (see the description of
     * data types for more information).
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool set_type(TMid id, const torero::trajectory::LineType type = torero::trajectory::Solid);
      /*
     * ### Changing the visibility of a trajectory
     *
     * This function changes the visibility of the trajectory with *identification number* = `id`.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const bool} visible = Visibility: `true` for visible, `false` for hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool set_visibility(TMid id, const bool visible = true);
      /*
     * ### Setting the transformation matrix
     *
     * This function changes the transformation matrix (coordinate system's origin and
     * orientation) of the **trajectory** with *identification number* = `id`.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const algebraica::mat4f*} transformation_matrix = Address to the new transformation matrix.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool set_transformation_matrix(TMid id, const algebraica::mat4f *transformation_matrix);
      /*
     * ### Translating the trajectory
     *
     * This function translates the **trajectory** with *identification number* = `id` a
     * distance in **X**, **Y** and **Z** from its current position.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const float} x = Translation distance in **X** axis.
     * {const float} y = Translation distance in **Y** axis.
     * {const float} z = Translation distance in **Z** axis.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool translate(TMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      /*
     * ### Rotating the trajectory
     *
     * This function rotates the **trajectory** with *identification number* = `id` using euler
     * angles; **pitch**, **yaw** and **roll** from its current orientation (see the coordinate
     * systems for more information).
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const float} pitch = Pitch angle in radians.
     * {const float} yaw = Yaw angle in radians.
     * {const float} roll = Roll angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool rotate(TMid id, const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      /*
     * ### Rotating the trajectory around its X axis
     *
     * This function rotates the **trajectory** with *identification number* = `id` around
     * its **X** axis.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool rotate_in_x(TMid id, const float angle);
      /*
     * ### Rotating the trajectory around its Y axis
     *
     * This function rotates the **trajectory** with *identification number* = `id` around
     * its **Y** axis.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool rotate_in_y(TMid id, const float angle);
      /*
     * ### Rotating the trajectory around its Z axis
     *
     * This function rotates the **trajectory** with *identification number* = `id` around
     * its **Z** axis.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool rotate_in_z(TMid id, const float angle);
      /*
     * ### Updating the data of a trajectory
     *
     * This function updates the data of the **trajectory** with *identification number* = `id`.
     * You should call this function every time the trajectory's data is modified or call
     * update_all() but it will update each trajectory even if some of them were not changed.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to update.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool update(TMid id);
      /*
     * ### Updating the data of every trajectory
     *
     * This function updates the data of every **trajectory** that is visible, note that it
     * will update each trajectory even if some of them were not changed (considering performance).
     *
     */
      void update_all();
      /*
     * ### Drawing the data of a trajectory
     *
     * This function draws the data of the **trajectory** with *identification number* = `id`
     * into the screen.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to draw.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool draw(TMid id);
      /*
     * ### Drawing every trajectory in screen
     *
     * This function draws the data of every **trajectory** that is visible into the screen.
     * You should always consider performance, do not call this function more than once if is
     * not necessary.
     *
     */
      void draw_all();
      /*
     * ### Deleting an specific trajectory
     *
     * This function deletes the **trajectory** with *identification number* = `id`, the point
     * cloud objects are stored in a vector, when you delete one of them, the object is deleted
     * but not the vector's element, you should call purge when possible if you constantly
     * create and delete trajectories.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to delete.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool delete_trajectory(TMid id);
      /*
     * ### Deleting all trajectories and cleaning the container
     *
     * This function **deletes all** the trajectories and **clear** the vector container,
     * call this function as much as possible if you **create and delete** trajectories
     * **constantly** so, the vector's size becomes 0 again, BUT, all the stored `TMid` you had
     * would **not work** again.
     *
     */
      void purge();
      /*
     * ### Connecting an specific trajectory's update to an external signal
     *
     * This function connects the **trajectory** with *identification number* = `id` and
     * updates its data every time the signal is **triggered**. It disconnects any previous
     * connection with the same trajectory.
     *
     * **Arguments**
     * {TMid} id = **id** of the trajectory you want to delete.
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     * **Returns**
     * {bool} Returns `false` if the trajectory with **id** was **not** found.
     *
     */
      bool connect(TMid id, boost::signals2::signal<void ()> *signal);
      /*
     * ### Connecting the function update_all() to an external signal
     *
     * This function connects the member `update_all()` to an external signal and
     * updates the data of all trajectories every time the signal is **triggered**.
     * It disconnects any previous connection with the same member function.
     *
     * **Arguments**
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     */
      void connect_all(boost::signals2::signal<void ()> *signal);

    private:
      void updated_camera();
      void initialize();

      torero::Core *core_;

      torero::gl::Shader *shader_;
      GLint u_point_light_, u_point_light_color_, u_directional_light_;
      GLint u_directional_light_color_, u_camera_position_;
      std::vector<torero::trajectory::Element> trajectories_;
      torero::gl::Texture *solid_, *dotted_, *dashed_, *arrowed_;

      boost::signals2::connection connection_draw_, connection_update_;
    };
  }
}
#endif // TORERO_TRAJECTORY_MANAGER_H
