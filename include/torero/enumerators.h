#ifndef TORERO_ENUMERATORS_H
#define TORERO_ENUMERATORS_H

namespace torero {
  // ------------------------------------------------------------------------------------ //
  // ------------------------------- 3D MODEL MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum Models : unsigned int {
    Empty             = 0u,
    DB5_Body          = 1u,
    DB5_Windows       = 2u,
    DB5_Accessories   = 3u,
    DB5_Interior      = 4u,
    Chassis           = 5u,
    Axis              = 6u,
    Steering          = 7u,
    Tire              = 8u,
    CoordinateSystem  = 9u,
    DB5               = 10u,
    ShuttleBody       = 11u,
    ShuttleWindows    = 12u,
    Shuttle           = 13u
  };

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- CAMERA MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum class Dimensionality : unsigned int {
    ThreeDimensional = 0u,
    TwoDimensional   = 1u
  };

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- GUI MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace GUIid {
    enum Id : unsigned int {
      Menu     = 1u,
      TitleBar = 2u
    };
  }

  namespace Mouse {
    enum Event : unsigned int {
      Move    = 0u,
      Click   = 1u,
      Release = 2u
    };
  }

  namespace Button {
    enum State : unsigned int {
      Normal = 0u,
      Hover  = 1u,
      Click  = 2u
    };
  }

  namespace Menu {
    enum ButtonType : unsigned int{
      ZoomIn        = 0u,
      IsometricView = 1u,
      TopView       = 2u,
      ZoomOut       = 3u,
      PEM_Logo      = 4u
    };
  }

  namespace Title {
    enum ButtonType : unsigned int {
      Title    = 0u,
      Close    = 1u,
      Maximize = 2u,
      Minimize = 3u,
      Options  = 4u
    };
  }

  enum class Cursor : unsigned int {
    Normal    = 0u,
    Hand      = 1u,
    Text      = 2u,
    CrossHair = 3u,
    Move      = 4u,
    Hidden    = 5u
  };

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- OBJECT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum Shape : unsigned int{
    Box      = 0u,
    Cylinder = 1u,
    Circle   = 2u,
    Square   = 3u
  };

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //

#ifndef P_C_DENSE_E
#define P_C_DENSE_E
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
#endif

  enum class ColorMode : unsigned int{
    Intensity = 0u,
    ColorMap  = 1u,
    Solid     = 2u,
    Data      = 3u
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
  // --------------------------------- STREETS MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifndef S_M_S_E
#define S_M_S_E
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

  enum class SignalType : unsigned int {
    Semaphore,
    SpeedLimit,
    Stop,
    TrafficCone
  };

#endif

  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- TEXT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum class Alignment : unsigned int {
    Left   = 0u,
    Center = 1u,
    Right  = 2u,
    Top    = 3u,
    Bottom = 4u
  };

  enum class TextWeight : unsigned int {
    Lighter = 0u,
    Light   = 1u,
    Normal  = 2u,
    Bold    = 3u,
    Bolder  = 4u
  };

  enum class Complexity : unsigned int {
    Simple  = 0u,
    Medium  = 1u,
    Complex = 2u
  };

  // ------------------------------------------------------------------------------------ //
  // ------------------------------ TRAJECTORY MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum LineType : unsigned int {
    Solid          = 0u,
    Dotted         = 1u,
    Dashed         = 2u,
    Arrowed        = 3u
  };

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum class Order : int {
    PointClouds  = 0,
    Objects      = 1,
    Skybox       = 2,
    Ground       = 3,
    Streets      = 4,
    Models       = 5,
    Trajectories = 6,
    Text         = 7,
    Camera       = 8,
    GUI          = 9
  };

  enum WindowErrors : int {
    WindowClosed    =  0,
    GLFW_NotLoaded  = -1,
    WindowNotLoaded = -2,
    GLAD_NotLoaded  = -3,
    ExistingWindow  = -4,
  };

  enum class Message : unsigned int {
    Error     = 0u,
    Warning   = 1u,
    Attention = 2u,
    Normal    = 3u
  };

}

#endif // TORERO_ENUMERATORS_H
