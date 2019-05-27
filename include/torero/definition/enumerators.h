#ifndef TORERO_ENUMERATORS_H
#define TORERO_ENUMERATORS_H

namespace torero {
  // ------------------------------------------------------------------------------------ //
  // ------------------------------- 3D MODEL MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace model {
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
  }

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- CAMERA MANAGEMENT -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace camera {
    enum class Dimensionality : unsigned int {
      ThreeDimensional = 0u,
      TwoDimensional   = 1u
    };

    enum class Zoom : unsigned int {
      In  = 0u,
      Out = 1u
    };
  }

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- GUI MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace gui {
    namespace object {
      enum Id : unsigned int {
        Menu     = 1u,
        TitleBar = 2u
      };
    }

    namespace mouse {
      enum Event : unsigned int {
        Move    = 0u,
        Click   = 1u,
        Release = 2u
      };
    }

    namespace button {
      enum State : unsigned int {
        Normal = 0u,
        Hover  = 1u,
        Click  = 2u
      };
    }

    namespace menu {
      enum ButtonType : unsigned int{
        ZoomIn        = 0u,
        IsometricView = 1u,
        TopView       = 2u,
        ZoomOut       = 3u,
        PEM_Logo      = 4u
      };
    }

    namespace title {
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
      Wait      = 4u,
      WaitArrow = 5u,
      ArrowNWSE = 6u,
      ArrowNESW = 7u,
      ArrowWE   = 8u,
      ArrowNS   = 9u,
      ArrowALL  = 10u,
      Forbidden = 11u
    };
  }

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- OBJECT MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace object {
    enum Shape : unsigned int{
      Box      = 0u,
      Cylinder = 1u,
      Circle   = 2u,
      Square   = 3u
    };
  }

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- POINT CLOUD MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace point_cloud {
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

    enum class Type : unsigned int{
      XYZ      = 0u,
      XYZI     = 1u,
      XYZRGB   = 2u,
      XYZRGBI  = 3u,
      XYZRGBA  = 4u,
      XYZRGBAI = 5u,
      XYZIAD   = 6u
    };
  }

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
  namespace text {
    enum class Alignment : unsigned int {
      Left   = 0u,
      Center = 1u,
      Right  = 2u,
      Top    = 3u,
      Bottom = 4u
    };

    enum class Weight : unsigned int {
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
  }

  // ------------------------------------------------------------------------------------ //
  // ------------------------------ TRAJECTORY MANAGEMENT ------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace trajectory {
    enum LineType : unsigned int {
      Solid          = 0u,
      Dotted         = 1u,
      Dashed         = 2u,
      Arrowed        = 3u
    };
  }

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- WINDOW MANAGEMENT --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  enum class Order : int {
    PointClouds  = 0,
    Covariance   = 1,
    Objects      = 2,
    Skybox       = 3,
    Ground       = 4,
    Streets      = 5,
    Models       = 6,
    Trajectories = 7,
    Text         = 8,
    Camera       = 9,
    GUI          = 10
  };

  enum WindowErrors : int {
    WindowClosed    =  0,
    SDL2_NotLoaded  = -1,
    WindowNotLoaded = -2,
    GLAD_NotLoaded  = -3,
    ExistingWindow  = -4,
    OpenGLNotLoaded = -5
  };

  namespace Message {
    enum MessageType : unsigned int {
      Error     = 0u,
      Warning   = 1u,
      Attention = 2u,
      Normal    = 3u
    };
  }

  enum class EventType : unsigned int {
    Down = 0u,
    Up   = 1u
  };

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------ CONTROLLER ------------------------------------ //
  // ------------------------------------------------------------------------------------ //
#ifndef T_M_C
#define T_M_C
  enum class Move : int {
    Backward  = -1,
    Beginning = 0,
    Forward   = 1,
    Ending    = 2
  };
#endif

  enum class ImageManagerType : unsigned int {
    Image   = 0u,
    Surface = 1u
  };

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- SHADER DATA ------------------------------------ //
  // ------------------------------------------------------------------------------------ //

  namespace ShaderLocation {
    enum Attributes : unsigned int {
      Position  = 0u,
      Normal    = 1u,
      UV        = 2u,
      Tangent   = 3u,
      Angle     = 2u,
      Color     = 5u,
      Size      = 2u,
      Id        = 3u,
      Element   = 4u,
      Scale     = 3u,
      Alpha     = 2u,
      Distance  = 3u,
      Width     = 2u,
      Intensity = 1u
    };
  }
  namespace ShaderTexture {
    enum TextureID : unsigned int {
      Cubemap     = 0u,
      Irradiance  = 1u,
      Prefilter   = 2u,
      BRDF        = 3u,
      Albedo      = 4u,
      Normal      = 5u,
      PBREmission = 6u,
      Albedo2     = 5u
    };
  }

  namespace ShaderAttachment {
    enum ColorID : unsigned int {
      Albedo    = 0u, // RGBA
      Normal    = 1u, // RGB
      MRAO      = 2u, // RGB: Metallic, Roughness, and Ambient Occlusion
      Emission  = 3u, // RGBA
      Depth     = 4u  // R
    };
  }
  namespace BufferLocation {
    enum LocationID : unsigned int {
      Scene  = 1u
    };
  }

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- TERMINAL COLORS ---------------------------------- //
  // ------------------------------------------------------------------------------------ //

  namespace terminal {
    namespace color {
      enum Code : int {
        Black         = 0,
        Red           = 1,
        Green         = 2,
        Yellow        = 3,
        Blue          = 4,
        Magenta       = 5,
        Cyan          = 6,
        White         = 7,
        BrightBlack   = 8,
        BrightRed     = 9,
        BrightGreen   = 10,
        BrightYellow  = 11,
        BrightBlue    = 12,
        BrightMagenta = 13,
        BrightCyan    = 14,
        BrightWhite   = 15,
      };

      enum Style : int {
        Reset         = 0,
        Bold          = 1,
        Faint         = 2,
        Italic        = 3,
        Underline     = 4,
        SlowBlink     = 5,
        RapidBlink    = 6,
        Inverse       = 7, // Swap text and background color
        Conceil       = 8,
        CrossedOut    = 9,
        DefaultFont   = 10,
        BoldOff       = 21,
        Normal        = 22,
        ItalicOff     = 23,
        UnderlineOff  = 24,
        InverseOff    = 27,
        Reveal        = 28,
        CrossedOutOff = 29,
        Foreground    = 38,
        Background    = 48,
        Overline      = 53,
        OverlineOff   = 55
      };
    }
  }

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- KEY CODES ------------------------------------ //
  // ------------------------------------------------------------------------------------ //

  namespace keyboard {
    enum KeyMod : int {
      NONE               = 0,
      CTRL               = 0x00c0,
      SHIFT              = 0x0003,
      ALT                = 0x0300,
      GUI                = 0x0c00,
      CTRL_SHIFT         = 0x00c3,
      CTRL_ALT           = 0x03c0,
      CTRL_GUI           = 0x0cc0,
      SHIFT_ALT          = 0x0303,
      SHIFT_GUI          = 0x0c03,
      ALT_GUI            = 0x0f00,
      CTRL_SHIFT_ALT     = 0x03c3,
      CTRL_SHIFT_GUI     = 0x0cc3,
      CTRL_ALT_GUI       = 0x0fc0,
      SHIFT_ALT_GUI      = 0x0f03,
      CTRL_SHIFT_ALT_GUI = 0x0fc3,
      NUM                = 0x1000,
      CAPS               = 0x2000,
      MODE               = 0x4000,
      RESERVED           = 0x8000,
      NOT_NONE           = 0x0fff
    };
  }
}

#endif // TORERO_ENUMERATORS_H
