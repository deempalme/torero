#ifndef TYPES_H
#define TYPES_H

#include "Algebraica.h"

class ThreeDimensionalModelLoader;

struct ComplexShaderData{
  // vertex position
  float x  = 0;
  float y  = 0;
  float z  = 0;
  // vertex normals
  float nx = 0;
  float ny = 0;
  float nz = 0;
  // vertex texture coordinates
  float ux = 0;
  float uy = 0;
  // vertex tangential normals
  float tx = 0;
  float ty = 0;
  float tz = 0;
  // vertex bitangential normals
  float bx = 0;
  float by = 0;
  float bz = 0;
};

struct SimpleShaderData{
  // vertex position
  vec3f position;
  // vertex texture coordinates
  vec2f texture;
  // vertex normals
  vec3f normal;
};

struct Model3D{
  ThreeDimensionalModelLoader *model;
  mat4f transformation_matrix;
};

struct ImageFile{
  int width = 0;
  int height = 0;
  int components_size = 0;
  float *data = nullptr;
};

//#include "includes/three_dimensional_model_loader.h"

#endif // TYPES_H
