#ifndef TORERO_CAMERA_MANAGER_H
#define TORERO_CAMERA_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"
// Torero includes
#include "torero/camera_element.h"
#include "torero/shader.h"
#include "torero/type_definitions.h"
// Linear mathematics
#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace torero {
  class Core;

  class CameraManager
  {
  public:
    CameraManager(Core *core);
    ~CameraManager();

    // ------------------------------------------------------------------------------------ //
    // --------------------------------- CAMERA MANAGEMENT -------------------------------- //
    // ------------------------------------------------------------------------------------ //

    CMid add(const torero::ImageFile *video_input,
             const std::string name,
             const torero::Dimensionality number_of_dimensions
                = torero::Dimensionality::ThreeDimensional,
             const algebraica::mat4f *transformation_matrix = nullptr,
             const bool visible = true);

    bool set_visibility(const CMid id, const bool visible = true);

    bool set_camera_curvature(const CMid id, const float curvature = 1.0f);

    bool set_transformation_matrix(const CMid id,
                                   const algebraica::mat4f *transformation_matrix);

    bool translate(const CMid id, const float x = 0.0f,
                   const float y = 0.0f, const float z = 0.0f);

    bool resize(const CMid id, const float width, const float height);

    bool rotate(const CMid id, const float pitch = 0.0f,
                const float yaw = 0.0f, const float roll = 0.0f);

    bool rotate_in_x(const CMid id, const float angle);

    bool rotate_in_y(const CMid id, const float angle);

    bool rotate_in_z(const CMid id, const float angle);

    bool update(const CMid id);

    void update_all();

    bool draw(const CMid id);

    void draw_all();

    bool delete_camera(const CMid id);

    void purge();

    bool connect(const CMid id, boost::signals2::signal<void ()> *signal);

    void connect_all(boost::signals2::signal<void ()> *signal);

  private:
    void update_camera();
    bool load_3d_model();

    Core *core_;

    Shader *shader_;
    GLint u_pv_;
    std::vector<torero::CameraComponent> cameras_;

    boost::signals2::connection signal_updated_camera_, signal_draw_all_;
    boost::signals2::connection signal_updated_all_;
  };
}

#endif // TORERO_CAMERA_MANAGER_H
