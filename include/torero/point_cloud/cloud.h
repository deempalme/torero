#ifndef TORERO_POINT_CLOUD_CLOUD_H
#define TORERO_POINT_CLOUD_CLOUD_H

#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/buffer.h"
#include "torero/gl/shader.h"
// Linear mathematic library
#include "algebraica/algebraica.h"
// Standard libraries
#include <vector>

namespace torero {
  namespace point_cloud {
    class Cloud
    {
    public:
      Cloud(torero::gl::Shader *shader, const float point_size = 1.0f,
            const float maximum_intensity_value = 1.0f);

      void set_cloud(const std::vector<torero::PointXYZ> *point_cloud);
      void set_cloud(const std::vector<torero::PointXYZI> *point_cloud);
      void set_cloud(const std::vector<torero::PointXYZRGB> *point_cloud);
      void set_cloud(const std::vector<torero::PointXYZRGBI> *point_cloud);
      void set_cloud(const std::vector<torero::PointXYZRGBA> *point_cloud);
      void set_cloud_buffer(const GLuint vertex_array_object_id, const GLsizei *cloud_size);

      // sets the colormap
      // you must specify the size of the palette, the maximum readable size is 10 colors
      bool set_color_palette(const torero::ColorRGB color);
      bool set_color_palette(const std::vector<torero::ColorRGB> &color_palette);
      // sets the color mode:
      //    torero::INTENSITY -> Colorize points multiplying *intensity* and first color
      //                             in *color_palete*
      //    torero::SOLID     -> Colorize all points using only the first color in
      //                             *color_palete*
      //    torero::COLORMAP  -> Colorize points interpolating the *color_palete* using
      //                             the point intensity
      //    torero::DATA      -> Use this when you specify colors for each point
      void set_color_mode(const torero::point_cloud::ColorMode color_mode
                          = torero::point_cloud::ColorMode::ColorMap);
      void set_coordinate_frame(const torero::Pose3D *coordinate_frame);
      void set_pose(torero::Pose3D *pose);
      bool set_point_size(const float point_size = 1.0f);
      bool set_maximum_intensity_value(const float maximum_intensity_value = 1.0f);

      void translate(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      void translate(const algebraica::vec3f &translation);
      void rotate(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
      void rotate(const algebraica::quaternionF &rotation);
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
      GLsizei size();
      const GLvoid *data();

      torero::gl::Shader *shader_;
      torero::gl::Buffer buffer_;
      GLuint buffer_id_;
      const GLsizei *buffer_size_;

      const void *point_cloud_;
      torero::point_cloud::Type cloud_type_;

      const torero::Pose3D initial_primary_model_;
      torero::Pose3D initial_secondary_model_;
      const torero::Pose3D *primary_model_;
      torero::Pose3D *secondary_model_;
      float point_size_, maximum_intensity_value_;
      std::vector<algebraica::vec3f> color_palette_;
      float color_size_, color_mode_;
      bool has_color_, has_alpha_, has_intensity_, has_buffer_;
      GLsizei type_size_;
      GLsizei data_size_;

      GLint u_primary_translation_, u_primary_orientation_, u_secondary_translation_;
      GLint u_secondary_orientation_, u_palette_, u_color_size_;
      GLint u_color_mode_, u_intensity_range_, u_has_alpha_;

      GLint offset_small_, offset_big_;
    };
  }
}
#endif // TORERO_POINT_CLOUD_CLOUD_H
