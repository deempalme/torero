#include "headers/three_dimensional_model_loader.h"

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const char *file_address, const int type,
                                                         const int shader_program) :
  file_address_(file_address),
  type_(type),
  shader_program_(shader_program),
  is_free_(false),
  is_initialized_(initialize())
{
}

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const string file_address, const int type,
                                                         const int shader_program) :
  file_address_(file_address.c_str()),
  type_(type),
  shader_program_(shader_program),
  is_free_(false),
  is_initialized_(initialize())
{
}

ThreeDimensionalModelLoader::ThreeDimensionalModelLoader(const unsigned int model, const int type,
                                                         const int shader_program) :
  type_(type),
  shader_program_(shader_program),
  is_free_(false)
{
  switch(model){
  case DB5:
    file_address_ = "";
    break;
  case TIRE:
    file_address_ = "";
    break;
  case SHUTTLE:
    file_address_ = "";
    break;
  default:
    file_address_ = "";
  }
  is_initialized_ = initialize();
}

ThreeDimensionalModelLoader::~ThreeDimensionalModelLoader(){

}

const bool &ThreeDimensionalModelLoader::is_free(){
  return is_free_;
}

bool ThreeDimensionalModelLoader::initialize(){
  bool ok{false};
  return ok;
}
