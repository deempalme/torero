#ifndef TORERO_TYPES_H
#define TORERO_TYPES_H

#include "algebraica/algebraica.h"
#include <boost/signals2.hpp>

namespace Toreo {
  class Ground;
  class Objects;
  class PointCloud;
  class Trajectory;
  class ThreeDimensionalModelLoader;
  class FontLoader;
  class TextObject;
}

typedef int PCMid, MMid, MMelement, OMid, TMid, GMid, TXMid, FTid;

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
    float A = 255.0f;
    bool metallize = false;
    float metallic = 0.0f;
    bool roughen = false;
    float roughness = 0.0f;
    bool emitting = false;
    bool foggy = false;
    bool pbr = true;
  };

  enum Models : unsigned int {
    EMPTY             = 0u,
    DB5_BODY          = 1u,
    DB5_WINDOWS       = 2u,
    DB5_ACCESSORIES   = 3u,
    DB5_INTERIOR      = 4u,
    CHASSIS           = 5u,
    AXIS              = 6u,
    STEERING          = 7u,
    TIRE              = 8u,
    COORDINATE_SYSTEM = 9u,
    DB5               = 10u,
    SHUTTLE_BODY      = 11u,
    SHUTTLE_WINDOWS   = 12u,
    SHUTTLE           = 13u
  };

  struct Model3D{
    Toreo::ThreeDimensionalModelLoader *model;
    std::vector<Model3DElement> elements;
    Models type = EMPTY;
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

#ifndef P_C_XY_LL
#define P_C_XY_LL
  template<typename T>
  union pointXY{
    struct{
      T x;
      T y;
    };
    T data[2];
  };

  template<typename T>
  union pointLL{
    struct{
      T latitude;
      T longitude;
    };
    T data[2];
  };
#endif

#ifndef P_C_XYZ
#define P_C_XYZ
  union pointXYZ{
    struct{
      float x;
      float y;
      float z;
    };
    float data[3];
  };
#endif

#ifndef P_C_XYZI
#define P_C_XYZI
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

#ifndef P_C_XYZRGB
#define P_C_XYZRGB
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
#endif

#ifndef P_C_RI
#define P_C_RI
  union pointRI{
    struct{
      // Range data [m] (Note: values < range_min or > range_max should be discarded)
      float range;
      // Intensity data [device-specific units].  If your device does not provide
      // intensities, please do not set the value (it should be 1.0f as default).
      float intensity;
    };
    float data[2] = { 0.0f, 1.0f };
  };

  union SensorRI{
    struct{
      // Start angle of the scan [rad]
      float angle_min;
      // End angle of the scan [rad]
      float angle_max;
      // Angular distance between measurements [rad]
      float angle_increment;

      // Time between measurements [seconds] - if your scanner is moving,
      // this will be used in interpolating position of 3d points
      float time_increment;

      // Time between scans [seconds]
      float scan_time;

      // Minimum range value [m]
      float range_min;
      // Maximum range value [m]
      float range_max;
    };
    float data[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };
  };
#endif

#ifndef P_C_DENSE
#define P_C_DENSE
  enum LaserFieldType : unsigned int {
    tI    = 0u,
    tX    = 1u,
    tXY   = 2u,
    tXYZ  = 3u,
    tRGB  = 4u,
    tRGBA = 5u,
    tGRAY = 6u,
    tMONO = 7u
  };

  struct PointFields{
    LaserFieldType type;
    unsigned int offset;
    unsigned short byte_size;
  };

  struct DensePoints{
    unsigned int stamp;
    unsigned int width;
    std::vector<PointFields> fields;
    bool is_bigendian = false;
    unsigned int point_step;
    unsigned char *data = nullptr;
  };
#endif

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
    // Object orientation (in radians)
    struct{
      float x = 0.0f;
      float y = 0.0f;
      float z = 0.0f;
      float w = 0.0f;
    } orientation, arrow_orientation;
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
    Algebraica::vec4f rotation;
    Algebraica::vec4f color;
    Algebraica::vec3f scale;
    float line_width;
  };

  struct ObjectShaderSolid{
    Algebraica::vec3f position;
    Algebraica::vec4f rotation;
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

#ifndef T_M_E
#define T_M_E
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
#endif

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
#ifndef G_M_E
#define G_M_E
  union Ground2D{
    struct{
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
    };
    float data[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
  };

  union Ground3D{
    struct{
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
      // cube's Height in meters
      float height;
    };
    float data[5] = { 255.0f, 255.0f, 255.0f, 255.0f, 0.0f };
  };

  union FreeGround2D{
    struct{
      // Position
      float x;
      float y;
      float z;
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
      // Dimensions
      float width;
      float length;
    };
    float data[9] = { 0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f };
  };

  union FreeGround3D{
    struct{
      // Position
      float x;
      float y;
      float z;
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
      // Dimensions
      float width;
      float length;
      float height;
    };
    float data[10] = { 0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f, 1.0f };
  };

  union FreePolarGround2D{
    struct{
      // Distance
      float distance;
      // Angle
      float angle;
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
      // Dimensions
      float width;
      float length;
    };
    float data[8] = { 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f };
  };

  union FreePolarGround3D{
    struct{
      // Distance
      float distance;
      // Angle
      float angle;
      // RGBA color (range 0.0f to 255.0f)
      float r;
      float g;
      float b;
      float alpha;
      // Dimensions
      float width;
      float length;
      float height;
    };
    float data[9] = { 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f, 1.0f };
  };
#endif

  struct Ground2DShader{
    Algebraica::vec3f position;
    Algebraica::vec4f color;
  };

  struct Ground3DShader{
    Algebraica::vec3f position;
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
  // -------------------------------- VEHICLE MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifndef V_M_S
#define V_M_S
  union OrientationPYR{
    struct{
      float pitch;
      float yaw;
      float roll;
    };
    float data[3];
  };

  union OrientationXYZW{
    struct{
      float x;
      float y;
      float z;
      float w;
    };
    float data[4];
  };

  union CoordinatesLLA{
    struct{
      float latitude;
      float longitude;
      float altitude;
    };
    float data[3];
  };

  union Vehicle{
    struct{
      CoordinatesLLA position;
      pointXYZ position_xyz, velocity, acceleration;
      OrientationXYZW orientation;

      float steering_angle;

      float rpm;
      int gear;
      float fuel;

      float gas;
      float clutch;
      float brake;
    };
    float data[23];
  };
#endif

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  enum Order : int {
    POINT_CLOUDS = 0,
    OBJECTS      = 1,
    SKYBOX       = 2,
    GROUND       = 3,
    STREETS      = 4,
    MODELS       = 5,
    TRAJECTORIES = 6,
    TEXT         = 7,
    CAMERA       = 8,
    GUI          = 9
  };
  enum Message : unsigned int {
    ERROR      = 0u,
    WARNING    = 1u,
    ATTENTION  = 2u,
    NORMAL     = 3u
  };

  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- TEXT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  struct TextSimple {
    // Position:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Text
    std::string text;
  };

  struct TextColored {
    // Position:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Color (range 0.0f -> 255.0f):
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
    // Text
    std::string text;
  };

  struct TextExtended {
    // Position:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Orientation (Euler angles in radians):
    float pitch = 0.0f;
    float yaw   = 0.0f;
    float roll  = 0.0f;
    // Color (range 0.0f -> 255.0f):
    float r     = 255.0f;
    float g     = 255.0f;
    float b     = 255.0f;
    float alpha = 255.0f;
    // Text
    std::string text;
  };

  struct FontCharacter {
    int ascii = 0;
    union {
      struct {
        // Dimensions
        float width;
        float height;
        // Texture
        float u;
        float v;
      };
      float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    };
    union {
      struct {
        // Offset
        float x;
        float y;
        // Next character position
        float next;
      };
      float offset[3] = { 0.0f, 0.0f, 0.0f };
    };
  };

  struct Text {
    Toreo::TextObject *text = nullptr;
    FTid font;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  enum Complexity : unsigned int {
    SIMPLE  = 0u,
    MEDIUM  = 1u,
    COMPLEX = 2u
  };

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- STREETS MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifndef S_M_S
#define S_M_S
  enum StreetLineType : unsigned int {
    // Solid lines:
    SingleSolidWhiteLine   = 0u,
    SingleSolidYellowLine  = 1u,
    DoubleSolidWhiteLine   = 2u,
    DoubleSolidYellowLine  = 3u,
    // Dashed lines:
    SingleDashedWhiteLine  = 4u,
    SingleDashedYellowLine = 5u,
    DoubleDashedWhiteLine  = 6u,
    DoubleDashedYellowLine = 7u,
    // No line
    NoLine                 = 8u
  };

  enum StreetSidewalkType : unsigned int {
    RightSidewalk  = 0u,
    LeftSidewalk   = 1u,
    DoubleSidewalk = 2u,
    NoSidewalk     = 3u
  };

  struct StreetVertex {
    // Position
    union {
      struct {
        float x;
        float y;
        float z;
      };
      struct {
        float latitude;
        float longitude;
        float altitude;
      };
      float position[3] = { 0.0f, 0.0f, 0.0f };
    };
    // Dimension
    float width = 1.0f;
    // Rotation angle in radians
    // This rotation affects only the longitudinal axis
    // (imaginary line from this vertex to the next one)
    float angle = 0.0f;
  };

  struct Street {
    // Street lines
    StreetLineType left  = StreetLineType::NoLine;
    StreetLineType right = StreetLineType::NoLine;
    // Sidewalks
    StreetSidewalkType sidewalk = StreetSidewalkType::NoSidewalk;
    // Street name
    std::string name;
    // vertices
    std::vector<StreetVertex> data;
  };

  enum SignalType : unsigned int {
    Semaphore,
    SpeedLimit,
    Stop,
    TrafficCone
  };

  struct Signal {
    // Position
    union {
      struct {
        float x;
        float y;
        float z;
      };
      struct {
        float latitude;
        float longitude;
        float altitude;
      };
      float position[3] = { 0.0f, 0.0f, 0.0f };
    };
    // Orientation (Euler angles in radians)
    float pitch = 0.0f;
    float yaw   = 0.0f;
    float roll  = 0.0f;
    // Type of signal
    SignalType type = SignalType::SpeedLimit;
    // Visibility
    bool visible = true;
  };
#endif
}

#endif // TORERO_TYPES_H
