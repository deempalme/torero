#ifndef TORERO_POINTCLOUD_H
#define TORERO_POINTCLOUD_H

#include "glad/glad.h"

#include "include/buffer.h"
#include "include/definitions.h"
#include "include/shader.h"
#include "include/types.h"

#include "Algebraica.h"

#include <vector>

namespace Toreo {
  class PointCloud
  {
  public:
    PointCloud(Shader *shader_program, const std::vector<Visualizer::pointXYZ> *point_cloud,
               const Algebraica::vec3f rgb_color, const float point_size = 1.0f,
               const float maximum_intensity_value = 1.0f);

    PointCloud(Shader *shader_program, const std::vector<Visualizer::pointXYZI> *point_cloud,
               const Visualizer::ColorMode color_mode = Visualizer::VARIABLE,
               const float point_size = 1.0f, const float maximum_intensity_value = 1.0f);

    PointCloud(Shader *shader_program, const std::vector<Visualizer::pointXYZRGB> *point_cloud,
               const float point_size = 1.0f, const float maximum_intensity_value = 1.0f);

    PointCloud(Shader *shader_program, const std::vector<Visualizer::pointXYZRGBA> *point_cloud,
               const float point_size = 1.0f, const float maximum_intensity_value = 1.0f);

    void change_input(const std::vector<Visualizer::pointXYZ> *point_cloud);
    void change_input(const std::vector<Visualizer::pointXYZI> *point_cloud);
    void change_input(const std::vector<Visualizer::pointXYZRGB> *point_cloud);
    void change_input(const std::vector<Visualizer::pointXYZRGBA> *point_cloud);

    // sets the colormap
    // you must specify the size of the palette, the maximum readable size is 10 colors
    void set_colormap(const Algebraica::vec3f *colors, const unsigned int quantity = 1u);
    // sets the color mode:
    //    Visualizer::GRAYSCALE   -> draws all the points in grayscale
    //    Visualizer::MONOCHROME  -> draws all the points in one color (the first color in the palette)
    //    Visualizer::VARIABLE    -> draws all the points within the colormap relative to their intensity
    //    Visualizer::NONE        -> use this when using RGB or RGBA data
    void set_color_mode(const Visualizer::ColorMode color_mode = Visualizer::VARIABLE);
    void set_transformation_matrix(const Algebraica::mat4f *transformation_matrix);
    void set_point_size(const float point_size = 1.0f);
    void set_maximum_intensity_value(const float maximum_intensity_value = 1.0f);

    void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    void translate(const Algebraica::vec3f translation);
    void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    void rotate(const Algebraica::vec3f rotation);
    void rotate_in_x(const float angle);
    void rotate_in_y(const float angle);
    void rotate_in_z(const float angle);

    // copies the point cloud's information into an openGL buffer
    // returns false if the GL_SHADER_PROGRAM was not created or compiled properly
    bool update();
    // draws the point cloud into the screen
    // returns false if the GL_SHADER_PROGRAM was not created or compiled properly
    bool draw();

  private:
    void initialize();
    void restart();

    Shader *shader_;
    Buffer buffer_;

    const std::vector<Visualizer::pointXYZ>     *point_cloud_xyz_;
    const std::vector<Visualizer::pointXYZI>    *point_cloud_xyzi_;
    const std::vector<Visualizer::pointXYZRGB>  *point_cloud_rgb_;
    const std::vector<Visualizer::pointXYZRGBA> *point_cloud_rgba_;

    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_, identity_matrix_;
    Visualizer::ColorMode color_mode_;
    float point_size_, maximum_intensity_value_;
    Algebraica::vec3f color_palette_[10];
    unsigned int type_, color_size_;
    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_position_, i_intensity_, i_color_, i_alpha_;
    GLint u_primary_model_, u_secondary_model_, u_palette_, u_color_size_;
    GLint u_color_mode_, u_intensity_range_, u_has_alpha_;

    GLint offset_, offset_x2_;
  };
  }

#endif // TORERO_POINTCLOUD_H
