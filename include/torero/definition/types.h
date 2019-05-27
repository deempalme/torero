#ifndef TORERO_TYPE_DEFINITIONS_H
#define TORERO_TYPE_DEFINITIONS_H

#include "torero/definition/constant_expressions.h"
#include "torero/definition/enumerators.h"

#include "algebraica/algebraica.h"
#include <boost/signals2.hpp>

class SDL_Surface;

namespace torero {
  typedef std::size_t PCMid, MMid, MMelement, OMid, TMid;
  typedef std::size_t GMid, TXMid, FTid, GUIid, CMid, CVMid;

  namespace camera {
    class Camera;
    class View;
  }
  namespace covariance { class Covariances; }
  namespace ground { class Ground; }
  namespace gui {
    class Loader;
    class Menu;
    class TitleBar;
  }
  namespace model{ class Loader; }
  namespace object { class Objects; }
  namespace point_cloud { class Cloud; }
  namespace text {
    class FontLoader;
    class TextObject;
  }
  namespace trajectory { class Trajectories; }

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- SHADER DATA ------------------------------------ //
  // ------------------------------------------------------------------------------------ //
  typedef struct ComplexShaderData {
    // vertex position
    algebraica::vec3f position;
    // vertex normals
    algebraica::vec3f normal;
    // vertex texture coordinates
    algebraica::vec2f texture;
    // vertex tangents
    algebraica::vec3f tangent;
  } ComplexShaderData;

  typedef struct SimpleShaderData {
    // vertex position
    algebraica::vec3f position;
    // vertex normals
    algebraica::vec3f normal;
    // vertex texture coordinates
    algebraica::vec2f texture;
  } SimpleShaderData;

#ifndef V_C_M_T
#define V_C_M_T
  namespace image {
    typedef struct File {
      // Use this in case the colors are inverted. Example: BGR instead og RGB
      bool is_inverted    = false;
      int width           = 0;
      int height          = 0;
      int components_size = 0;
      unsigned char *data = nullptr;
    } File;

    typedef struct FileFloat {
      bool is_inverted = false;
      int width        = 0;
      int height       = 0;
      int components   = 0;
      float *data      = nullptr;
    } FileFloat;

    typedef struct Surface {
      SDL_Surface *surface       = nullptr;
      torero::image::File *image = nullptr;
    } Surface;
  }

  namespace camera {
    typedef struct Info {
      int height;
      int width;
    } Info;
  }
#endif
  // ------------------------------------------------------------------------------------ //
  // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace point_cloud {
    typedef struct Element {
      torero::point_cloud::Cloud *point_cloud = nullptr;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Element;
  }

#ifndef P_C_XY_LL
#define P_C_XY_LL
  template<typename T>
  union PointXY{
    struct XY{
      T x;
      T y;
    } point;
    T data[2];
  };

  template<typename T>
  union PointLL{
    struct LL{
      T latitude;
      T longitude;
    } coordinates;
    T data[2];
  };
#endif

#ifndef P_C_XYZ
#define P_C_XYZ
  union PointXYZ{
    struct XYZ{
      float x;
      float y;
      float z;
    } point;
    float data[3] = { 0.0f, 0.0f, 0.0f };
  };
#endif

#ifndef P_C_XYZI
#define P_C_XYZI
  union PointXYZI{
    struct XYZI{
      float x;
      float y;
      float z;
      float intensity;
    } point;
    float data[4];
  };
#endif

#ifndef P_C_XYZIAD
#define P_C_XYZIAD
  union PointXYZIAD{
    struct XYZIAD{
      float x;
      float y;
      float z;
      float intensity;
      float angle;
      float distance;
    } point;
    float data[6];
  };
#endif

#ifndef P_C_VHDI
#define P_C_VHDI
  union pointVHDI {
    struct VHDI {
      float vertical_angle = 0.0f;
      float horizontal_angle = 0.0f;
      float distance = 0.0f;
      float intensity = 0.0f;
    } point;
    float data[4];
  };
#endif

#ifndef P_C_XYZRGB
#define P_C_XYZRGB
  union PointXYZRGB{
    struct XYZRGB{
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
    } point;
    float data[6];
  };

  union PointXYZRGBI{
    struct XYZRGBI{
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
      float intensity;
    } point;
    float data[7];
  };

  union PointXYZRGBA{
    struct XYZRGBA {
      float x;
      float y;
      float z;
      float r;
      float g;
      float b;
      float a;
    } point;
    float data[7];
  };
#endif

#ifndef P_C_RI
#define P_C_RI
  union PointRI{
    struct RI{
      // Range data [m] (Note: values < range_min or > range_max should be discarded)
      float range;
      // Intensity data [device-specific units].  If your device does not provide
      // intensities, please do not set the value (it should be 1.0f as default).
      float intensity;
      // Point's ID
      float id;
    } point;
    float data[3] = { 0.0f, 1.0f, 0.0f };
  };

  union SensorRI{
    struct Info{
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
      // Maximum size of the segmentations at one meter
      float segment_max_size;
    } properties;
    float data[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
  };
#endif

#ifndef O_C_I
#define O_C_I
  union CoordinatesLLA{
    struct LLA{
      float latitude;
      float longitude;
      float altitude;
    } coordinates;
    float data[3] = { 0.0f, 0.0f, 0.0f };
  };

  union OrientationPYR{
    struct PYR {
      float pitch;
      float yaw;
      float roll;
    } angles;
    float data[3] = { 0.0f, 0.0f, 0.0f };
  };

  union OrientationXYZW{
    struct XYZW{
      float x;
      float y;
      float z;
      float w;
    } axes;
    float data[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  };
#endif

#ifndef T_POSE_D
#define T_POSE_D
  typedef struct Pose3D {
    PointXYZ position;
    OrientationXYZW orientation;
  } Pose3D;
#endif

#ifndef C_RGB_A
#define C_RGB_A
  union ColorRGB {
    struct RGB {
      float red;
      float green;
      float blue;
    } rgb;
    float data[3];
  };

  union ColorRGBA{
    struct RGBA{
      float red;
      float green;
      float blue;
      float alpha;
    } rgba;
    float data[4];
  };
#endif

#ifndef P_C_DENSE_T
#define P_C_DENSE_T
  typedef struct PointFields{
    point_cloud::LaserFieldType type;
    unsigned int offset;
    unsigned short byte_size;
  } PointFields;

  typedef struct DensePoints{
    unsigned int stamp;
    unsigned int width;
    std::vector<PointFields> fields;
    bool is_bigendian = false;
    unsigned int point_step;
    unsigned char *data = nullptr;
  } DensePoints;
#endif

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- OBJECT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace object {
#ifndef O_M_A
#define O_M_A
    typedef struct Arrow{
      // Arrow's orientation [quaternion]
      OrientationXYZW orientation;
      // Arrow's length in meters
      float length = 1.0f;
      // Display the arrow
      bool display = true;
    } Arrow;
#endif

#ifndef O_M_D
#define O_M_D
    typedef struct Object{
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
    } Object;
#endif

#ifndef O_M_P
#define O_M_P
    typedef struct Polygon{
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
    } Polygon;
#endif

    typedef struct Element{
      torero::object::Objects *object = nullptr;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Element;

    typedef struct ShaderHollow{
      algebraica::vec3f position;
      algebraica::vec4f rotation;
      algebraica::vec4f color;
      algebraica::vec3f scale;
      float line_width;
    } ShaderHollow;

    typedef struct ShaderSolid{
      algebraica::vec3f position;
      algebraica::vec4f rotation;
      algebraica::vec4f color;
      algebraica::vec3f scale;
    } ShaderSolid;

    typedef struct Buffer{
      float x, y, z;
      float n_x, n_y, n_z;
      float u, v;
      float w, h, l;
    } Buffer;
  }
  // ------------------------------------------------------------------------------------ //
  // ------------------------------ TRAJECTORY MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace trajectory {
#ifndef T_M_E
#define T_M_E
    typedef struct Vertex{
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
    } Vertex;

    typedef std::vector<torero::trajectory::Vertex> Trajectory;
#endif

    typedef struct Element{
      torero::trajectory::Trajectories *trajectory = nullptr;
      LineType type;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Element;

    typedef struct Shader{
      algebraica::vec3f position;
      algebraica::vec4f color;
      float line_width;
      float distance;
      float angle;
    } Shader;
  }
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- GROUND MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace ground {
#ifndef G_M_E
#define G_M_E
    union Grid{
      struct PH {
        // Probability if occupied (-1.0f if unknown)
        float probability;
        // Box height
        float height;
      } ground;
      float data[2] = {-1.0f, 0.0f};
    };

    union Ground2D{
      // RGBA color (range 0.0f to 255.0f)
      ColorRGBA color;
      float data[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
    };

    union Ground3D{
      struct RGBAH {
        // RGBA color (range 0.0f to 255.0f)
        ColorRGBA color;
        // cube's Height in meters
        float height;
      } ground;
      float data[5] = { 255.0f, 255.0f, 255.0f, 255.0f, 0.0f };
    };

    union Free2D{
      struct XYZRGBAWL {
        // Position
        PointXYZ position;
        // RGBA color (range 0.0f to 255.0f)
        ColorRGBA color;
        // Dimensions
        float width;
        float length;
      } ground;
      float data[9] = { 0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f };
    };

    union Free3D{
      struct XYZRGBAWLH {
        // Position
        PointXYZ position;
        // RGBA color (range 0.0f to 255.0f)
        ColorRGBA color;
        // Dimensions
        float width;
        float length;
        float height;
      } ground;
      float data[10] = { 0.0f, 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f, 1.0f };
    };

    union FreePolar2D{
      struct DARGBAWL {
        // Distance
        float distance;
        // Angle
        float angle;
        // RGBA color (range 0.0f to 255.0f)
        ColorRGBA color;
        // Dimensions
        float width;
        float length;
      } ground;
      float data[8] = { 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f };
    };

    union FreePolar3D{
      struct DARGBAWLH {
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
      } ground;
      float data[9] = { 0.0f, 0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 1.0f, 1.0f, 1.0f };
    };

    typedef struct OccupancyGrid {
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
    } OccupancyGrid;
#endif

    typedef struct Shader2D {
      algebraica::vec3f position;
      algebraica::vec4f color;
    } Shader2D;

    typedef struct Shader3D {
      algebraica::vec3f position;
      algebraica::vec4f color;
      float height;
    } Shader3D;

    typedef struct GridShader{
      algebraica::vec3f position;
      float probability;
    } GridShader;

    typedef struct Element{
      torero::ground::Ground *ground = nullptr;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Element;
  }
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- VEHICLE MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace vehicle {
#ifndef V_M_S
#define V_M_S
    typedef struct Vehicle{
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
    } Vehicle;
#endif
  }
  // ------------------------------------------------------------------------------------ //
  // ------------------------------- 3D MODEL MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace model {
    typedef struct Element3D{
      const torero::Pose3D *main = nullptr;
      torero::Pose3D secondary = {};
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
    } Element3D;

    typedef struct Model3D{
      torero::model::Loader *model;
      std::vector<model::Element3D> elements;
      model::Models type = torero::model::Empty;
    } Model3D;
  }
  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- TEXT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace text {
    typedef struct Simple {
      // Position [meters]
      PointXYZ position;
      // Orientation angle (in the screen's plane) [radians]
      float angle = 0.0f;
      // Text
      std::string text;
    } Simple;

    typedef struct Colored {
      // Position [meters]
      PointXYZ position;
      // Orientation angle (in the screen's plane) [radians]
      float angle = 0.0f;
      // Color (range 0.0f -> 255.0f):
      ColorRGBA color = ColorRGBA{ 255.0f, 255.0f, 255.0f, 255.0f };
      // Text
      std::string text;
    } Colored;

    typedef struct SimpleShader{
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
    } SimpleShader;

    typedef struct ColoredShader{
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
    } ColoredShader;

    typedef struct FontCharacter {
      unsigned int ascii = 0;
      union Texture {
        struct WHUV {
          // Dimensions
          float width;
          float height;
          // Texture
          float u1;
          float v1;
          float u2;
          float v2;
        } map;
        float data[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
      } texture;
      union Position {
        struct XYN {
          // Offset
          float x;
          float y;
          // Next character position
          float next;
        } offset;
        float data[3] = { 0.0f, 0.0f, 0.0f };
      } position;
    } FontCharacter;

    typedef struct Text {
      torero::text::TextObject *text = nullptr;
      FTid font;
      std::string name;
      bool visibility;
      std::size_t index;
      boost::signals2::connection connection;
    } Text;

    typedef struct Index {
      std::size_t index = 0u;
      bool exist = true;
    } Index;
  }

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- STREETS MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifndef S_M_S_T
#define S_M_S_T
  typedef struct StreetVertex {
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
  } StreetVertex;

  typedef struct Street {
    // Street lines
    StreetLineType left  = StreetLineType::NoLine;
    StreetLineType right = StreetLineType::NoLine;
    // Sidewalks
    StreetSidewalkType sidewalk = StreetSidewalkType::NoSidewalk;
    // Street name
    std::string name;
    // vertices
    std::vector<StreetVertex> data;
  } Street;

  typedef struct Signal {
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
  } Signal;
#endif

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- GUI MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace gui {
    namespace button {
      typedef struct Button {
        float top    = 0.0f;
        float left   = 0.0f;
        float u      = 0.0f;
        float v      = 0.0f;
        State state  = State::Normal;
        float width  = 0.0f;
        float height = 0.0f;
        float id     = 0u;
        std::string text = "";
      } Button;
    }

    namespace menu {
      typedef struct Object {
        torero::gui::Menu *object  = nullptr;
        text::Alignment vertical   = text::Alignment::Center;
        text::Alignment horizontal = text::Alignment::Left;
        float width                = torero::k_menu_width;
        float height               = torero::k_menu_height;
        bool visibility            = false;
      } Object;
    }

    typedef struct ButtonShader{
      float top;
      float left;
      float u;
      float v;
      float width;
      float height;
      float id;
      float element;
    } ButtonShader;

    typedef struct PBR_Shader {
      float angle = 0.0f;
      // Object color
      torero::ColorRGB color = torero::ColorRGB{ 255.0f, 255.0f, 255.0f };
    } PBR_Shader;

    typedef struct TextShader{
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
    } TextShader;
  }
  // ------------------------------------------------------------------------------------ //
  // --------------------------------- CAMERA MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace camera {
    typedef struct Component {
      torero::camera::Camera *camera = nullptr;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Component;

    typedef struct Resolution {
      int window_width;  // Window width [screen coordinates]
      int window_height; // Window height [screen coordinates]
      // It is posible to define diferent resolutions for window and content, if distinct
      // then OpenGL will scalate the content to fit the window size
      int frame_width;  // Framebuffer width [pixels]
      int frame_height; // Framebuffer height [pixels]
      // Ratios between windows and frame sizes
      float relation = 1.0f;
    } Resolution;
  }

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- COVARIANCE MATRICES ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  namespace covariance {
    typedef struct Covariance {
      struct Mat2 {
        float m00, m01, m10, m11;
      } matrix;
      PointXYZ position;
    } Covariance;

    typedef struct Element{
      torero::covariance::Covariances *matrix = nullptr;
      std::string name;
      bool visibility;
      boost::signals2::connection connection;
    } Element;

    typedef struct Shader{
      algebraica::vec3f mean;
      algebraica::mat2f inverse;
      float constant;
    } Shader;
  }
}

#endif // TORERO_TYPE_DEFINITIONS_H
