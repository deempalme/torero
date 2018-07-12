#ifndef TORERO_TEXT_MANAGER_H
#define TORERO_TEXT_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/font_loader.h"
#include "torero/shader.h"
#include "torero/text_object.h"
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

  class TextManager
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
    explicit TextManager(Core *core);
    ~TextManager();

    // ------------------------------------------------------------------------------------ //
    // --------------------------------- FONT MANAGEMENT ---------------------------------- //
    // ------------------------------------------------------------------------------------ //

    FTid add_font(const std::string font_distance_path, const std::string font_info_path);
    bool change_font(TXMid id, FTid new_font);
    bool delete_font(FTid font);
    const std::vector<torero::FontCharacter> *font_characters(const FTid font) const;
    void purge_fonts();
    bool use_font(const FTid font);

    // ------------------------------------------------------------------------------------ //
    // --------------------------------- TEXT MANAGEMENT ---------------------------------- //
    // ------------------------------------------------------------------------------------ //

    TXMid add_3D(FTid font,
                 const std::vector<torero::TextSimple> *data,
                 const std::string name,
                 const algebraica::mat4f *transformation_matrix = nullptr,
                 const bool visible = true);
    TXMid add_3D(FTid font,
                 const std::vector<torero::TextColored> *data,
                 const std::string name,
                 const algebraica::mat4f *transformation_matrix = nullptr,
                 const bool visible = true);
    TXMid add_2D(FTid font,
                 const std::vector<torero::TextSimple> *data,
                 const std::string name,
                 const bool visible = true);
    TXMid add_2D(FTid font,
                 const std::vector<torero::TextColored> *data,
                 const std::string name,
                 const bool visible = true);
    /*
     * ### Changing the point cloud data input: 3D and RGBA colors
     *
     * This function changes the data input for the three-dimensional **point cloud**
     * with *identification number* = `id`.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const std::vector<torero::PointXYZRGBA>*} point_cloud = new address to a 3D point
     * cloud's data.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
    bool change_input(TXMid id, const std::vector<torero::TextSimple> *data);
    bool change_input(TXMid id, const std::vector<torero::TextColored> *data);
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
    bool set_visibility(TXMid id, const bool visible = true);
    bool set_fog(TXMid id, const bool foggy = true);
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
    bool set_font(TXMid id, FTid font);
    bool set_font_color(TXMid id,
                   const float R = 255.0f,
                   const float G = 255.0f,
                   const float B = 255.0f,
                   const float A = 255.0f);
    bool set_font_size(TXMid id, const unsigned int size = 15u);
    bool set_font_weight(TXMid id, const torero::TextWeight
                         width = torero::TextWeight::Normal);
    bool set_edge_softness(TXMid id, const unsigned int softness = 1u);
    bool set_border_width(TXMid id, const unsigned int width = 0u);
    bool set_border_softness(TXMid id, const unsigned int softness = 0u);
    bool set_border_color(TXMid id,
                          const float R = 127.0f,
                          const float G = 127.0f,
                          const float B = 127.0f,
                          const float A = 255.0f);
    bool set_border_offset(TXMid id, const unsigned int x = 0u, const unsigned int y = 0u);
    /*
     * ### Setting the transformation matrix
     *
     * This function changes the transformation matrix (coordinate system's origin and
     * orientation) of the **point cloud** with *identification number* = `id`.
     *
     * **Arguments**
     * {PCMid} id = **id** of the point cloud you want to modify.
     * {const algebraica::mat4f*} transformation_matrix = Address to the new transformation matrix.
     *
     * **Returns**
     * {bool} Returns `false` if the point cloud with **id** was **not** found.
     *
     */
    bool set_transformation_matrix(TXMid id, const algebraica::mat4f *transformation_matrix);
    bool set_line_height(TXMid id, const float line_height = 1.2f);
    bool set_horizontal_alignment(TXMid id, const torero::Alignment
                                  alignment = torero::Alignment::Left);
    bool set_vertical_alignment(TXMid id, const torero::Alignment
                                alignment = torero::Alignment::Top);
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
    bool translate(TXMid id, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
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
    bool rotate(TXMid id, const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
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
    bool rotate_in_x(TXMid id, const float angle);
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
    bool rotate_in_y(TXMid id, const float angle);
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
    bool rotate_in_z(TXMid id, const float angle);
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
    bool update(TXMid id);
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
    bool draw(TXMid id);
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
    bool delete_text(TXMid id);
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
    bool connect(TXMid id, boost::signals2::signal<void ()> *signal);
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
    void updated_camera();
    void initialize();

    Core *core_;

    Shader *shader_;
    GLint u_projection_, u_view_, u_window_;
    const int *width_, *height_;

    std::vector<FontLoader*> fonts_;
    std::map<FTid, std::vector<torero::TextIndex>> text_indices_;
    std::vector<torero::Text> texts_;
    const algebraica::mat4f *transformation_matrix_;

    boost::signals2::connection signal_updated_camera_, signal_draw_all_;
    boost::signals2::connection signal_updated_all_;
  };
}
#endif // TORERO_TEXT_MANAGER_H
