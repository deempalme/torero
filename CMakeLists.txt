#cmake version
cmake_minimum_required(VERSION 3.1)

#project name
project(Torero)
set(TORERO_NAME Torero)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# compatibility with c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# finding dependencies
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

#header files
set(HPP_FILES
  include/buffer.h
  include/camera.h
  include/core.h
  include/cubemap.h
  include/definitions.h
  include/line_grid.h
  include/ground.h
  include/ground_manager.h
  include/model_manager.h
  include/objects.h
  include/object_manager.h
  include/point_cloud_manager.h
  include/point_cloud.h
  include/shader.h
  include/skybox.h
  include/texture.h
  include/three_dimensional_model_loader.h
  include/trajectory.h
  include/trajectory_manager.h
  include/types.h
  include/vehicle_manager.h
)

#source files
set(CPP_FILES
  src/camera.cpp
  src/core.cpp
  src/cubemap.cpp
  src/line_grid.cpp
  src/ground.cpp
  src/ground_manager.cpp
  src/model_manager.cpp
  src/objects.cpp
  src/object_manager.cpp
  src/point_cloud_manager.cpp
  src/point_cloud.cpp
  src/skybox.cpp
  src/three_dimensional_model_loader.cpp
  src/trajectory_manager.cpp
  src/trajectory.cpp
  src/vehicle_manager.cpp
)

#resource files
set(RESOURCE_FILES
  resources/shaders/brdf.frag
  resources/shaders/brdf.vert
  resources/shaders/irradiance.frag
  resources/shaders/cubemap.vert
  resources/shaders/ground.frag
  resources/shaders/ground.geom
  resources/shaders/ground.vert
  resources/shaders/lines.frag
  resources/shaders/lines.vert
  resources/shaders/objects.frag
  resources/shaders/objects.vert
  resources/shaders/PBR.frag
  resources/shaders/PBR.vert
  resources/shaders/point_cloud.frag
  resources/shaders/point_cloud.vert
  resources/shaders/prefilter.frag
  resources/shaders/skybox.frag
  resources/shaders/skybox.vert
  resources/shaders/trajectory.frag
  resources/shaders/trajectory.geom
  resources/shaders/trajectory.vert
)

# ++++++++++++++++++++++++++++++++++++++ GLAD ++++++++++++++++++++++++++++++++++++++
# adding the root directory of the GLAD source tree to your project
set(GLAD_FILES
  lib/GLAD/include/glad/glad.h
  lib/GLAD/include/KHR/khrplatform.h
  lib/GLAD/src/glad.c
)

# +++++++++++++++++++++++++++++++++++ ALGEBRAICA +++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
add_subdirectory(lib/algebraica)

# ++++++++++++++++++++++++++++++ COORDINATE CONVERSOR ++++++++++++++++++++++++++++++
# adding the root directory of coordinate_conversor source tree to your project
add_subdirectory(lib/coordinate_conversor)

# ++++++++++++++++++++++++++++++++ stb_image loader ++++++++++++++++++++++++++++++++
add_definitions(-DSTB_IMAGE_STATIC)
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
add_definitions(-DSTB_IMAGE_WRITE_IMPLEMENTATION)
add_definitions(-DSTBI_NO_HDR)
# adding the root directory of the stb library source tree to your project
set(STB_FILES
  lib/stb/stb_image.h
  lib/stb/stb_image_write.h
)

set(TORERO_DIRS_LOCAL
  lib/GLAD/include
  ${ALGEBRAICA_DIRS}
  ${COORDINATE_DIRS}
  lib/stb
  ${Boost_INCLUDE_DIR}
)

include_directories(${TORERO_DIRS_LOCAL})

set(TORERO_DIRS
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/lib/GLAD/include
  ${ALGEBRAICA_DIRS}
  ${COORDINATE_DIRS}
  ${PROJECT_SOURCE_DIR}/lib/stb

  CACHE INTERNAL ""
)

set(TORERO_LIB ${TORERO_NAME} CACHE INTERNAL "")

file(GLOB_RECURSE RESOURCES
  LIST_DIRECTORIES true
  ${PROJECT_SOURCE_DIR}/resources/*
)
foreach(RESOURCE ${RESOURCES})
  if(IS_DIRECTORY ${RESOURCE})
    file(COPY ${RESOURCE} DESTINATION ${CMAKE_BINARY_DIR}/resources)
  endif(IS_DIRECTORY ${RESOURCE})
endforeach(RESOURCE)

#add every file to the executable
add_library(${TORERO_NAME}
  ${GLAD_FILES}
  ${STB_FILES}
  ${HPP_FILES}
  ${CPP_FILES}
  ${RESOURCE_FILES}
)

#indicates which libraries to use in the executable
target_link_libraries(${TORERO_NAME}
  ${GLFW_LIBRARIES}
  ${ALGEBRAICA_LIB}
  ${COORDINATE_LIB}
  ${OPENGL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
