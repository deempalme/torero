#ifndef TORERO_GROUND_MANAGER_H
#define TORERO_GROUND_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/line_grid.h"
#include "torero/ground.h"
#include "torero/shader.h"
#include "torero/texture.h"
#include "torero/type_definitions.h"

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

  class GroundManager
  {
  public:
    /*
     * ### Constructor
     *
     * This function will create the **Ground manager** *class*, it requires the **Core**
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
    explicit GroundManager(Core *core);
    ~GroundManager();

    // ------------------------------------------------------------------------------------ //
    // -------------------------------- GROUND MANAGEMENT --------------------------------- //
    // ------------------------------------------------------------------------------------ //
    /*
     * ### Adding a new *Occupancy Grid*
     *
     * This will add a new ground with values type `torero::GroundGrid`, you must
     * define the probabilities value. You may choose to display different grid's height
     * proportional to its probability. White is for free space, black for occupied space and
     * middle grey for unkonw (may also be for probability ~50%). It will return the
     * ground's **ID**, this will be useful if you want to modify properties or values of
     * the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::GroundGrid>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const float} width = Ground's width in meters.
     * {const float} length = Ground's length in meters.
     * {const unsigned int} number_of_elements_through_width = Quantity of elements along the
     * lateral axis.
     * {const unsigned int} number_of_elements_through_length = Quantity of elements along the
     * longitudinal axis.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     * {const bool} calculate_height = If true calculates a proportional height for each grid
     * depending on its probability.
     * {const float} maximum_height = Maximum reachable height (when probability = 100%).
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::GroundGrid> *ground,
             const std::string name,
             const float width = 100.0f,
             const float length = 100.0f,
             const unsigned int number_of_elements_through_width = 100u,
             const unsigned int number_of_elements_through_length = 100u,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true,
             const bool calculate_height = true,
             const float maximum_height = 2.0f);
    /*
     * ### Adding a new *uniform* two-dimensional ground
     *
     * This will add a new ground with values type `torero::Ground2D`, you must also
     * define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::Ground2D>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const float} width = Ground's width in meters.
     * {const float} length = Ground's length in meters.
     * {const unsigned int} number_of_elements_through_width = Quantity of elements along the
     * lateral axis.
     * {const unsigned int} number_of_elements_through_length = Quantity of elements along the
     * longitudinal axis.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::Ground2D> *ground,
             const std::string name,
             const float width = 100.0f,
             const float length = 100.0f,
             const unsigned int number_of_elements_through_width = 100u,
             const unsigned int number_of_elements_through_length = 100u,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Adding a new *uniform* three-dimensional ground
     *
     * This will add a new ground with values type `torero::Ground3D`, you must also
     * define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::Ground3D>*} trajectories = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const float} width = Ground's width in meters.
     * {const float} length = Ground's length in meters.
     * {const unsigned int} number_of_elements_through_width = Quantity of elements along the
     * lateral axis.
     * {const unsigned int} number_of_elements_through_length = Quantity of elements along the
     * longitudinal axis.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::Ground3D> *ground,
             const std::string name,
             const float width = 100.0f,
             const float length = 100.0f,
             const unsigned int number_of_elements_through_width = 100u,
             const unsigned int number_of_elements_through_length = 100u,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Adding a new two-dimensional ground
     *
     * This will add a new ground with values type `torero::FreeGround2D`, with this **data
     * type** you can define a **position** and **size** for each segment. You must also
     * define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::FreeGround2D>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::FreeGround2D> *ground,
             const std::string name,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Adding a new three-dimensional ground
     *
     * This will add a new ground with values type `torero::FreeGround3D`, with this **data
     * type** you can define a **position**, **size** and **height** for each segment. You must
     * also define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::FreeGround3D>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::FreeGround3D> *ground,
             const std::string name,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Adding a new two-dimensional ground that uses polar values
     *
     * This will add a new ground with values type `torero::FreePolarGround2D`, with this **data
     * type** you can define a **distance**, **angle** and **size** for each segment. You must also
     * define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::FreePolarGround2D>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::FreePolarGround2D> *ground,
             const std::string name,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Adding a new three-dimensional ground that uses polar values
     *
     * This will add a new ground with values type `torero::FreePolarGround3D`, with this **data
     * type** you can define a **position**, **size** and **height** for each segment. You must
     * also define a **RGBA color** to color each segment. It will return the ground's **ID**,
     * this will be useful if you want to modify properties or values of the created ground.
     *
     * **Arguments**
     * {const std::vector<torero::FreePolarGround3D>*} ground = Address to the ground data
     * (see data types for more information).
     * {const std::string} name = Title to display for this ground.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     * {const bool} ground_visible = Visibility of this ground.
     *
     * **Returns**
     * {GMid} Ground identification number (use it for future modifications)
     *
     */
    GMid add(const std::vector<torero::FreePolarGround3D> *ground,
             const std::string name,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool ground_visible = true);
    /*
     * ### Changing the ground data input: uniform 2D ground
     *
     * This function changes the data input for a different *uniform* two-dimensional **ground**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::Ground2D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::Ground2D> *ground);
    /*
     * ### Changing the ground data input: uniform 3D ground
     *
     * This function changes the data input for a different *uniform* three-dimensional **ground**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::Ground3D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::Ground3D> *ground);
    /*
     * ### Changing the ground data input: 2D ground
     *
     * This function changes the data input for a different two-dimensional **ground**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::FreeGround2D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::FreeGround2D> *ground);
    /*
     * ### Changing the ground data input: 3D ground
     *
     * This function changes the data input for a different three-dimensional **ground**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::FreeGround3D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::FreeGround3D> *ground);
    /*
     * ### Changing the ground data input: polar 2D ground
     *
     * This function changes the data input for a different two-dimensional **ground**
     * with *identification number* = `id`. **It uses polar values**.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::FreePolarGround2D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::FreePolarGround2D> *ground);
    /*
     * ### Changing the ground data input: polar 3D ground
     *
     * This function changes the data input for a different three-dimensional **ground**
     * with *identification number* = `id`. **It uses polar values**.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const std::vector<torero::FreePolarGround3D>*} trajectories = new address to a 3D ground's
     * data.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool change_input(GMid id, const std::vector<torero::FreePolarGround3D> *ground);
    /*
     * ### Adding fog to the scene
     *
     * This function will add/remove fog to the ground with *identification number* = `id`.
     * The ground segments will start vanishing after certain distance from the camera.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const bool} visible = visibility: `true` for visible, `false` for hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool fog_visibility(GMid id, const bool visible = true);
    /*
     * ### Adding a grid of lines
     *
     * This will add a grid of lines, you must specify the **size** and **quantity of lines** per
     * axis (**X** and **Y**). If you do not declare the transformation matrix, then, it will
     * take the "fixed frame" transformation matrix.
     *
     * **Arguments**
     * {const float} width = Grid's width in meters.
     * {const float} length = Grid's length in meters.
     * {const unsigned int} line_quantity_through_width = Quantity of lines along the
     * lateral axis.
     * {const unsigned int} line_quantity_through_length = Quantity of lines along the
     * longitudinal axis.
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has been already** created.
     *
     */
    bool grid_add(const float width = 100.0f,
                  const float length = 100.0f,
                  const unsigned int line_quantity_through_width = 100u,
                  const unsigned int line_quantity_through_length = 100u,
                  const algebraica::mat4f *transformation_matrix = nullptr);
    /*
     * ### Changing the lines' color
     *
     * This function changes the lines' color of the grid.
     *
     * **Arguments**
     * {const float} r = RED intensity color (range: 0.0f  to 255.0f).
     * {const float} ǵ = GREEN intensity color (range: 0.0f  to 255.0f).
     * {const float} b = BLUE intensity color (range: 0.0f  to 255.0f).
     * {const float} alpha = ALPHA intensity color (range: 0.0f  to 255.0f).
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_color(const float r = 255.0f, const float g = 255.0f,
                    const float b = 255.0f, const float alpha = 255.0f);
    /*
     * ### Adding fog to the line grid
     *
     * This function will add/remove fog to the line grid. The lines will start blending with
     * the environment after certain distance from the camera.
     *
     * **Arguments**
     * {const bool} visible = visibility: `true` for visible, `false` for hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_fog(const bool visible = true);
    /*
     * ### Changing grid's properties
     *
     * This function changes the **dimension** and **quantity of lines** that has the line grid.
     *
     * **Arguments**
     * {const float} width = Grid's width in meters.
     * {const float} length = Grid's length in meters.
     * {const unsigned int} line_quantity_through_width = Quantity of lines along the
     * lateral axis.
     * {const unsigned int} line_quantity_through_length = Quantity of lines along the
     * longitudinal axis.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_properties(const float width = 100.0f,
                         const float length = 100.0f,
                         const unsigned int line_quantity_through_width = 100u,
                         const unsigned int line_quantity_through_length = 100u);
    /*
     * ### Rotating the line grid
     *
     * This function rotates the **line grid** using euler angles; **pitch**, **yaw** and
     * **roll** from its current orientation (see the [[coordinate systems|Coordinate-systems]]
     * for more information).
     *
     * **Arguments**
     * {const float} pitch = Pitch angle in radians.
     * {const float} yaw = Yaw angle in radians.
     * {const float} roll = Roll angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    /*
     * ### Translating the line grid
     *
     * This function translates the **line grid* a distance in **X**, **Y** and **Z**
     * from its current position.
     *
     * **Arguments**
     * {const float} x = Translation distance in **X** axis.
     * {const float} y = Translation distance in **Y** axis.
     * {const float} z = Translation distance in **Z** axis.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    /*
     * ### Changin the grid's transformation matrix
     *
     * This function changes the transformation matrix of the grid. If you do not declare
     * the transformation matrix, then, it will take the "fixed frame" transformation matrix.
     *
     * **Arguments**
     * {const algebraica::mat4f*} transformation_matrix = Address to the transformation matrix
     * that defines the coordinate system's origin and orientation.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_transformation_matrix(const algebraica::mat4f *transformation_matrix = nullptr);
    /*
     * ### Changing the line grid's visibility
     *
     * This function changes the visibility of the line grid.
     *
     * **Arguments**
     * {const bool} visible = Changes the visibility: `false` -> hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the line grid **has not been** created.
     *
     */
    bool grid_visibility(const bool visible = true);
    /*
     * ### Changing the ground's properties
     *
     * This function changes the size in meters of the ground with *identification number* = `id`.
     * You could also define how many elements the **ground matriy** will have.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} width = Ground's width in meters.
     * {const float} length = Ground's length in meters.
     * {const unsigned int} number_of_elements_through_width = Quantity of elements along the
     * lateral axis.
     * {const unsigned int} number_of_elements_through_length = Quantity of elements along the
     * longitudinal axis.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool ground_size(GMid id, const float width = 100.0f, const float length = 100.0f,
                     const unsigned int number_of_elements_through_width = 100u,
                     const unsigned int number_of_elements_through_length = 100u);
    /*
     * ### Changing the visibility of a ground
     *
     * This function changes the visibility of the ground with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const bool} visible = visibility: `true` for visible, `false` for hidden.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool set_visibility(GMid id, const bool visible = true);
    /*
     * ### Setting the transformation matrix
     *
     * This function changes the transformation matrix (coordinate system's origin and
     * orientation) of the **ground** with *identification number* = `id`.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const algebraica::mat4f*} transformation_matrix = Address to the new transformation matrix.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool set_transformation_matrix(GMid id, const algebraica::mat4f *transformation_matrix);
    /*
     * ### Translating the ground
     *
     * This function translates the **ground** with *identification number* = `id` a
     * distance in **X**, **Y** and **Z** from its current position.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} x = Translation distance in **X** axis.
     * {const float} y = Translation distance in **Y** axis.
     * {const float} z = Translation distance in **Z** axis.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool translate(GMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    /*
     * ### Rotating the ground
     *
     * This function rotates the **ground** with *identification number* = `id` using euler
     * angles; **pitch**, **yaw** and **roll** from its current orientation (see the
     * [[coordinate systems|Coordinate-systems]] for more information).
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} pitch = Pitch angle in radians.
     * {const float} yaw = Yaw angle in radians.
     * {const float} roll = Roll angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool rotate(GMid id, const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    /*
     * ### Rotating the ground around its X axis
     *
     * This function rotates the **ground** with *identification number* = `id` around
     * its **X** axis.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool rotate_in_x(GMid id, const float angle);
    /*
     * ### Rotating the ground around its Y axis
     *
     * This function rotates the **ground** with *identification number* = `id` around
     * its **Y** axis.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool rotate_in_y(GMid id, const float angle);
    /*
     * ### Rotating the ground around its Z axis
     *
     * This function rotates the **ground** with *identification number* = `id` around
     * its **Z** axis.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to modify.
     * {const float} angle = Angle in radians.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool rotate_in_z(GMid id, const float angle);
    /*
     * ### Updating the data of a ground
     *
     * This function updates the data of the **ground** with *identification number* = `id`.
     * You should call this function every time the ground's data is modified or call
     * update_all() but it will update each ground even if some of them were not changed.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to update.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool update(GMid id);
    /*
     * ### Updating the data of every ground
     *
     * This function updates the data of every **ground** that is visible, note that it
     * will update each ground even if some of them were not changed (considering performance).
     *
     */
    void update_all();
    /*
     * ### Drawing the data of a ground
     *
     * This function draws the data of the **ground** with *identification number* = `id`
     * into the screen.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to draw.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool draw(GMid id);
    /*
     * ### Drawing every ground in screen
     *
     * This function draws the data of every **ground** that is visible into the screen.
     * You should always consider performance, do not call this function more than once if is
     * not necessary.
     *
     */
    void draw_all();
    /*
     * ### Deleting an specific ground
     *
     * This function deletes the **ground** with *identification number* = `id`, the point
     * cloud objects are stored in a vector, when you delete one of them, the object is deleted
     * but not the vector's element, you should call purge when possible if you constantly
     * create and delete trajectories.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to delete.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool delete_ground(GMid id);
    /*
     * ### Deleting all trajectories and cleaning the container
     *
     * This function **deletes all** the trajectories and **clear** the vector container,
     * call this function as much as possible if you **create and delete** trajectories
     * **constantly** so, the vector's size becomes 0 again, BUT, all the stored `GMid` you had
     * would **not work** again.
     *
     */
    void purge();
    /*
     * ### Connecting an specific ground's update to an external signal
     *
     * This function connects the **ground** with *identification number* = `id` and
     * updates its data every time the signal is **triggered**. It disconnects any previous
     * connection with the same ground.
     *
     * **Arguments**
     * {GMid} id = **id** of the ground you want to delete.
     * {boost::signals2::signal<void ()>*} signal = boost signal to connect.
     *
     * **Returns**
     * {bool} Returns `false` if the ground with **id** was **not** found.
     *
     */
    bool connect(GMid id, boost::signals2::signal<void ()> *signal);
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

    Core *core_;

    Shader *ground_shader_;
    GLint u_projection_ground_, u_view_ground_, u_pv_ground_, u_point_light_ground_;
    GLint u_point_light_color_ground_, u_directional_light_ground_;
    GLint u_directional_light_color_ground_, u_camera_position_ground_;

    std::vector<torero::GroundElement> grounds_;

    LineGrid *grid_;
    Shader *line_shader_;
    GLint u_view_line_, u_perspective_line_;
    bool grid_visibility_;

    boost::signals2::connection signal_updated_camera_;
    boost::signals2::connection signal_updated_all_, signal_draw_all_;
  };
}

#endif // TORERO_GROUND_MANAGER_H
