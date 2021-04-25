#ifndef TORERO_POINT_CLOUD_MANAGER_H
#define TORERO_POINT_CLOUD_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"
// Torero includes
#include "torero/definition/types.h"
#include "torero/gl/shader.h"
#include "torero/point_cloud/cloud.h"

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

  namespace point_cloud {
    class Manager
    {
    public:
      /*
     * ### Constructor
     *
     * This function will create the **Point cloud manager** *class*, it requires the **Core**
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
      // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
      // ------------------------------------------------------------------------------------ //
      /*
     * ### Adding a new 3D point cloud
     *
     * This will add a new point cloud with values type `torero::PointXYZ`, you must also
     * define a **RGB color** to color all the points. It will return the point cloud's **ID**,
     * this will be useful if you want to modify properties or values of the created point cloud.
     *
     * **Arguments**
     * {const std::vector<torero::PointXYZ>*} point_cloud = Address to the point cloud data
     * (see data types for more information).
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const float} color_red = red color to paint the cloud, range: 0.0f -> 255.0f.
     * {const float} color_green = green color to paint the cloud, range: 0.0f -> 255.0f.
     * {const float} color_blue = blue color to paint the cloud, range: 0.0f -> 255.0f.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const std::vector<torero::PointXYZ> *point_cloud,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const float color_red = 255.0f,
                const float color_green = 255.0f,
                const float color_blue = 255.0f,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Adding a new 3D point cloud with intensity values
     *
     * This will add a new point cloud with values type `torero::PointXYZI`, the intensity
     * value is included in the data and you must define the maximum value that the intensity
     * could reach. It will return the point cloud's **ID**, this will be useful if you want
     * to modify properties or values of the created point cloud.
     *
     * **Arguments**
     * {const std::vector<torero::PointXYZI>*} point_cloud = Address to the point cloud data
     * (see data types for more information).
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const torero::point_cloud::ColorMode} color_mode = see **color mode** *section*.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const std::vector<torero::PointXYZI> *point_cloud,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const torero::point_cloud::ColorMode color_mode
                = torero::point_cloud::ColorMode::ColorMap,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Adding a new 3D point cloud with RGB colors
     *
     * This will add a new point cloud with values type `torero::PointXYZRGB`, each point
     * must include its own **RGB color**. It will return the point cloud's **ID**, this will
     * be useful if you want to modify properties or values of the created point cloud.
     *
     * **Arguments**
     * {const std::vector<torero::PointXYZRGB>*} point_cloud = Address to the point cloud data
     * (see data types for more information).
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const std::vector<torero::PointXYZRGB> *point_cloud,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Adding a new 3D point cloud with RGB colors and Intensity
     *
     * This will add a new point cloud with values type `torero::PointXYZRGBI`, each point
     * must include its own **RGB color** and **Intensity**. It will return the point cloud's
     * **ID**, this will be useful if you want to modify properties or values of the
     * created point cloud.
     *
     * **Arguments**
     * {const std::vector<torero::PointXYZRGBI>*} point_cloud = Address to the point cloud data
     * (see data types for more information).
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const std::vector<torero::PointXYZRGBI> *point_cloud,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Adding a new 3D point cloud with RGB colors and transparency
     *
     * This will add a new point cloud with values type `torero::PointXYZRGBA`, each point
     * must include its own **RGB color** and its Alpha value (transparency). It will return
     * the point cloud's **ID**, this will be useful if you want to modify properties or values
     * of the created point cloud.
     *
     * **Arguments**
     * {const std::vector<torero::PointXYZRGBA>*} point_cloud = Address to the point cloud data
     * (see data types for more information).
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const std::vector<torero::PointXYZRGBA> *point_cloud,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Adding a new 3D point cloud with an existing Vertex Array Object
     *
     * This will add a new point cloud with values type `torero::PointXYZI`, each point
     * must include its own **coordinates** and its **intensity**. It will return
     * the point cloud's **ID**, this will be useful if you want to modify properties or values
     * of the created point cloud.
     *
     * The buffer must have enabled the attributes 0 and 1;
     *   - **0** is position coordinates using XYZ [sizeof(float) * 3]
     *   - **1** is intensity [sizeof(float)]
     *
     * **Arguments**
     * {const GLuint} vertex_array_buffer_id = OpenGL vertex array buffer ID
     * {const std::string} name = Title to display for this point cloud.
     * {const torero::Pose3D*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} visible = visibility of this point cloud.
     * {const float} point_size = point size for each point in the cloud.
     * {const float} maximum_intensity_value = maximum value that the intensity could have.
     *
     * **Returns**
     * {PCMid} Point cloud identification number (use it for future modifications)
     *
     */
      PCMid add(const GLuint vertex_array_object_id, const GLsizei *buffer_size,
                const std::string &name,
                const torero::Pose3D *coordinate_frame = nullptr,
                const torero::point_cloud::ColorMode color_mode
                = torero::point_cloud::ColorMode::ColorMap,
                const bool visible = true,
                const float point_size = 1.0f,
                const float maximum_intensity_value = 1.0f);
      /*
     * ### Changing the visibility of a point cloud
     *
     * This function changes the visibility of the point cloud with *identification number* = `id`.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const bool} visible = visibility: `true` for visible, `false` for hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool set_visibility(PCMid id, const bool visible = true);
      /*
     * ### Changing the colormap's palette of a point cloud
     *
     * Sets the colormap palette. You must specify the size of the palette, the maximum
     * readable size is 10 colors per **colormap**. You can only specify one colormap for
     * each Cloud.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const algebraica::vec3f*} colors = Address of a color's palette array.
     * {const unsigned int} quantity = quantity of array's elements.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     * **Errors**
     * A **segmentation fault** will happend if `quantity` is bigger than the number of elements
     * in the array `colors`.
     *
     */
      bool set_colormap(PCMid id, const torero::ColorRGB color);
      bool set_colormap(PCMid id, const std::vector<torero::ColorRGB> &colors);
      /*
     * ### Setting the color mode
     *
     * Sets the color mode; it could be *grayscale*, *monochrome*, *variable* or *none*. See
     * **color mode** types for more information.
     *
     * torero::point_cloud::ColorMode::INTENSITY -> Colorize points multiplying *intensity* and
     *                                              first color in *color_palete*
     * torero::point_cloud::ColorMode::SOLID     -> Colorize all points using only the first color in
     *                                              *color_palete*
     * torero::point_cloud::ColorMode::COLORMAP  -> Colorize points interpolating the *color_palete*
     *                                              using the point intensity
     * torero::point_cloud::ColorMode::DATA      -> Use this when you specify colors for each point
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const torero::point_cloud::ColorMode} color_mode = new color mode.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool set_color_mode(PCMid id,
                          const torero::point_cloud::ColorMode color_mode
                          = torero::point_cloud::ColorMode::ColorMap);
      /*
     * ### Setting the transformation matrix
     *
     * This function changes the transformation matrix (coordinate system's origin and
     * orientation) of the **point cloud** with *identification number* = `id`.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const torero::Pose3D*} transformation_matrix = Address to the new transformation matrix.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool set_coordiante_frame(PCMid id, const torero::Pose3D *coordinate_frame);
      bool set_pose(PCMid id, torero::Pose3D *pose);
      /*
     * ### Translating the point cloud
     *
     * This function translates the **point cloud** with *identification number* = `id` a
     * distance in **X**, **Y** and **Z** from its current position.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const float} x = Translation distance in **X** axis.
     * {const float} y = Translation distance in **Y** axis.
     * {const float} z = Translation distance in **Z** axis.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool translate(PCMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      /*
     * ### Rotating the point cloud
     *
     * This function rotates the **point cloud** with *identification number* = `id` using euler
     * angles; **pitch**, **yaw** and **roll** from its current orientation (see the coordinate
     * systems for more information).
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const float} pitch = Pitch angle in radians.
     * {const float} yaw = Yaw angle in radians.
     * {const float} roll = Roll angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool rotate(PCMid id,
                  const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      /*
     * ### Rotating the point cloud around its X axis
     *
     * This function rotates the **point cloud** with *identification number* = `id` around
     * its **X** axis.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool rotate_in_x(PCMid id, const float angle);
      /*
     * ### Rotating the point cloud around its Y axis
     *
     * This function rotates the **point cloud** with *identification number* = `id` around
     * its **Y** axis.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool rotate_in_y(PCMid id, const float angle);
      /*
     * ### Rotating the point cloud around its Z axis
     *
     * This function rotates the **point cloud** with *identification number* = `id` around
     * its **Z** axis.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool rotate_in_z(PCMid id, const float angle);
      /*
     * ### Updating the data of a point cloud
     *
     * This function updates the data of the **point cloud** with *identification number* = `id`.
     * You should call this function every time the point cloud's data is modified or call
     * update_all() but it will update each point cloud even if some of them were not changed.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to update.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool update(PCMid id);
      /*
     * ### Updating the data of every point cloud
     *
     * This function updates the data of every **point cloud** that is visible, note that it
     * will update each point cloud even if some of them were not changed (considering performance).
     *
     */
      void update_all();
      /*
     * ### Drawing the data of a point cloud
     *
     * This function draws the data of the **point cloud** with *identification number* = `id`
     * into the screen.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to draw.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool draw(PCMid id);
      /*
     * ### Drawing every point cloud in screen
     *
     * This function draws the data of every **point cloud** that is visible into the screen.
     * You should always consider performance, do not call this function more than once if is
     * not necessary.
     *
     */
      void draw_all();
      /*
     * ### Deleting an specific point cloud
     *
     * This function deletes the **point cloud** with *identification number* = `id`, the point
     * cloud objects are stored in a vector, when you delete one of them, the object is deleted
     * but not the vector's element, you should call purge when possible if you constantly
     * create and delete point clouds.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to delete.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool delete_cloud(PCMid id);
      /*
     * ### Deleting all point clouds and cleaning the container
     *
     * This function **deletes all** the point clouds and **clear** the vector container,
     * call this function as much as possible if you **create and delete** point clouds
     * **constantly** so, the vector's size becomes 0 again, BUT, all the stored `PCMid` you had
     * would **not work** again.
     *
     */
      void purge();
      /*
     * ### Connecting an specific point cloud's update to an external signal
     *
     * This function connects the **point cloud** with *identification number* = `id` and
     * updates its data every time the signal is **triggered**. It disconnects any previous
     * connection with the same point cloud.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to delete.
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
      bool connect(PCMid id, boost::signals2::signal<void ()> *signal);
      /*
     * ### Connecting the function update_all() to an external signal
     *
     * This function connects the member `update_all()` to an external signal and
     * updates the data of all point clouds every time the signal is **triggered**.
     * It disconnects any previous connection with the same member function.
     *
     * **Arguments**
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     */
      void connect_all(boost::signals2::signal<void ()> *signal);

    private:
      torero::Core *core_;

      torero::gl::Shader *shader_;
      std::vector<torero::point_cloud::Element> point_clouds_;

      boost::signals2::connection connection_draw_, connection_update_;
    };
  }
}
#endif // TORERO_POINT_CLOUD_MANAGER_H