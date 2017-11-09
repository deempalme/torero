#ifndef POINT_CLOUD_MANAGER_H
#define POINT_CLOUD_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "includes/definitions.h"
#include "includes/point_cloud.h"
#include "includes/shader.h"
#include "includes/types.h"

#include "Algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>

class Console;

class PointCloudManager
{
public:
  explicit PointCloudManager(Console *console);
  ~PointCloudManager();

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  // This will add a new point cloud with values Visualizer::pointXYZ, you must also define
  // a RGB color. It will return the point cloud's ID, it will be useful if you want to
  // modify properties or values of the created point cloud.
  unsigned int point_cloud_add(const std::vector<Visualizer::pointXYZ> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix = nullptr,
                               const float color_red = 255.0f,
                               const float color_green = 255.0f,
                               const float color_blue = 255.0f,
                               const bool visible = true,
                               const float point_size = 1.0f,
                               const float maximum_intensity_value = 1.0f);
  // This will add a new point cloud with values Visualizer::pointXYZI, you must define a color
  // mode, the values could be:
  //  Visualizer::GRAYSCALE  -> draws all the points in grayscale
  //  Visualizer::MONOCHROME -> draws all the points in one color (the first color in the palette)
  //  Visualizer::VARIABLE   -> draws all the points within the colormap relative to their intensity
  //  Visualizer::NONE       -> use this when using RGB or RGBA data
  // It will return the point cloud's ID, it will be useful if you want to modify properties
  // or values of the created point cloud.
  unsigned int point_cloud_add(const std::vector<Visualizer::pointXYZI> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix = nullptr,
                               const Visualizer::ColorMode color_mode = Visualizer::VARIABLE,
                               const bool visible = true,
                               const float point_size = 1.0f,
                               const float maximum_intensity_value = 1.0f);
  // This will add a new point cloud with values Visualizer::pointXYZRGB. Each point must
  // include its respective RGB color.
  // It will return the point cloud's ID, it will be useful if you want to modify properties
  // or values of the created point cloud.
  unsigned int point_cloud_add(const std::vector<Visualizer::pointXYZRGB> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix = nullptr,
                               const bool visible = true,
                               const float point_size = 1.0f,
                               const float maximum_intensity_value = 1.0f);
  // This will add a new point cloud with values Visualizer::pointXYZRGBA. Each point must
  // include its respective RGB and Alpha color.
  // It will return the point cloud's ID, it will be useful if you want to modify properties
  // or values of the created point cloud.
  unsigned int point_cloud_add(const std::vector<Visualizer::pointXYZRGBA> *point_cloud,
                               const std::string name,
                               const Algebraica::mat4f *transformation_matrix = nullptr,
                               const bool visible = true,
                               const float point_size = 1.0f,
                               const float maximum_intensity_value = 1.0f);
  // This will change the input data for the Point cloud with ID = id
  bool point_cloud_change_input(const unsigned int id,
                                const std::vector<Visualizer::pointXYZ> *point_cloud);
  bool point_cloud_change_input(const unsigned int id,
                                const std::vector<Visualizer::pointXYZI> *point_cloud);
  bool point_cloud_change_input(const unsigned int id,
                                const std::vector<Visualizer::pointXYZRGB> *point_cloud);
  bool point_cloud_change_input(const unsigned int id,
                                const std::vector<Visualizer::pointXYZRGBA> *point_cloud);
  // Changes the visibility of the point cloud,
  // returns false if point cloud with ID = id was not found.
  bool point_cloud_set_visibility(const unsigned int id, const bool visible = true);
  // Sets the colormap palette. You must specify the size of the palette, the maximum
  // readable size is 10 colors per colormap. You can only specify one for each Cloud.
  bool point_cloud_set_colormap(const unsigned int id, const Algebraica::vec3f *colors,
                                const unsigned int quantity = 1u);
  // Sets the color mode:
  //  Visualizer::GRAYSCALE  -> draws all the points in grayscale
  //  Visualizer::MONOCHROME -> draws all the points in one color (the first color in the palette)
  //  Visualizer::VARIABLE   -> draws all the points within the colormap relative to their intensity
  //  Visualizer::NONE       -> use this when using RGB or RGBA data
  bool point_cloud_set_color_mode(const unsigned int id,
                                  const Visualizer::ColorMode color_mode = Visualizer::VARIABLE);
  // Sets the transformation matrix for the Point cloud with ID = id.
  bool point_cloud_set_transformation_matrix(const unsigned int id,
                                             const Algebraica::mat4f *transformation_matrix);
  // Translates the point cloud using X, Y and Z values
  bool point_cloud_translate(const unsigned int id,
                             const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
  // Rotates the point cloud using Pitch, Yaw and Roll values
  bool point_cloud_rotate(const unsigned int id, const float pitch = 0.0f,
                          const float yaw = 0.0f, const float roll = 0.0f);
  // Performs a rotation in the point cloud around its X axis
  bool point_cloud_rotate_in_x(const unsigned int id, const float x);
  // Performs a rotation in the point cloud around its Y axis
  bool point_cloud_rotate_in_y(const unsigned int id, const float y);
  // Performs a rotation in the point cloud around its Z axis
  bool point_cloud_rotate_in_z(const unsigned int id, const float z);
  // Updates the data of the point cloud.
  // Returns false if the point cloud was not proerly created.
  bool point_cloud_update(const unsigned int id);
  // Updates all the point cloud's data
  void point_cloud_update_all();
  // Draws the point cloud into the screen.
  // Returns false if the point cloud was not proerly created.
  bool point_cloud_draw(const unsigned int id);
  // Draws all the point clouds
  void point_cloud_draw_all();
  // Deletes the point cloud width ID = id,
  // Returns false if point cloud already deleted.
  bool point_cloud_delete(const unsigned int id);
  // This will delete all point clouds and free memory, if you create and delete point clouds
  // dinamically call this method from time to time to increase the program performance,
  // all the previous point clouds' IDs will not work anymore
  void point_cloud_purge();
  // Connects the point cloud with ID = id to a signal for updating and/or drawing the data.
  // Returns false if te point clouds does not exists.
  // If you delete the point cloud with ID = id or purge all, you will need to call this
  // function again to a new id or new point cloud.
  bool point_cloud_connect(boost::signals2::signal<void ()> *signal,
                           const unsigned int id);
private:
  void updated_camera();

  Console *console_;

  Shader *point_cloud_shader_;
  GLint pc_u_pv_;
  std::vector<Visualizer::PointCloudElement> point_clouds_;

  boost::signals2::connection updated_camera_signal_, updated_screen_signal_;
};

#endif // POINT_CLOUD_MANAGER_H
