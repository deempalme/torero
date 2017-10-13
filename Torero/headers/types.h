#ifndef TYPES_H
#define TYPES_H

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
  float uz = 0;
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
  float uz = 0;
};

#endif // TYPES_H
