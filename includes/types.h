#ifndef TORERO_TYPES_H
#define TORERO_TYPES_H

#include "libs/algebraica/Algebraica.h"
#include <boost/signals2.hpp>

namespace Toreo {
  class ThreeDimensionalModelLoader;
  class PointCloud;
  class Objects;
}

namespace Visualizer {

  struct ComplexShaderData{
    // vertex position
    float x  = 0;
    float y  = 0;
    float z  = 0;
    // vertex normals
    float nx = 0;
    float ny = 0;
    float nz = 0;
    // vertex texture coordinates
    float ux = 0;
    float uy = 0;
    // vertex tangential normals
    float tx = 0;
    float ty = 0;
    float tz = 0;
    // vertex bitangential normals
    float bx = 0;
    float by = 0;
    float bz = 0;
  };

  struct SimpleShaderData{
    // vertex position
    Algebraica::vec3f position;
    // vertex normals
    Algebraica::vec3f normal;
    // vertex texture coordinates
    Algebraica::vec2f texture;
  };

  struct PointCloudElement{
    Toreo::PointCloud *point_cloud = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  struct Model3DElement{
    const Algebraica::mat4f *main = nullptr;
    Algebraica::mat4f secondary;
    bool visibility = true;
    bool colorize = false;
    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;
    float A = 1.0f;
    bool metallize = false;
    float metallic = 0.0f;
    bool roughen = false;
    float roughness = 0.0f;
    bool foggy = false;
    bool pbr = true;
  };

  struct Model3D{
    Toreo::ThreeDimensionalModelLoader *model;
    std::vector<Model3DElement> elements;
  };

  struct ImageFile{
    int width = 0;
    int height = 0;
    int components_size = 0;
    unsigned char *data = nullptr;
  };

#ifndef C_C_S
#define C_C_S
  union pointXY{
    struct{
      double x;
      double y;
    };
    double data[2];
  };
  union pointLL{
    struct{
      double latitude;
      double longitude;
    };
    double data[2];
  };
#endif

  union pointXYZ{
    struct{
      float x;
      float y;
      float z;
    };
    float data[3];
  };

#ifndef P_C_E
#define P_C_E
  union pointXYZI{
    struct{
      float x;
      float y;
      float z;
      float intensity;
    };
    float data[4];
  };
#endif

  union pointXYZRGB{
    struct{
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
    };
    float data[6];
  };

  union pointXYZRGBA{
    struct{
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
      float a;
    };
    float data[7];
  };

  enum ColorMode : unsigned int{
    GRAYSCALE  = 0u,
    MONOCHROME = 1u,
    VARIABLE   = 2u,
    NONE       = 3u
  };

  enum Shape : unsigned int{
    BOX      = 0u,
    CYLINDER = 1u,
    CIRCLE   = 2u,
    SQUARE   = 3u
  };

  struct Object{
    // Object position (LOCATED at the object's center)
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Object direction (in radians)
    float pitch = 0.0f;
    float yaw   = 0.0f;
    float roll  = 0.0f;
    // Object's size in meters
    float width  = 1.0f;
    float length = 1.0f;
    float height = 1.0f;
    // Object's color (0 to 255)
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
    // Arrow's properties:
    // Arrow's direction (in readians)
    float arrow_pitch = 0.0f;
    float arrow_yaw   = 0.0f;
    float arrow_roll  = 0.0f;
    // Arrow's length in meters
    float arrow_length = 1.0f;
    // Displays the arrow
    bool arrow = true;
    // Displays the object as a solid (filled faces)
    bool solid = false;
    // Line width in meters
    float line_width = 0.1f;
  };

  struct ObjectElement{
    Toreo::Objects *object = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  struct ObjectShaderHollow{
    Algebraica::vec3f position;
    Algebraica::vec3f rotation;
    Algebraica::vec4f color;
    Algebraica::vec3f scale;
    float line_width;
  };

  struct ObjectShaderSolid{
    Algebraica::vec3f position;
    Algebraica::vec3f rotation;
    Algebraica::vec4f color;
    Algebraica::vec3f scale;
  };

  struct ObjectBuffer{
    float x, y, z;
    float n_x, n_y, n_z;
    float u, v;
    float w, h, l;
  };
}

typedef int PCMid, MMid, MMelement, OMid;

#endif // TORERO_TYPES_H
