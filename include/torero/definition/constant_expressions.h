#ifndef TORERO_CONSTANT_EXPRESSIONS_H
#define TORERO_CONSTANT_EXPRESSIONS_H

#include <cstddef>
#include <stdint.h>

namespace torero {
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- Window definitions -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  constexpr int k_default_width  = 1500;
  constexpr int k_default_height = 800;

  constexpr float k_initial_zoom = 1.0f;
  constexpr float k_maximum_zoom = 15.0f;
  constexpr float k_minimum_zoom = 0.05f;

  // 45 degrees = 0.785398163 radians
  constexpr float k_field_of_view = 0.785398163f;
  constexpr float k_near_plane    = 0.1f;    // Meters
  constexpr float k_far_plane     = 500.0f; // Meters

  // Minimum camera distance to its target
  constexpr float k_min_distance = 0.1f;   // Meters

  // Distance moved when the arrow keys are pressed
  constexpr float k_moving_distance = 3.0f; // Meters

  // 20 Hertz
  constexpr double k_frequency = 50; // milliseconds

  // Drawing elements type quantity: (objects, trajectories, ground, point cloud, etc.)
  constexpr std::size_t k_drawing_elements{11};

  // ------------------------------------------------------------------------------------ //
  // ---------------------------------- GUI definitions --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  constexpr float k_menu_width         = 371.0f;
  constexpr float k_menu_height        = 97.0f;
  constexpr float k_menu_button_width  = 80.0f;
  constexpr float k_menu_button_height = 87.0f;

  constexpr float k_title_height       = 30.0f;
  constexpr float k_title_button_width = 30.0f;

  constexpr float k_ortho_width        = 5.0f;
  constexpr float k_ortho_height       = 2.67f;

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- Default files ---------------------------------- //
  // ------------------------------------------------------------------------------------ //

  constexpr char k_brdf_path[]                = TORERO_RESOURCE_DIR  "/resources/environment/brdf.bin";
  constexpr int  k_brdf_components            = 4;   // RGBA channels
  constexpr int  k_brdf_resolution            = 512; // pixels
  constexpr int  k_cubemap_resolution         = 512; // pixels
  constexpr char k_default_skybox_path[]      = TORERO_RESOURCE_DIR  "/resources/cubemap/dark_night.hdr";
  constexpr char k_default_environment_path[] = TORERO_RESOURCE_DIR  "/resources/environment/rathaus.hdr";
  constexpr char k_icon_default_path[]        = TORERO_RESOURCE_DIR  "/resources/images/icon.png";

  constexpr char k_gui_folder[]          = TORERO_RESOURCE_DIR  "/resources/GUI";
  constexpr char k_font_iceland_image[]  = TORERO_RESOURCE_DIR  "/resources/fonts/Iceland.png";
  constexpr char k_font_iceland_config[] = TORERO_RESOURCE_DIR  "/resources/fonts/Iceland.fnt";
  constexpr char k_trajectory_solid[]    = TORERO_RESOURCE_DIR  "/resources/models3D/trajectory/solid.png";
  constexpr char k_trajectory_dotted[]   = TORERO_RESOURCE_DIR  "/resources/models3D/trajectory/dotted.png";
  constexpr char k_trajectory_dashed[]   = TORERO_RESOURCE_DIR  "/resources/models3D/trajectory/dashed.png";
  constexpr char k_trajectory_arrowed[]  = TORERO_RESOURCE_DIR  "/resources/models3D/trajectory/arrowed.png";
  constexpr char k_cylinder_ao[]         = TORERO_RESOURCE_DIR  "/resources/models3D/cylinder/ao.png";
  constexpr char k_box_ao[]              = TORERO_RESOURCE_DIR  "/resources/models3D/box/ao.png";
  constexpr char k_square_ao[]           = TORERO_RESOURCE_DIR  "/resources/models3D/square/ao.png";
  constexpr char k_circle_ao[]           = TORERO_RESOURCE_DIR  "/resources/models3D/circle/ao.png";
  constexpr char k_arrow_ao[]            = TORERO_RESOURCE_DIR  "/resources/models3D/arrow/ao.png";

  // :::::::::::::::::::::::::::::::::::::: 3D Models ::::::::::::::::::::::::::::::::::::::

  constexpr char k_compass_model[]         = TORERO_RESOURCE_DIR  "/resources/models3D/compass";
  constexpr char k_speedo_model[]          = TORERO_RESOURCE_DIR  "/resources/models3D/speedo";
  constexpr char k_db5_body_model[]        = TORERO_RESOURCE_DIR  "/resources/models3D/db5_body";
  constexpr char k_db5_windows_model[]     = TORERO_RESOURCE_DIR  "/resources/models3D/db5_windows";
  constexpr char k_db5_accessories_model[] = TORERO_RESOURCE_DIR  "/resources/models3D/db5_accessories";
  constexpr char k_db5_interior_model[]    = TORERO_RESOURCE_DIR  "/resources/models3D/db5_interior";
  constexpr char k_chassis_model[]         = TORERO_RESOURCE_DIR  "/resources/models3D/chassis";
  constexpr char k_axis_model[]            = TORERO_RESOURCE_DIR  "/resources/models3D/axis";
  constexpr char k_steering_model[]        = TORERO_RESOURCE_DIR  "/resources/models3D/steering";
  constexpr char k_tire_model[]            = TORERO_RESOURCE_DIR  "/resources/models3D/tire";
  constexpr char k_shuttle_body_model[]    = TORERO_RESOURCE_DIR  "/resources/models3D/shuttle_body";
  constexpr char k_shuttle_windows_model[] = TORERO_RESOURCE_DIR  "/resources/models3D/shuttle_windows";
  constexpr char k_coordinates_model[]     = TORERO_RESOURCE_DIR  "/resources/models3D/coordinate_system";

  // ::::::::::::::::::::::::::::::::::::::: Shaders :::::::::::::::::::::::::::::::::::::::

  constexpr char k_brdf_vertex[]          = TORERO_RESOURCE_DIR  "/resources/shaders/brdf.vert";
  constexpr char k_brdf_fragment[]        = TORERO_RESOURCE_DIR  "/resources/shaders/brdf.frag";
  constexpr char k_cubemap_vertex[]       = TORERO_RESOURCE_DIR  "/resources/shaders/cubemap_generator.vert";
  constexpr char k_cubemap_fragment[]     = TORERO_RESOURCE_DIR  "/resources/shaders/cubemap_generator.frag";
  constexpr char k_environment_vertex[]   = TORERO_RESOURCE_DIR  "/resources/shaders/environment.vert";
  constexpr char k_environment_fragment[] = TORERO_RESOURCE_DIR  "/resources/shaders/environment.frag";
  constexpr char k_irradiance_vertex[]    = TORERO_RESOURCE_DIR  "/resources/shaders/cubemap.vert";
  constexpr char k_irradiance_fragment[]  = TORERO_RESOURCE_DIR "/resources/shaders/irradiance.frag";
  constexpr char k_prefilter_vertex[]     = TORERO_RESOURCE_DIR "/resources/shaders/cubemap.vert";
  constexpr char k_prefilter_fragment[]   = TORERO_RESOURCE_DIR "/resources/shaders/prefilter.frag";
  constexpr char k_skybox_vertex[]        = TORERO_RESOURCE_DIR "/resources/shaders/skybox.vert";
  constexpr char k_skybox_fragment[]      = TORERO_RESOURCE_DIR "/resources/shaders/skybox.frag";
  constexpr char k_pbr_vertex[]           = TORERO_RESOURCE_DIR "/resources/shaders/PBR_complex.vert";
  constexpr char k_pbr_fragment[]         = TORERO_RESOURCE_DIR "/resources/shaders/PBR.frag";

  constexpr char k_point_cloud_vertex[]   = TORERO_RESOURCE_DIR "/resources/shaders/point_cloud.vert";
  constexpr char k_point_cloud_fragment[] = TORERO_RESOURCE_DIR "/resources/shaders/point_cloud.frag";
  constexpr char k_camera_vertex[]        = TORERO_RESOURCE_DIR "/resources/shaders/camera.vert";
  constexpr char k_camera_fragment[]      = TORERO_RESOURCE_DIR "/resources/shaders/camera.frag";
  constexpr char k_covariance_vertex[]    = TORERO_RESOURCE_DIR "/resources/shaders/covariance.vert";
  constexpr char k_covariance_fragment[]  = TORERO_RESOURCE_DIR "/resources/shaders/covariance.frag";
  constexpr char k_ground_vertex[]        = TORERO_RESOURCE_DIR "/resources/shaders/ground.vert";
  constexpr char k_ground_fragment[]      = TORERO_RESOURCE_DIR "/resources/shaders/ground.frag";
  constexpr char k_ground_geometry[]      = TORERO_RESOURCE_DIR "/resources/shaders/ground.geom";
  constexpr char k_lines_vertex[]         = TORERO_RESOURCE_DIR "/resources/shaders/lines.vert";
  constexpr char k_lines_fragment[]       = TORERO_RESOURCE_DIR "/resources/shaders/lines.frag";

  constexpr char k_pbr_gui_vertex[]     = TORERO_RESOURCE_DIR "/resources/shaders/gui_PBR.vert";
  constexpr char k_pbr_gui_fragment[]   = TORERO_RESOURCE_DIR "/resources/shaders/gui_PBR.frag";
  constexpr char k_3D_gui_vertex[]      = TORERO_RESOURCE_DIR "/resources/shaders/gui_3D.vert";
  constexpr char k_3D_gui_fragment[]    = TORERO_RESOURCE_DIR "/resources/shaders/gui_3D.frag";
  constexpr char k_gui_mouse_vertex[]   = TORERO_RESOURCE_DIR "/resources/shaders/gui_mouse.vert";
  constexpr char k_gui_mouse_fragment[] = TORERO_RESOURCE_DIR "/resources/shaders/gui_mouse.frag";
  constexpr char k_2D_gui_vertex[]      = TORERO_RESOURCE_DIR "/resources/shaders/gui_2D.vert";
  constexpr char k_2D_gui_fragment[]    = TORERO_RESOURCE_DIR "/resources/shaders/gui_2D.frag";
  constexpr char k_gui_text_vertex[]    = TORERO_RESOURCE_DIR "/resources/shaders/gui_text.vert";
  constexpr char k_gui_text_fragment[]  = TORERO_RESOURCE_DIR "/resources/shaders/gui_text.frag";
  constexpr char k_gui_text_geometry[]  = TORERO_RESOURCE_DIR "/resources/shaders/gui_text.geom";

  constexpr char k_text_vertex[]      = TORERO_RESOURCE_DIR "/resources/shaders/text.vert";
  constexpr char k_text_fragment[]    = TORERO_RESOURCE_DIR "/resources/shaders/text.frag";
  constexpr char k_text_geometry[]    = TORERO_RESOURCE_DIR "/resources/shaders/text.geom";
  constexpr char k_objects_vertex[]   = TORERO_RESOURCE_DIR "/resources/shaders/objects.vert";
  constexpr char k_objects_fragment[] = TORERO_RESOURCE_DIR "/resources/shaders/objects.frag";

  constexpr char k_trajectory_vertex[]   = TORERO_RESOURCE_DIR "/resources/shaders/trajectory.vert";
  constexpr char k_trajectory_fragment[] = TORERO_RESOURCE_DIR "/resources/shaders/trajectory.frag";
  constexpr char k_trajectory_geometry[] = TORERO_RESOURCE_DIR "/resources/shaders/trajectory.geom";

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- Physical value definitions --------------------------- //
  // ------------------------------------------------------------------------------------ //

#ifndef P_V_D
#define P_V_D
  #define _1D            1
  #define _2D            2
  #define _3D            3
  #define _4D            4
#endif

#ifndef C_C_M
#define C_C_M

  constexpr float _PI   = 3.14159265359f;
  // 2 * PI
  constexpr float _2PI  = 6.28318530718f;
  // PI / 2
  constexpr float _PI2  = 1.57079632679f;
  // PI / 3
  constexpr float _PI3  = 1.04719755100f;
  // 4 * PI / 3
  constexpr float _4PI3 = 4.18879020500f;
  // PI / 4
  constexpr float _PI4  = 0.78539816339f;
  // 3/2 * PI
  constexpr float _3PI2 = 4.71238898038f;

  constexpr float k_360_degrees = 6.283190f;
  constexpr float k_270_degrees = 4.712390f;
  constexpr float k_210_degrees = 3.665190f;
  constexpr float k_190_degrees = 3.316130f;
  constexpr float k_180_degrees = 3.141590f;
  constexpr float k_105_degrees = 1.832600f;
  constexpr float k_90_degrees  = 1.570796f;
  constexpr float k_45_degrees  = 0.785398f;
  constexpr float k_42_degrees  = 0.733038f;
  constexpr float k_22_degrees  = 0.383972f;
  constexpr float k_20_degrees  = 0.349066f;
  constexpr float k_15_degrees  = 0.261799f;
  constexpr float k_10_degrees  = 0.174533f;
  constexpr float k_1_degree    = 0.0174533f;

  constexpr float k_earth_radius = 6378137.0f;
  // Gravity constant acceleration on earth
  constexpr float k_gravity      = 9.81f;

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- Physical convertions ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  // Radians to degrees
  constexpr float to_degrees(float value){ return value * 57.2957795131f; }
  // Degrees to radians
  constexpr float to_radians(float value){ return value * 0.01745329252f; }
  constexpr double to_radians(double angle){ return angle * 3.14159265358979323846 / 180.0; }
  // Meters per hour to kilometers per hour
  constexpr float to_KMPH(float value){ return value * 3.6f; }

  // ------------------------------------------------------------------------------------ //
  // ------------------------------- Static cast shortcuts ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  constexpr float to_float(const double value)      { return static_cast<float>(value); }
  constexpr float to_float(const int value)         { return static_cast<float>(value); }
  constexpr float to_float(const unsigned int value){ return static_cast<float>(value); }

  constexpr int to_int(const float value) { return static_cast<int>(value); }
  constexpr int to_int(const double value){ return static_cast<int>(value); }

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- Comparation using floats ----------------------------- //
  // ------------------------------------------------------------------------------------ //

  // APPLICATION:
  //   if( compared != comparation )
  constexpr bool float_differentiation(const float compared,
                                       const float comparation = 0.0f,
                                       const float range = 0.05f){
    return compared > (comparation + range) || compared < (comparation - range);
  }

  // APPLICATION:
  //   if( compared == comparation )
  constexpr bool float_equalization(const float compared,
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
