#ifndef TORERO_MODEL_MANAGER_H
#define TORERO_MODEL_MANAGER_H

// OpenGL loader and core library
#include "glad/glad.h"

#include "torero/definition/types.h"
#include "torero/gl/shader.h"
#include "torero/model/loader.h"

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

  namespace model {
    class Manager
    {
    public:
      Manager(torero::Core *core);
      ~Manager();

      // Loads a model and its textures located in the folder = folder_address;
      // There must be the following files inside that folder:
      //   + Textures (Physically based rendering):
      //     - albedo.png       ->  Model's color texture (RGBA)
      //     - normal.png       ->  Model's normal map texture (RGB)
      //     - pbr_emission.png ->  Metallic (R channel), Roughness (G channel), Cavity (B channel)
      //                            and Emission (A channel)
      //   + Model:
      //     - model.obj      ->  3D model exported as Wavefront (.obj)
      // Returns the id of the loaded 3D model.
      MMid load_new_model(const std::string folder_address);
      // Returns the id of the loaded 3D model.
      MMid load_new_model(const torero::model::Models model);
      // This will duplicate an existing model with id = MMid, it is possible to change the
      // position of the duplicated model if you specify a *transformation_matrix, translate
      // or rotates the duplicated model.
      // Returns the id of the new duplicated 3D model or -1 if 3D model was not properly created.
      MMelement add(MMid id, const torero::Pose3D *object_frame);
      // Colorizes the 3D model, this will ignore the albedo texture and apply a uniform color
      // through the whole 3D model. Max value is 255.
      // Returns false if Model element with ID = id was not found.
      bool colorize(MMid model_id, MMelement element_id, const bool colorize = true,
                    const float R = 255.0f, const float G = 255.0f,
                    const float B = 255.0f, const float Alpha = 255.0f);
      // Metallizes the 3D model, this will ignore the metallic texture and apply a uniform metallic
      // effect through the whole 3D model. Max value is 1.0f.
      // Returns false if Model element with ID = id was not found.
      bool metallize(MMid model_id, MMelement element_id, const bool metallize = true,
                     const float metallic_value = 0.0f);
      // Roughen the 3D model, this will ignore the roughness texture and apply a uniform roughness
      // effect through the whole 3D model. Max value is 1.0f.
      // Returns false if Model element with ID = id was not found.
      bool roughen(MMid model_id, MMelement element_id, const bool roughen = true,
                   const float roughness_value = 0.0f);
      // Changes the visibility of the 3D model.
      // Returns false if Model element with ID = id was not found.
      bool set_visibility(MMid model_id, MMelement element_id, const bool visible = true);
      // Sets the transformation matrix for the 3D model with ID = id.
      // Returns false if Model element with ID = id was not found.
      bool set_transformation_matrix(MMid model_id, MMelement element_id,
                                     const torero::Pose3D *object_frame);
      // Translates the 3D model using X, Y and Z values
      // Returns false if Model element with ID = id was not found.
      bool translate(MMid model_id, MMelement element_id,
                     const float x = 0.0f, const float y = 0.0f, const float z = 0.0f);
      // Rotates the 3D model using Pitch, Yaw and Roll values
      // Returns false if Model element with ID = id was not found.
      bool rotate_with_euler(MMid model_id, MMelement element_id,
                             const float pitch = 0.0f,
                             const float yaw = 0.0f,
                             const float roll = 0.0f);
      bool rotate_with_quaternion(MMid model_id, MMelement element_id,
                                  const float x = 0.0f, const float y = 0.0f,
                                  const float z = 0.0f, const float w = 1.0f);
      bool rotate_in_axes(MMid model_id, MMelement element_id,
                          const float x = 0.0f, const float y = 0.0f,
                          const float z = 0.0f, const float angle = 0.0f);
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

    private:
      void draw(torero::model::Model3D *model, torero::model::Element3D *element);

      MMid load_db5();

      torero::Core *core_;

      torero::gl::Shader *pbr_shader_;
      GLint u_object_rotation_, u_object_translation_, u_frame_rotation_, u_frame_translation_;
      GLint u_quaternion_, u_quaternion_usage_, u_metallized_, u_metallic_value_;
      GLint u_roughed_, u_roughness_value_, u_colored_, u_color_, u_light_, u_light_color_;

      std::vector<torero::model::Model3D> models_;

      boost::signals2::connection connection_draw_;
    };
  }
}
#endif // TORERO_MODEL_MANAGER_H
