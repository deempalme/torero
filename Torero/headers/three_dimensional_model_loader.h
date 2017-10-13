#ifndef THREE_DIMENSIONAL_MODEL_LOADER_H
#define THREE_DIMENSIONAL_MODEL_LOADER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "headers/definitions.h"
#include "headers/types.h"

#include "Algebraica.h"
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <IL/il.h>

#include <thread>
#include <mutex>
#include <string>

using namespace std;
using namespace boost;

class ThreeDimensionalModelLoader
{
public:
  explicit ThreeDimensionalModelLoader(const char *file_address, const int type,
                                       const int shader_program);
  explicit ThreeDimensionalModelLoader(const string file_address, const int type,
                                       const int shader_program);
  explicit ThreeDimensionalModelLoader(const unsigned int model, const int type,
                                       const int shader_program);
  ~ThreeDimensionalModelLoader();

  const bool &is_free();

  // ------------------------------------------------------------------------------------ //
  // ------------------------------------- SIGNALS -------------------------------------- //
  // ------------------------------------------------------------------------------------ //
  signals2::signal<void (const char*, int)> error_signal;

private:
  bool initialize();

  const char *file_address_;
  const int type_, shader_program_;
  bool is_free_, is_initialized_;

  vec3f translation_, scaling_;
  vec3f rotation_, subrotation_;
  mat4f m_model_, m_submodel_;
  mat3f m_normal_model_, m_normal_submodel_;

  unsigned int vertex_attributes_, vertex_buffer_, data_size_;
  unsigned int i_position_, i_normal_, i_uv_;
  unsigned int u_normal_, u_subnormal_, u_model_, u_submodel_, u_scale_;
};

#endif // THREE_DIMENSIONAL_MODEL_LOADER_H
