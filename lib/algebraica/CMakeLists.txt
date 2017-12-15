#cmake version
cmake_minimum_required(VERSION 3.1)

#project name
project(Algebraica)
set(ALGEBRAICA_NAME Algebraica)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# compatibility with c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#header files
set(HPP_FILES
  Algebraica.h
  linear.h
  vec2.h
  vec3.h
  vec4.h
  mat3.h
  mat4.h
)

set(ALGEBRAICA_DIRS ${PROJECT_SOURCE_DIR} CACHE INTERNAL "")
set(ALGEBRAICA_LIB ${ALGEBRAICA_NAME} CACHE INTERNAL "")

#add every file to the executable
add_library(${ALGEBRAICA_NAME}
  ${HPP_FILES}
)

set_target_properties(${ALGEBRAICA_NAME} PROPERTIES LINKER_LANGUAGE CXX)
