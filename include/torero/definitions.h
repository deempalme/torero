#ifndef TORERO_DEFINITIONS_H
#define TORERO_DEFINITIONS_H

// ------------------------------------------------------------------------------------ //
// -------------------------------- Window definitions -------------------------------- //
// ------------------------------------------------------------------------------------ //

constexpr int WINDOW_CLOSED     =  0;
constexpr int GLFW_NOT_LOADED   = -1;
constexpr int WINDOW_NOT_LOADED = -2;
constexpr int GLAD_NOT_LOADED   = -3;
constexpr int EXISTING_WINDOW   = -4;

constexpr int DEFAULT_WIDTH  = 1500;
constexpr int DEFAULT_HEIGHT = 800;

// ------------------------------------------------------------------------------------ //
// ---------------------------------- Point cloud types ------------------------------- //
// ------------------------------------------------------------------------------------ //

#define POINT_XYZ      0u
#define POINT_XYZI     1u
#define POINT_XYZRGB   2u
#define POINT_XYZRGBA  3u

// ------------------------------------------------------------------------------------ //
// ---------------------------- Physical values definitions --------------------------- //
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

constexpr float EARTH_RADIUS = 6378137.0f;
// Gravity constant acceleration on earth
constexpr float GRAVITY      = 9.81f;

// Radians to degrees
constexpr float toDEGREES(float value){ return value * 57.2957795131f; }
// Degrees to radians
constexpr float toRADIANS(float value){ return value * 0.01745329252f; }
// Meters per hour to kilometers per hour
constexpr float toKMPH(float value){ return value * 3.6f; }
// Static cast shortcuts
constexpr float FLOAT(double value){ return static_cast<float>(value); }

#endif

constexpr float INITIAL_ZOOM = 1.0f;
constexpr float MAXIMUM_ZOOM = 15.0f;
constexpr float MINIMUM_ZOOM = 0.05f;

//#define FIELD_OF_VIEW       1.047197551f
// 35mm lens (41.7degrees)
constexpr float FIELD_OF_VIEW = 0.7278023f;
constexpr float NEAR_PLANE    = 0.1f;
constexpr float FAR_PLANE     = 1000.0f;

#endif // TORERO_DEFINITIONS_H
