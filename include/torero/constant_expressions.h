#ifndef TORERO_CONSTANT_EXPRESSIONS_H
#define TORERO_CONSTANT_EXPRESSIONS_H

namespace torero {
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- Window definitions -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  constexpr int kDefaultWidth  = 1500;
  constexpr int kDefaultHeight = 800;

  constexpr float kInitialZoom = 1.0f;
  constexpr float kMaximumZoom = 15.0f;
  constexpr float kMinimumZoom = 0.05f;

  // 35mm lens (41.7degrees = 0.7278023
  constexpr float kFieldOfView = 0.7278023f;
  constexpr float kNearPlane    = 0.1f;
  constexpr float kFarPlane     = 1000.0f;

  // 20 Hertz
  constexpr double kFrequency    = 0.05;

  // Drawing elements type quantity: (objects, trajectories, ground, point cloud, etc.)
  constexpr int kDrawingElements{10};

  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- GUI definitions --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  constexpr float kMenuWidth         = 371.0f;
  constexpr float kMenuHeight        = 97.0f;
  constexpr float kMenuButtonWidth   = 80.0f;
  constexpr float kMenuButtonHeight  = 87.0f;

  constexpr float kTitleHeight       = 30.0f;
  constexpr float kTitleButtonWidth  = 30.0f;

  constexpr float kOrthoWidth        = 5.0f;
  constexpr float kOrthoHeight       = 2.67f;

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- Point cloud types -------------------------------- //
  // ------------------------------------------------------------------------------------ //
  /*
  #define POINT_XYZ      0u
  #define POINT_XYZI     1u
  #define POINT_XYZRGB   2u
  #define POINT_XYZRGBA  3u
  */
  // ------------------------------------------------------------------------------------ //
  // ----------------------------- Physical value definitions --------------------------- //
  // ------------------------------------------------------------------------------------ //

  #define _1D            1
  #define _2D            2
  #define _3D            3
  #define _4D            4

#ifndef C_C_M
#define C_C_M

  constexpr float _PI          = 3.14159265359f;
  // 2 * PI
  constexpr float _2PI         = 6.28318530718f;
  // PI / 2
  constexpr float _PI2         = 1.57079632679f;
  // PI / 3
  constexpr float _PI3         = 1.04719755100f;
  // 4 * PI / 3
  constexpr float _4PI3        = 4.18879020500f;
  // PI / 4
  constexpr float _PI4         = 0.78539816339f;
  // 3/2 * PI
  constexpr float _3PI2        = 4.71238898038f;

  constexpr float k360degrees = 6.283190f;
  constexpr float k270degrees = 4.712390f;
  constexpr float k210degrees = 3.665190f;
  constexpr float k190degrees = 3.316130f;
  constexpr float k180degrees = 3.141590f;
  constexpr float k105degrees = 1.832600f;
  constexpr float k90degrees  = 1.570796f;
  constexpr float k42degrees  = 0.733038f;
  constexpr float k22degrees  = 0.383972f;
  constexpr float k20degrees  = 0.349066f;
  constexpr float k15degrees  = 0.261799f;
  constexpr float k10degrees  = 0.174533f;

  constexpr float kEarthRadius = 6378137.0f;
  // Gravity constant acceleration on earth
  constexpr float kGravity      = 9.81f;

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- Physical convertions ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  // Radians to degrees
  constexpr float ToDegrees(float value){ return value * 57.2957795131f; }
  // Degrees to radians
  constexpr float ToRadians(float value){ return value * 0.01745329252f; }
  // Meters per hour to kilometers per hour
  constexpr float toKMPH(float value){ return value * 3.6f; }

  // ------------------------------------------------------------------------------------ //
  // ------------------------------- Static cast shortcuts ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  constexpr float ToFloat(const double value)      { return static_cast<float>(value); }
  constexpr float ToFloat(const int value)         { return static_cast<float>(value); }
  constexpr float ToFloat(const unsigned int value){ return static_cast<float>(value); }

  constexpr int ToInt(const float value) { return static_cast<int>(value); }
  constexpr int ToInt(const double value){ return static_cast<int>(value); }

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- Comparation using floats ----------------------------- //
  // ------------------------------------------------------------------------------------ //

  // APPLICATION:
  //   if( compared != comparation )
  constexpr bool FloatDifferentiation(const float compared,
                                      const float comparation = 0.0f,
                                      const float range = 0.05f){
    return compared > (comparation + range) && compared < (comparation - range);
  }

  // APPLICATION:
  //   if( compared == comparation )
  constexpr bool FloatEqualization(const float compared,
                                   const float comparation = 0.0f,
                                   const float range = 0.05f){
    return compared > (comparation - range) && compared < (comparation + range);
  }
#endif

#ifndef V_C_M_C
#define V_C_M_C
  namespace ImageEncoding {
    constexpr int RED  = 1;
    constexpr int RG   = 2;
    constexpr int RGB  = 3;
    constexpr int BGR  = 3;
    constexpr int RGBA = 4;
    constexpr int BGRA = 4;
  }
#endif
}

#endif // TORERO_CONSTANT_EXPRESSIONS_H
