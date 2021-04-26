#ifndef TORERO_MODEL_MANAGER_H
#define TORERO_MODEL_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "include/definitions.h"
#include "include/three_dimensional_model_loader.h"
#include "include/shader.h"
#include "include/skybox.h"
#include "include/cubemap.h"
#include "include/types.h"

#include "algebraica/algebraica.h"
// signals and slots
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
// standard
#include <iostream>
#include <string>
#include <vector>

namespace Toreo {
  class Core;

  class ModelManager
  {
  public:
    ModelManager(Core *core);
    ~ModelManager();

    // Loads a model and its textures located in the folder = folder_address;
    // There must be the following files inside that folder:
    //   + Textures (Physically based rendering):
    //     - albedo.png     ->  Model's color texture
    //     - ao.png         ->  Model's ambient occlusion texture
    //     - metallic.png   ->  Model's metallic texture
    //     - normal.png     ->  Model's normal map texture
    //     - roughness.png  ->  Model's roughness texture
    //   + Model:
    //     - model.obj      ->  3D model exported as Wavefront (.obj)
    // Returns the id of the loaded 3D model.
    MMid load_new_model(const std::string folder_address);
    // Returns the id of the loaded 3D model.
    MMid load_new_model(const Visualizer::Models model);
    // This will duplicate an existing model with id = MMid, it is possible to change the
    // position of the duplicated model if you specify a *transformation_matrix, translate
    // or rotates the duplicated model.
    // Returns the id of the new duplicated 3D model or -1 if 3D model was not properly created.
    MMelement add(MMid id, const algebraica::mat4f *transformation_matrix);
    // Colorizes the 3D model, this will ignore the albedo texture and apply a uniform color
    // through the whole 3D model. Max value is 255.
    // Returns false if Model element with ID = id was not found.
    bool colorize(MMid model_id, MMelement element_id, const bool colorize = true,
                  const int R = 0, const int G = 0, const int B = 0, const int Alpha = 255);
    // Metallizes the 3D model, this will ignore the metallic texture and apply a uniform metallic
    // effect through the whole 3D model. Max value is 1.0f.
    // Returns false if Model element with ID = id was not found.
    bool metallize(MMid model_id, MMelement element_id,
                   const bool metallize = true, const float metallic_value = 0.0f);
    // Roughen the 3D model, this will ignore the roughness texture and apply a uniform roughness
    // effect through the whole 3D model. Max value is 1.0f.
    // Returns false if Model element with ID = id was not found.
    bool roughen(MMid model_id, MMelement element_id,
                 const bool roughen = true, const float roughness_value = 0.0f);
    // Changes the visibility of the 3D model.
    // Returns false if Model element with ID = id was not found.
    bool set_visibility(MMid model_id, MMelement element_id, const bool visible = true);
    // Sets the transformation matrix for the 3D model with ID = id.
    // Returns false if Model element with ID = id was not found.
    bool set_transformation_matrix(MMid model_id, MMelement element_id,
                                   const algebraica::mat4f *transformation_matrix);
    // Translates the 3D model using X, Y and Z values
    // Returns false if Model element with ID = id was not found.
    bool translate(MMid model_id, MMelement element_id,
                   const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
    // Rotates the 3D model using Pitch, Yaw and Roll values
    // Returns false if Model element with ID = id was not found.
    bool rotate(MMid model_id, MMelement element_id,
                const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f);
    // Performs a rotation in the 3D model around its X axis
    // Returns false if Model element with ID = id was not found.
    bool rotate_in_x(MMid model_id, MMelement element_id, const float angle);
    // Performs a rotation in the 3D model around its Y axis
    // Returns false if Model element with ID = id was not found.
    bool rotate_in_y(MMid model_id, MMelement element_id, const float angle);
    // Performs a rotation in the 3D model around its Z axis
    // Returns false if Model element with ID = id was not found.
    bool rotate_in_z(MMid model_id, MMelement element_id, const float angle);
    // Draws the 3D model's duplicated with id = element_id into the screen.
    // Returns false if the duplicated element does not exist
    // or the 3D model was not proerly created.
    bool draw_element(MMid model_id, MMelement element_id);
    // Draws all the duplicated elements of the 3D model with id = model_id into the screen.
    // Returns false if the 3D model was not proerly created.
    bool draw_model(MMid model_id);
    // Draws all the 3D models
    void draw_all();
    // Deletes the 3D model element (duplicate) width ID = id,
    // Returns false if 3D model width ID = id is already deleted.
    bool delete_element(MMid model_id, MMelement element_id);
    // Deletes the 3D model width ID = id and its elements (duplicates),
    // Returns false if 3D model width ID = id and its duplicates are already deleted.
    bool delete_model(MMid id);
    // This will delete all 3D models and free memory, if you create and delete 3D models
    // dinamically call this method from time to time to increase the program performance,
    // all the previous 3D models' IDs will not work anymore
    void purge();
    // Creates a skybox (background image)
    bool skybox(const std::string up    = "resources/skybox/up.png",
                const std::string down  = "resources/skybox/dn.png",
                const std::string left  = "resources/skybox/lf.png",
                const std::string right = "resources/skybox/rt.png",
                const std::string front = "resources/skybox/ft.png",
                const std::string back  = "resources/skybox/bk.png");
    // Draws skybox
    bool skybox_draw();
    // Hides or displays the skybox (scene background)
    bool skybox_visibility(const bool hidden = true);
    // Defines the sun's direction and color
    void sun_properties(const algebraica::vec3f direction =
                            algebraica::vec3f(-0.866f, 0.70711f, 0.70711f),
                        const int R = 255, const int G = 255, const int B = 255);

  private:
    void draw(Visualizer::Model3D *model, Visualizer::Model3DElement *element);
    void update_camera();
    void update_vehicle_model();

    MMid load_db5();
    MMid load_shuttle();

    Core *core_;

    Shader *model_shader_;
    GLint m_u_scene_model_, m_u_object_model_, m_u_view_, m_u_projection_, m_u_light_;
    GLint m_u_light_color_, m_u_light_size_, m_u_camera_, m_u_sun_, m_u_sun_color_;
    GLint m_u_pbr_, m_u_fog_, m_u_metallized_, m_u_metallic_value_, m_u_roughed_;
    GLint m_u_roughness_value_, m_u_colored_, m_u_color_, m_u_emitting_;

    Skybox *skybox_;
    bool skybox_visibility_;

    Cubemap *cubemap_;

    std::vector<Visualizer::Model3D> models_;

    boost::signals2::connection signal_updated_camera_, signal_draw_all_;

    algebraica::vec3f sun_direction_, sun_color_;
  };
}

#endif // TORERO_MODEL_MANAGER_H
