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

#header files
set(HPP_FILES
  includes/buffer.h
  includes/camera.h
  includes/core.h
  includes/definitions.h
  includes/model_manager.h
  includes/objects.h
  includes/object_manager.h
  includes/point_cloud_manager.h
  includes/point_cloud.h
  includes/shader.h
  includes/skybox.h
  includes/texture.h
  includes/three_dimensional_model_loader.h
  includes/types.h
)

#source files
set(CPP_FILES
  src/camera.cpp
  src/core.cpp
  src/model_manager.cpp
  src/objects.cpp
  src/object_manager.cpp
  src/point_cloud_manager.cpp
  src/point_cloud.cpp
  src/skybox.cpp
  src/three_dimensional_model_loader.cpp
)

#resource files
set(RESOURCE_FILES
  resources/shaders/brdf.frag
  resources/shaders/brdf.vert
  resources/shaders/irradiance.frag
  resources/shaders/cubemap.vert
  resources/shaders/objects.frag
  resources/shaders/objects.vert
  resources/shaders/PBR.frag
  resources/shaders/PBR.vert
  resources/shaders/point_cloud.frag
  resources/shaders/point_cloud.vert
  resources/shaders/prefilter.frag
  resources/shaders/skybox.frag
  resources/shaders/skybox.vert
)

# ++++++++++++++++++++++++++++++++++++++ GLFW ++++++++++++++++++++++++++++++++++++++
# disable build the GLFW tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# adding the root directory of the GLFW source tree to your project
add_subdirectory(libs/glfw)

# ++++++++++++++++++++++++++++++++++++++ GLAD ++++++++++++++++++++++++++++++++++++++
# adding the root directory of the GLAD source tree to your project
set(GLAD_FILES
  libs/GLAD/include/glad/glad.h
  libs/GLAD/include/KHR/khrplatform.h
  libs/GLAD/src/glad.c
)

# +++++++++++++++++++++++++++++++++++ ALGEBRAICA +++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
add_subdirectory(libs/algebraica)

# ++++++++++++++++++++++++++++++ COORDINATE CONVERSOR ++++++++++++++++++++++++++++++
# adding the root directory of coordinate_conversor source tree to your project
add_subdirectory(libs/coordinate_conversor)

# ++++++++++++++++++++++++++++++++ stb_image loader ++++++++++++++++++++++++++++++++
add_definitions(-DSTB_IMAGE_STATIC)
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
add_definitions(-DSTBI_NO_HDR)
# adding the root directory of the stb library source tree to your project
set(STB_FILES
  libs/stb/stb_image.h
)

set(TORERO_DIRS_LOCAL
  libs/glfw/include
  libs/GLAD/include
  ${ALGEBRAICA_DIRS}
  ${COORDINATE_DIRS}
  libs/stb
  ${Boost_INCLUDE_DIR}
)

include_directories(${TORERO_DIRS_LOCAL})

set(TORERO_DIRS
  libs/glfw/include
  libs/GLAD/include
  ${ALGEBRAICA_DIRS}
  ${COORDINATE_DIRS}
  libs/stb
  libs/torero
  libs/torero/includes

  PARENT_SCOPE
)

set(TORERO_LIB ${TORERO_NAME} PARENT_SCOPE)

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
  glfw
  ${glfw_LIBRARIES}
  ${ALGEBRAICA_LIB}
  ${COORDINATE_LIB}
  ${OPENGL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
