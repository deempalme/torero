#ifndef TORERO_DEFINITIONS_H
#define TORERO_DEFINITIONS_H

// ------------------------------------------------------------------------------------ //
// -------------------------------- Window definitions -------------------------------- //
// ------------------------------------------------------------------------------------ //

#define WINDOW_CLOSED          0
#define GLFW_NOT_LOADED       -1
#define WINDOW_NOT_LOADED     -2
#define GLAD_NOT_LOADED       -3
#define EXISTING_WINDOW       -4

#define DEFAULT_WIDTH       1500
#define DEFAULT_HEIGHT      800

// ------------------------------------------------------------------------------------ //
// ---------------------------------- Point cloud types ------------------------------- //
// ------------------------------------------------------------------------------------ //

#define POINT_XYZ             0u
#define POINT_XYZI            1u
#define POINT_XYZRGB          2u
#define POINT_XYZRGBA         3u

// ------------------------------------------------------------------------------------ //
// ---------------------------- Physical values definitions --------------------------- //
// ------------------------------------------------------------------------------------ //

#define _1D                 1
#define _2D                 2
#define _3D                 3
#define _4D                 4

#ifndef C_C_M
#define C_C_M

#define _PI                 3.14159265359f
// 2 * PI
#define _2PI                6.28318530718f
// PI / 2
#define _PI2                1.57079632679f
// PI / 3
#define _PI3                1.04719755100f
// 4 * PI / 3
#define _4PI3               4.18879020500f
// PI / 4
#define _PI4                0.78539816339f
// 3/2 * PI
#define _3PI2               4.71238898038f

#define EARTH_RADIUS        6378137.0f

// Radians to degrees
#define toDEGREES           57.2957795131f
// Degrees to radians
#define toRADIANS           0.01745329252f
// Meters per hour to kilometers per hour
#define toKMPH              3.6f
// Gravity constant acceleration on earth
#define GRAVITY             9.81f

#endif

#define INITIAL_ZOOM        1.0f
#define MAXIMUM_ZOOM        15.0f
#define MINIMUM_ZOOM        0.05f

//#define FIELD_OF_VIEW       1.047197551f
// 35mm lens (41.7degrees)
#define FIELD_OF_VIEW       0.7278023f
#define NEAR_PLANE          0.1f
#define FAR_PLANE           1000.0f

#endif // TORERO_DEFINITIONS_H
