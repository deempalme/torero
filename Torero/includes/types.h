#ifndef TYPES_H
#define TYPES_H

#include "Algebraica.h"
#include <boost/signals2.hpp>

class ThreeDimensionalModelLoader;
class PointCloud;

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
    // vertex texture coordinates
    Algebraica::vec2f texture;
    // vertex normals
    Algebraica::vec3f normal;
  };

  struct PointCloudElement{
    PointCloud *point_cloud = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  struct Model3D{
    ThreeDimensionalModelLoader *model;
    Algebraica::mat4f transformation_matrix;
  };

  struct ImageFile{
    int width = 0;
    int height = 0;
    int components_size = 0;
    unsigned char *data = nullptr;
  };

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
    GRAYSCALE = 0u,
    MONOCHROME = 1u,
    VARIABLE = 2u,
    NONE = 3u
  };
}

//#include "includes/three_dimensional_model_loader.h"

#endif // TYPES_H
