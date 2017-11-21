#cmake version
cmake_minimum_required(VERSION 3.1)

#project name
project(Torero)
set(PROJECT_BINARY_NAME Torero)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# compatibility with c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# start debug mode
set(CMAKE_BUILD_TYPE Debug)

# Widgets finds its own dependencies
find_package(OpenGL REQUIRED)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

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

  add_definitions(-DROS_DATA)
  set(DATA_H libs/ros/includes/ros_data.h)
  set(DATA_S libs/ros/src/ros_data.cpp)

  include_directories(${catkin_INCLUDE_DIRS})

  set(ros_LIBRARIES ${catkin_LIBRARIES})
endif(DEFINED ROS_DATA)
# // ------------------------------------------------------------------------------------ //
# // ------------------------------------- END ROS -------------------------------------- //
# // ------------------------------------------------------------------------------------ //

#header files
set(HPP_FILES
  includes/buffer.h
  includes/camera.h
  includes/core.h
  includes/definitions.h
  includes/model_manager.h
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
  src/point_cloud_manager.cpp
  src/point_cloud.cpp
  src/skybox.cpp
  src/three_dimensional_model_loader.cpp
  main.cpp
)

#resource files
set(RESOURCE_FILES
  resources/shaders/brdf.vert
  resources/shaders/brdf.frag
  resources/shaders/cubemap.vert
  resources/shaders/irradiance.frag
  resources/shaders/PBR.vert
  resources/shaders/PBR.frag
  resources/shaders/point_cloud.frag
  resources/shaders/point_cloud.vert
  resources/shaders/prefilter.frag
  resources/shaders/skybox.vert
  resources/shaders/skybox.frag
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
  libs/GLAD/include/KHR/khrplatform.h
  libs/GLAD/src/glad.c
)

# +++++++++++++++++++++++++++++++++++ ALGEBRAICA +++++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(libs/algebraica)
set(ALGEBRAICA_FILES
  libs/algebraica/Algebraica.h
  libs/algebraica/linear.h
  libs/algebraica/vec2.h
  libs/algebraica/vec3.h
  libs/algebraica/vec4.h
  libs/algebraica/mat3.h
  libs/algebraica/mat4.h
)

# ++++++++++++++++++++++++++++++++ stb_image loader ++++++++++++++++++++++++++++++++
add_definitions(-DSTB_IMAGE_STATIC)
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
add_definitions(-DSTBI_NO_HDR)
# adding the root directory of the stb library source tree to your project
include_directories(libs/stb)
set(STB_FILES
  libs/stb/stb_image.h
)

# ++++++++++++++++++++++++++++++++++++++ BOOST +++++++++++++++++++++++++++++++++++++
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# +++++++++++++++++++++++++++++++++ KITTI LIBRARIES ++++++++++++++++++++++++++++++++
# adding the root directory of algebraica source tree to your project
include_directories(libs/kitti)
set(KITTI_FILES
  libs/kitti/includes/kitti_point_cloud_reader.h
  libs/kitti/src/kitti_point_cloud_reader.cpp
)
# ------------------------------- END KITTI LIBRARIES ------------------------------

#add every file to the executable
add_executable(${PROJECT_BINARY_NAME}
  ${GLAD_FILES}
  ${STB_FILES}
  ${ALGEBRAICA_FILES}
  ${KITTI_FILES}
  ${HPP_FILES}
  ${CPP_FILES}
  ${RESOURCE_FILES}
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
  ${Boost_LIBRARIES}
  ${ros_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
