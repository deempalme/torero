#ifndef TORERO_TYPES_H
#define TORERO_TYPES_H

#include "torero/definitions.h"

#include "algebraica/algebraica.h"
#include <boost/signals2.hpp>

namespace Toreo {
  class CameraElement;
  class FontLoader;
  class Ground;
  class GUILoader;
  class GUIMenu;
  class GUITitleBar;
  class Objects;
  class PointCloud;
  class Trajectory;
  class ThreeDimensionalModelLoader;
  class TextObject;
}

typedef int PCMid, MMid, MMelement, OMid, TMid, GMid, TXMid, FTid, GUIid, CMid;

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

#ifndef V_C_M
#define V_C_M
  namespace ImageEncoding {
    const int RED  = 1;
    const int RG   = 2;
    const int RGB  = 3;
    const int BGR  = 3;
    const int RGBA = 4;
    const int BGRA = 4;
  }

  struct ImageFile{
    // Use this in case each data's component is 16 bits long
    bool is_16bits      = false;
    // Use this in case the colors are inverted. Example: BGR instead og RGB
    bool is_inverted    = false;
    int width = 0;
    int height = 0;
    int components_size = 0;
    unsigned char *data = nullptr;
  };

  struct CameraInfo {
    int height;
    int width;
  };
#endif
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
  union PointXY{
    struct{
      T x;
      T y;
    };
    T data[2];
  };

  template<typename T>
  union PointLL{
    struct{
      T latitude;
      T longitude;
    };
    T data[2];
  };
#endif

#ifndef P_C_XYZ
#define P_C_XYZ
  union PointXYZ{
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
  union PointXYZI{
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
  union PointXYZRGB{
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

  union PointXYZRGBI{
    struct{
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
      float intensity;
    };
    float data[7];
  };

  union PointXYZRGBA{
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
  union PointRI{
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

#ifndef O_C_I
#define O_C_I
  union CoordinatesLLA{
    struct{
      float latitude;
      float longitude;
      float altitude;
    };
    float data[3];
  };

  union OrientationPYR{
    struct{
      float pitch;
      float yaw;
      float roll;
    };
    float data[3] = { 0.0f, 0.0f, 0.0f };
  };

  union OrientationXYZW{
    struct{
      float x;
      float y;
      float z;
      float w;
    };
    float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  };
#endif

#ifndef C_RGB_A
#define C_RGB_A
  union ColorRGB{
    struct{
      float red;
      float green;
      float blue;
    };
    float data[3];
  };

  union ColorRGBA{
    struct{
      float red;
      float green;
      float blue;
      float alpha;
    };
    float data[4];
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

  enum class ColorMode : unsigned int{
    INTENSITY = 0u,
    COLORMAP  = 1u,
    SOLID     = 2u,
    DATA      = 3u
  };

  enum class PointCloudType : unsigned int{
    XYZ      = 0u,
    XYZI     = 1u,
    XYZRGB   = 2u,
    XYZRGBI  = 3u,
    XYZRGBA  = 4u,
    XYZRGBAI = 5u
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

#ifndef O_M_A
#define O_M_A
  struct Arrow{
    // Arrow's orientation [quaternion]
    OrientationXYZW orientation;
    // Arrow's length in meters
    float length = 1.0f;
    // Display the arrow
    bool display = true;
  };
#endif

#ifndef O_M_D
#define O_M_D
  struct Object{
    // Object position (LOCATED at the object's center)
    PointXYZ position;
    // Object orientation (in radians)
    OrientationXYZW orientation;
    // Object's RGBA color
    ColorRGBA color = ColorRGBA{ 255.0f, 255.0f, 255.0f, 255.0f };
    // Arrow's properties:
    Arrow arrow;
    // Object's size in meters
    float width  = 1.0f;
    float length = 1.0f;
    float height = 1.0f;
    // Displays the object as a solid (filled faces)
    bool solid = false;
    // Line width in meters
    float line_width = 0.1f;
    std::string name;
  };
#endif

#ifndef O_M_P
#define O_M_P
  struct Polygon{
    // Object's center position
    PointXYZ position;
    // Object polygon shape, the origin is located at **position**
    std::vector<PointXYZ> points;
    // Object orientation (in radians)
    OrientationXYZW orientation;
    // Object's RGBA color
    ColorRGBA color = ColorRGBA{ 255.0f, 255.0f, 255.0f, 255.0f };
    // Arrow's properties:
    Arrow arrow;
    // Object's height in meters
    float height = 1.0f;
    // Displays the object as a solid (filled faces)
    bool solid = false;
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
    PointXYZ position;
    // Object's color (0 to 255)
    ColorRGBA color = ColorRGBA{ 255.0f, 255.0f, 255.0f, 255.0f };
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
  union GroundGrid{
    struct{
      // Probability if occupied (-1.0f if unknown)
      float probability;
      // Box height
      float height;
    };
    float data[2] = {-1.0f, 0.0f};
  };

  union Ground2D{
    struct{
      // RGBA color (range 0.0f to 255.0f)
      ColorRGBA color;
    };
    float data[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
  };

  union Ground3D{
    struct{
      // RGBA color (range 0.0f to 255.0f)
      ColorRGBA color;
      // cube's Height in meters
      float height;
    };
    float data[5] = { 255.0f, 255.0f, 255.0f, 255.0f, 0.0f };
  };

  union FreeGround2D{
    struct{
      // Position
      PointXYZ position;
      // RGBA color (range 0.0f to 255.0f)
      ColorRGBA color;
      // Dimensions
      float width;
      float length;
    };
    float data[9] = { 0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f };
  };

  union FreeGround3D{
    struct{
      // Position
      PointXYZ position;
      // RGBA color (range 0.0f to 255.0f)
      ColorRGBA color;
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
      ColorRGBA color;
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
      ColorRGBA color;
      // Dimensions
      float width;
      float length;
      float height;
    };
    float data[9] = { 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f, 1.0f };
  };

  struct OccupancyGrid {
    // The map width (in Y axis) [meters]
    float width  = 100.0f;
    // The map length (in X axis) [meters]
    float length = 100.0f;
    // Number of cells through Y axis
    unsigned int number_of_elements_through_width  = 100u;
    // Number of cells through X axis
    unsigned int number_of_elements_through_length = 100u;
    // Map origin
    PointXYZ origin;
    // Map orientation
    OrientationXYZW quaternion;
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

  struct GroundGridShader{
    Algebraica::vec3f position;
    float probability;
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
  struct Vehicle{
    CoordinatesLLA position = CoordinatesLLA{ 0.0f, 0.0f, 0.0f };
    PointXYZ position_xyz, velocity, acceleration;
    OrientationXYZW orientation;
    OrientationPYR euler;

    float steering_angle;

    float speed;
    float rpm;
    float fuel;

    float gas;
    float clutch;
    float brake;

    std::string gear;
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

  constexpr int DRAWING_ELEMENTS{10};

  enum class Message : unsigned int {
    ERROR      = 0u,
    WARNING    = 1u,
    ATTENTION  = 2u,
    NORMAL     = 3u
  };

  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- TEXT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  enum class Alignment : unsigned int {
    LEFT   = 0u,
    CENTER = 1u,
    RIGHT  = 2u,
    TOP    = 3u,
    BOTTOM = 4u
  };

  enum class TextWeight : unsigned int {
    LIGHTER = 0u,
    LIGHT   = 1u,
    NORMAL  = 2u,
    BOLD    = 3u,
    BOLDER  = 4u
  };

  struct TextSimple {
    // Position [meters]
    PointXYZ position;
    // Orientation angle (in the screen's plane) [radians]
    float angle = 0.0f;
    // Text
    std::string text;
  };

  struct TextColored {
    // Position [meters]
    PointXYZ position;
    // Orientation angle (in the screen's plane) [radians]
    float angle = 0.0f;
    // Color (range 0.0f -> 255.0f):
    ColorRGBA color = ColorRGBA{ 255.0f, 255.0f, 255.0f, 255.0f };
    // Text
    std::string text;
  };

  struct TextSimpleShader{
    // Origin:
    PointXYZ origin;
    // Position
    float p_x;
    float p_y;
    // Dimensions:
    float width;
    float height;
    // Texture coordinates
    //   First corner
    float u1;
    float v1;
    //   Last corner
    float u2;
    float v2;
    // Orientation angle (in the screen's plane) [radians]
    float angle;
  };

  struct TextColoredShader{
    // Origin:
    PointXYZ origin;
    // Position
//    float p_x;
//    float p_y;
//    // Dimensions:
//    float width;
//    float height;
//    // Color (range 0.0f -> 255.0f):
//    float r;
//    float g;
//    float b;
//    float a;
//    // Texture coordinates
//    //   First corner
//    float u1;
//    float v1;
//    //   Last corner
//    float u2;
//    float v2;
//    // Orientation angle (in the screen's plane) [radians]
//    float angle;
  };

  struct FontCharacter {
    unsigned int ascii = 0;
    union {
      struct {
        // Dimensions
        float width;
        float height;
        // Texture
        float u1;
        float v1;
        float u2;
        float v2;
      };
      float data[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
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
    std::size_t index;
    boost::signals2::connection connection;
  };

  struct TextIndex {
    std::size_t index = 0u;
    bool exist = true;
  };

  enum class Complexity : unsigned int {
    SIMPLE  = 0u,
    MEDIUM  = 1u,
    COMPLEX = 2u
  };

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- STREETS MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifndef S_M_S
#define S_M_S
  enum class StreetLineType : unsigned int {
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

  enum class StreetSidewalkType : unsigned int {
    RightSidewalk  = 0u,
    LeftSidewalk   = 1u,
    DoubleSidewalk = 2u,
    NoSidewalk     = 3u
  };

  struct StreetVertex {
    // Position
    union {
      PointXYZ position;
      CoordinatesLLA coordinates = CoordinatesLLA{ 0.0f, 0.0f, 0.0f };
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

  enum class SignalType : unsigned int {
    Semaphore,
    SpeedLimit,
    Stop,
    TrafficCone
  };

  struct Signal {
    // Position
    union {
      PointXYZ position;
      CoordinatesLLA coordinates = CoordinatesLLA{ 0.0f, 0.0f, 0.0f };
    };
    // Orientation (in radians)
    OrientationPYR euler;
    // Type of signal
    SignalType type = SignalType::SpeedLimit;
    // Visibility
    bool visible = true;
  };
#endif

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- GUI MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace GUIid {
    enum Id : unsigned int {
      MENU = 1u,
      TITLE_BAR = 2u
    };
  }

  namespace Mouse {
    enum Event : unsigned int {
      MOVE    = 0u,
      CLICK   = 1u,
      RELEASE = 2u
    };
  }

  namespace Button {
    enum State : unsigned int {
      NORMAL = 0u,
      HOVER  = 1u,
      CLICK  = 2u
    };

    struct Button {
      float top    = 0.0f;
      float left   = 0.0f;
      float u      = 0.0f;
      float v      = 0.0f;
      State state  = State::NORMAL;
      float width  = 0.0f;
      float height = 0.0f;
      float id     = 0u;
      std::string text = "";
    };
  }

  namespace Menu {
    enum ButtonType : unsigned int{
      ZOOM_IN        = 0u,
      ISOMETRIC_VIEW = 1u,
      TOP_VIEW       = 2u,
      ZOOM_OUT       = 3u,
      PEM_LOGO       = 4u
    };

    struct Object {
      Toreo::GUIMenu *object = nullptr;
      Alignment vertical     = Alignment::CENTER;
      Alignment horizontal   = Alignment::LEFT;
      float width            = MENU_WIDTH;
      float height           = MENU_HEIGHT;
      bool visibility        = false;
    };
  }

  namespace Title {
    enum ButtonType : unsigned int {
      TITLE    = 0u,
      CLOSE    = 1u,
      MAXIMIZE = 2u,
      MINIMIZE = 3u,
      OPTIONS  = 4u
    };
  }

  enum class Cursor : unsigned int {
    NORMAL    = 0u,
    HAND      = 1u,
    TEXT      = 2u,
    CROSSHAIR = 3u,
    MOVE      = 4u,
    HIDDEN    = 5u
  };

  struct ButtonShader{
    float top;
    float left;
    float u;
    float v;
    float width;
    float height;
    float id;
    float element;
  };

  struct PBR_GUIShader {
    float angle = 0.0f;
    // Object color
    ColorRGB color = ColorRGB{ 255.0f, 255.0f, 255.0f };
  };

  struct GUITextShader{
    // Position
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Dimension
    float width  = 1.0f;
    float height = 1.0f;
    // Texture coordinates
    // First corner
    float u1 = 0.0f;
    float v1 = 0.0f;
    // Second corner
    float u2 = 0.0f;
    float v2 = 0.0f;
    // Color [Range: 0 -> 1]
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
  };

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- CAMERA MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
  struct CameraElement {
    Toreo::CameraElement *camera = nullptr;
    std::string name;
    bool visibility;
    boost::signals2::connection connection;
  };

  enum class Dimensionality : unsigned int {
    THREE_DIMENSIONAL = 0u,
    TWO_DIMENSIONAL   = 1u
  };
}

#endif // TORERO_TYPES_H
