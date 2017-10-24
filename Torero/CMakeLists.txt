#cmake version
cmake_minimum_required(VERSION 3.1)

#project name
project(Torero)
set(PROJECT_BINARY_NAME Torero)

# // ------------------------------------------------------------------------------------ //
# // --------------------------------------- ROS ---------------------------------------- //
# // ------------------------------------------------------------------------------------ //
# use the next line if you are using ROS
#set(ROS_DATA true)

if(DEFINED ROS_DATA)
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    message_filters
    message_generation
  )

  add_message_files(
    FILES
    object.msg
    object_vec.msg
  )

  generate_messages(DEPENDENCIES std_msgs)

  catkin_package(
    CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
  )
endif(DEFINED ROS_DATA)
# // ------------------------------------------------------------------------------------ //
# // ------------------------------------- END ROS -------------------------------------- //
# // ------------------------------------------------------------------------------------ //

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# compatibility with c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# compatibility with C++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# start debug mode
set(CMAKE_BUILD_TYPE Debug)

# Widgets finds its own dependencies
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(DevIL REQUIRED)
find_package(Eigen3 3.3.4 COMPONENTS Core Geometry REQUIRED)

# // ------------------------------------------------------------------------------------ //
# // --------------------------------------- ROS ---------------------------------------- //
# // ------------------------------------------------------------------------------------ //
# checks if ROS_DATA is defined
if(DEFINED ROS_DATA)
  add_definitions(-DROS_DATA)
#  set(DATA_H extras/headers/rosdata.h)
#  set(DATA_S extras/src/rosdata.cpp)

  include_directories(${catkin_INCLUDE_DIRS})

  set(ros_LIBRARIES ${catkin_LIBRARIES})
endif(DEFINED ROS_DATA)
# // ------------------------------------------------------------------------------------ //
# // ------------------------------------- END ROS -------------------------------------- //
# // ------------------------------------------------------------------------------------ //

#source files
set(CPP_FILES
  src/console.cpp
  src/gui.cpp
  src/main.cpp
  src/skybox.cpp
  src/three_dimensional_model_loader.cpp
#  ${DATA_S}
)

#header files
set(HPP_FILES
  includes/console.h
  includes/definitions.h
  includes/gui.h
  includes/skybox.h
  includes/three_dimensional_model_loader.h
  includes/types.h
#  ${DATA_H}
  README.md
)

set(SHADER_FILES
  resources/shaders/PBR.vert
  resources/shaders/PBR.frag
  resources/shaders/irradiance_convolution.frag
  resources/shaders/skybox.vert
  resources/shaders/skybox.frag
  resources/shaders/cubemap.vert
)

# ++++++++++++++++++++++++++++++++++++++ GLFW ++++++++++++++++++++++++++++++++++++++
# disable build the GLFW tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# adding the root directory of the GLFW source tree to your project
add_subdirectory(libs/GLFW)
include_directories(libs/GLFW/include)

# ++++++++++++++++++++++++++++++++++++++ GLAD ++++++++++++++++++++++++++++++++++++++
# adding the root directory of the GLAD source tree to your project
include_directories(libs/GLAD/include)
set(GLAD_FILES
  libs/GLAD/include/glad/glad.h
  libs/GLAD/src/glad.c
)

# +++++++++++++++++++++++++++++++++++ ALGEBRAICA +++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(libs/algebraica)
set(ALGEBRAICA_FILES
  libs/algebraica/Algebraica.h
  libs/algebraica/vec2.h
  libs/algebraica/vec3.h
  libs/algebraica/vec4.h
  libs/algebraica/mat3.h
  libs/algebraica/mat4.h
)

# +++++++++++++++++++++++++++++++++++++ EIGEN ++++++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(${EIGEN3_INCLUDE_DIR})

# ++++++++++++++++++++++++++++++++++++++ DevIL +++++++++++++++++++++++++++++++++++++
INCLUDE_DIRECTORIES(${IL_INCLUDE_DIR})

# ++++++++++++++++++++++++++++++++ stb_image loader ++++++++++++++++++++++++++++++++
# adding the root directory of the stb library source tree to your project
include_directories(libs/stb)
set(STB_FILES
  libs/stb/stb_image.h
)

# ++++++++++++++++++++++++++++++++++++++ BOOST +++++++++++++++++++++++++++++++++++++
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
# ------------------------------------ END BOOST -----------------------------------

#add every file to the executable
add_executable(${PROJECT_BINARY_NAME}
  ${GLAD_FILES}
  ${STB_FILES}
  ${ALGEBRAICA_FILES}
  ${HPP_FILES}
  ${CPP_FILES}
  ${SHADER_FILES}
)

# // ------------------------------------------------------------------------------------ //
# // --------------------------------------- ROS ---------------------------------------- //
# // ------------------------------------------------------------------------------------ //
if(DEFINED ROS_DATA)
  add_dependencies(${PROJECT_BINARY_NAME} ${PROJECT_NAME}_generate_messages_cpp ${catkin_EXPORTED_TARGETS})
endif(DEFINED ROS_DATA)
# // ------------------------------------------------------------------------------------ //
# // ------------------------------------- END ROS -------------------------------------- //
# // ------------------------------------------------------------------------------------ //

#indicates which libraries to use in the executable
target_link_libraries(${PROJECT_BINARY_NAME}
  glfw
  ${glfw_LIBRARIES}
  ${OPENGL_LIBRARIES}
#  ${Boost_FILESYSTEM_LIBRARY}
#  ${Boost_SYSTEM_LIBRARY}
#  ${Boost_THREAD_LIBRARY}
  ${Boost_LIBRARIES}
  Eigen3::Eigen
  ${IL_LIBRARIES}
  ${ros_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
