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
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(DevIL REQUIRED)
find_package(Eigen3 3.3.4 COMPONENTS Core Geometry REQUIRED)

# // ------------------------------------------------------------------------------------ //
# // --------------------------------------- ROS ---------------------------------------- //
# // ------------------------------------------------------------------------------------ //
# checks if ROS_DATA is defined
if(DEFINED ROS_DATA)
  add_definitions(-DROS_DATA)
#  set(DATA_H extras/headers/rosdata.h)
#  set(DATA_S extras/sources/rosdata.cpp)

  include_directories(${catkin_INCLUDE_DIRS})

  set(ros_LIBRARIES ${catkin_LIBRARIES})
endif(DEFINED ROS_DATA)
# // ------------------------------------------------------------------------------------ //
# // ------------------------------------- END ROS -------------------------------------- //
# // ------------------------------------------------------------------------------------ //

#source files
set(CPP_FILES
  sources/console.cpp
  sources/gui.cpp
  sources/main.cpp
  sources/three_dimensional_model_loader.cpp
#  ${DATA_S}
)

#header files
set(HPP_FILES
  headers/console.h
  headers/definitions.h
  headers/gui.h
  headers/three_dimensional_model_loader.h
  headers/types.h
#  ${DATA_H}
  README.md
)

#including the resource files
set(PROJECT_RESOURCES
  resources/fonts.qrc
  resources/images.qrc
  resources/shaders.qrc
)

# ++++++++++++++++++++++++++++++++++++++ GLFW ++++++++++++++++++++++++++++++++++++++
# disable build the GLFW tests, examples and documentation
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# adding the root directory of the GLFW source tree to your project
add_subdirectory(GLFW)
include_directories(GLFW/include)

# ++++++++++++++++++++++++++++++++++++++ GLAD ++++++++++++++++++++++++++++++++++++++
# adding the root directory of the GLAD source tree to your project
include_directories(GLAD/include)
set(GLAD_FILES
  GLAD/include/glad/glad.h
  GLAD/src/glad.c
)

# +++++++++++++++++++++++++++++++++++ ALGEBRAICA +++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(algebraica)
set(ALGEBRAICA_FILES
  algebraica/Algebraica.h
  algebraica/vec2.h
  algebraica/vec3.h
  algebraica/vec4.h
  algebraica/mat4.h
)

# +++++++++++++++++++++++++++++++++++++ EIGEN ++++++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(${EIGEN3_INCLUDE_DIR})

# ++++++++++++++++++++++++++++++++++++++ DevIL +++++++++++++++++++++++++++++++++++++
INCLUDE_DIRECTORIES(${IL_INCLUDE_DIR})

# ++++++++++++++++++++++++++++++++++++++ BOOST +++++++++++++++++++++++++++++++++++++
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
# ------------------------------------ END BOOST -----------------------------------

#add every file to the executable
add_executable(${PROJECT_BINARY_NAME}
  ${GLAD_FILES}
  ${ALGEBRAICA_FILES}
  ${HPP_FILES}
  ${CPP_FILES}
  ${PROJECT_RESOURCES}
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
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  Eigen3::Eigen
  ${IL_LIBRARIES}
  ${ros_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
