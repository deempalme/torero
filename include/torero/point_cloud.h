#ifndef TORERO_POINTCLOUD_H
#define TORERO_POINTCLOUD_H

#include "glad/glad.h"

#include "torero/buffer.h"
#include "torero/definitions.h"
#include "torero/shader.h"
#include "torero/types.h"
// Linear mathematic library
#include "algebraica/algebraica.h"
// Standard libraries
#include <vector>

namespace Toreo {
  class PointCloud
  {
  public:
    PointCloud(Shader *shader, const float point_size = 1.0f,
               const float maximum_intensity_value = 1.0f);

    void set_cloud(const std::vector<Visualizer::PointXYZ> *point_cloud);
    void set_cloud(const std::vector<Visualizer::PointXYZI> *point_cloud);
    void set_cloud(const std::vector<Visualizer::PointXYZRGB> *point_cloud);
    void set_cloud(const std::vector<Visualizer::PointXYZRGBI> *point_cloud);
    void set_cloud(const std::vector<Visualizer::PointXYZRGBA> *point_cloud);

    // sets the colormap
    // you must specify the size of the palette, the maximum readable size is 10 colors
    bool set_color_palette(const Visualizer::ColorRGB color);
    bool set_color_palette(const std::vector<Visualizer::ColorRGB> &color_palette);
    // sets the color mode:
    //    Visualizer::INTENSITY -> Colorize points multiplying *intensity* and first color
    //                             in *color_palete*
    //    Visualizer::SOLID     -> Colorize all points using only the first color in
    //                             *color_palete*
    //    Visualizer::COLORMAP  -> Colorize points interpolating the *color_palete* using
    //                             the point intensity
    //    Visualizer::DATA      -> Use this when you specify colors for each point
    void set_color_mode(const Visualizer::ColorMode color_mode = Visualizer::ColorMode::COLORMAP);
    void set_transformation_matrix(const Algebraica::mat4f *transformation_matrix);
    bool set_point_size(const float point_size = 1.0f);
    bool set_maximum_intensity_value(const float maximum_intensity_value = 1.0f);

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
    const GLsizei size();
    const GLvoid *data();

    Shader *shader_;
    Buffer buffer_;

    const void *point_cloud_;
    Visualizer::PointCloudType cloud_type_;

    const Algebraica::mat4f identity_matrix_;
    const Algebraica::mat4f *primary_model_;
    Algebraica::mat4f secondary_model_;
    float point_size_, maximum_intensity_value_;
    std::vector<Algebraica::vec3f> color_palette_;
    float color_size_, color_mode_;
    bool has_color_, has_alpha_, has_intensity_;
    GLsizei type_size_;
    GLsizei data_size_;

    GLint i_position_, i_intensity_, i_color_, i_alpha_;
    GLint u_primary_model_, u_secondary_model_, u_palette_, u_color_size_;
    GLint u_color_mode_, u_intensity_range_, u_has_alpha_;

    GLint offset_small_, offset_big_;
  };
}

#endif // TORERO_POINTCLOUD_H
