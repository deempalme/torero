#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// ------------------------------------------------------------------------------------ //
// -------------------------------- Window definitions -------------------------------- //
// ------------------------------------------------------------------------------------ //

#define WINDOW_CLOSED          0
#define GLFW_NOT_LOADED       -1
#define WINDOW_NOT_LOADED     -2
#define GLAD_NOT_LOADED       -3
#define EXISTING_WINDOW       -4

// ------------------------------------------------------------------------------------ //
// -------------------------------- Errors and warnings ------------------------------- //
// ------------------------------------------------------------------------------------ //

#define FILE_NOT_FOUND        0

// ------------------------------------------------------------------------------------ //
// -------------------------------- 3D Model definitions ------------------------------ //
// ------------------------------------------------------------------------------------ //

#define DB5_BODY              0u
#define DB5_WINDOWS           1u
#define DB5_ACCESSORIES       2u
#define DB5_INTERIOR          3u
#define CHASSIS               4u
#define AXIS                  5u
#define STEERING              6u
#define TIRE                  7u
#define COORDINATE_SYSTEM     8u

// ------------------------------------------------------------------------------------ //
// ---------------------------- Physical values definitions --------------------------- //
// ------------------------------------------------------------------------------------ //

#define _PI                 3.14159265359f
#define _2PI                6.28318530718f
#define _PI2                1.57079632679f
#define _PI4                0.78539816339f
#define _3PI2               4.71238898038f

#define toDEGREES           57.2957795131f
#define toRADIANS           0.01745329252f
#define toKMPH              3.6f
#define GRAVITY             9.81f

#endif // DEFINITIONS_H