#ifndef TORERO_TYPES_H
#define TORERO_TYPES_H

#include "Algebraica.h"
#include <boost/signals2.hpp>

namespace Toreo {
  class Ground;
  class Objects;
  class PointCloud;
  class Trajectory;
  class ThreeDimensionalModelLoader;
}

namespace Visualizer {
  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- SHADER DATA ------------------------------------ //
  // ------------------------------------------------------------------------------------ //
  struct ComplexShaderData{
    // vertex position
    Algebraica::vec3f position;
    // vertex normals
    Algebraica::vec3f normal;
    // vertex tangents
    Algebraica::vec3f tangent;
    // vertex tangents
    Algebraica::vec3f bitangent;
    // vertex texture coordinates
    Algebraica::vec2f texture;
  };

  struct SimpleShaderData{
    // vertex position
    Algebraica::vec3f position;
    // vertex normals
    Algebraica::vec3f normal;
    // vertex texture coordinates
    Algebraica::vec2f texture;
  };

  struct ImageFile{
    int width = 0;
    int height = 0;
    int components_size = 0;
    unsigned char *data = nullptr;
  };
  // ------------------------------------------------------------------------------------ //
  // ------------------------------- 3D MODEL MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
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
  // ------------------------------------------------------------------------------------ //
  // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  struct PointCloudElement{
    Toreo::PointCloud *point_cloud = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
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
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- OBJECT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  enum Shape : unsigned int{
    BOX      = 0u,
    CYLINDER = 1u,
    CIRCLE   = 2u,
    SQUARE   = 3u
  };

#ifndef O_M_D
#define O_M_D
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
    std::string name;
  };
#endif

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
  // ------------------------------------------------------------------------------------ //
  // ------------------------------ TRAJECTORY MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  enum LineType : unsigned int {
    SOLID          = 0u,
    DOTTED         = 1u,
    DASHED         = 2u,
    ARROWED        = 3u
  };

  struct TrajectoryVertex{
    // Object position
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Object's color (0 to 255)
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
    // Line width in meters
    float line_width = 1.0f;
    // Rotation angle in radians
    // This rotation affects only the longitudinal axis
    // (imaginary line from this vertex to the next one)
    float angle = 0.0f;
  };

  typedef std::vector<TrajectoryVertex> Trajectory;

  struct TrajectoryElement{
    Toreo::Trajectory *trajectory = nullptr;
    Visualizer::LineType type;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  struct TrajectoryShader{
    Algebraica::vec3f position;
    Algebraica::vec4f color;
    float line_width;
    float distance;
    float angle;
  };
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- GROUND MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  struct Ground2D{
    // RGBA color (range 0.0f to 255.0f)
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
  };

  struct Ground3D{
    // RGBA color (range 0.0f to 255.0f)
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
    // cube's Height in meters
    float height = 0.0f;
  };

  struct Ground2DShader{
    Algebraica::vec2f position;
    Algebraica::vec4f color;
  };

  struct Ground3DShader{
    Algebraica::vec2f position;
    Algebraica::vec4f color;
    float height;
  };

  struct GroundElement{
    Toreo::Ground *ground = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  enum Order : int {
    POINT_CLOUDS = 0,
    OBJECTS      = 1,
    GROUND       = 2,
    STREETS      = 3,
    MODELS       = 4,
    TRAJECTORIES = 5,
    TEXT         = 6,
    CAMERA       = 7,
    GUI          = 8
  };
  enum Message : unsigned int {
    ERROR      = 0u,
    WARNING    = 1u,
    ATTENTION  = 2u,
    NORMAL     = 3u
  };
}

typedef int PCMid, MMid, MMelement, OMid, TMid, GMid;

#endif // TORERO_TYPES_H
