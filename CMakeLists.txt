#cmake version
cmake_minimum_required(VERSION 3.5)

if(NOT TARGET ${TORERO_LIB})
  #project name
  set(TORERO_NAME torero)
  project(${TORERO_NAME})

  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  # Compatibility with c++14
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  # Compatilibity with c++14 in Qt
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

  # finding dependencies
  find_package(OpenGL REQUIRED)
  find_package(Boost REQUIRED COMPONENTS system chrono filesystem thread)
  find_package(Threads REQUIRED)
  # Loading SDL2
  find_package(SDL2 REQUIRED)

  #header files
  set(HPP_FILES
    # Camera
    include/torero/camera/camera.h
    include/torero/camera/manager.h
    include/torero/camera/view.h
    # Covariance
    include/torero/covariance/covariance.h
    include/torero/covariance/manager.h
    # General definitions
    include/torero/definition/constant_expressions.h
    include/torero/definition/enumerators.h
    include/torero/definition/types.h
    # Keyboard, mouse, touch events
    include/torero/event/handler.h
    # File utilities
    include/torero/file/manager.h
    # OpenGL utilities
    include/torero/gl/buffer.h
    include/torero/gl/frame_buffer.h
    include/torero/gl/render_buffer.h
    include/torero/gl/render_processor.h
    include/torero/gl/shader.h
    include/torero/gl/texture.h
    include/torero/gl/uniform_buffer.h
    # Ground
    include/torero/ground/ground.h
    include/torero/ground/line_grid.h
    include/torero/ground/manager.h
    # GUI
    include/torero/gui/compass.h
    include/torero/gui/controller.h
    include/torero/gui/cursors.h
    include/torero/gui/loader.h
    include/torero/gui/manager.h
    include/torero/gui/menu.h
#    include/torero/gui/options.h
    include/torero/gui/screen_conversor.h
    include/torero/gui/speedometer.h
    include/torero/gui/title_bar.h
    # Image file utilities
    include/torero/image/manager.h
    # 3D models
    include/torero/model/environment.h
    include/torero/model/loader.h
    include/torero/model/manager.h
#    include/torero/model/skybox.h
    # Objects
    include/torero/object/manager.h
    include/torero/object/objects.h
    # Point clouds
    include/torero/point_cloud/manager.h
    include/torero/point_cloud/cloud.h
    # Terminal utilities
    include/torero/terminal/printer.h
    # Text
    include/torero/text/font_loader.h
    include/torero/text/manager.h
    include/torero/text/text_object.h
    # Multithreading
    include/torero/thread/manager.h
    # Trajectories
    include/torero/trajectory/manager.h
    include/torero/trajectory/trajectories.h
    # Vehicle
    include/torero/vehicle/manager.h
    # Main
    include/torero/core.h
  )

  #source files
  set(CPP_FILES
    # Camera
    src/torero/camera/camera.cpp
    src/torero/camera/manager.cpp
    src/torero/camera/view.cpp
    # Covariance
    src/torero/covariance/covariance.cpp
    src/torero/covariance/manager.cpp
    # Keyboard, mouse, touch events
    src/torero/event/handler.cpp
    # File utilities
    src/torero/file/manager.cpp
    # OpenGL utilities
    src/torero/gl/buffer.cpp
    src/torero/gl/frame_buffer.cpp
    src/torero/gl/render_buffer.cpp
    src/torero/gl/render_processor.cpp
    src/torero/gl/texture.cpp
    # Ground
    src/torero/ground/ground.cpp
    src/torero/ground/line_grid.cpp
    src/torero/ground/manager.cpp
    # GUI
    src/torero/gui/compass.cpp
    src/torero/gui/controller.cpp
    src/torero/gui/cursors.cpp
    src/torero/gui/loader.cpp
    src/torero/gui/manager.cpp
    src/torero/gui/menu.cpp
#    src/torero/gui/options.cpp
    src/torero/gui/screen_conversor.cpp
    src/torero/gui/speedometer.cpp
    src/torero/gui/title_bar.cpp
    # Image file utilities
    src/torero/image/manager.cpp
    # 3D models
    src/torero/model/environment.cpp
    src/torero/model/loader.cpp
    src/torero/model/manager.cpp
#    src/torero/model/skybox.cpp
    src/torero/vehicle/manager.cpp
    # Objects
    src/torero/object/manager.cpp
    src/torero/object/objects.cpp
    # Point clouds
    src/torero/point_cloud/manager.cpp
    src/torero/point_cloud/cloud.cpp
    # Terminal utilities
    src/torero/terminal/printer.cpp
    # Text
    src/torero/text/font_loader.cpp
    src/torero/text/manager.cpp
    src/torero/text/text_object.cpp
    # Multithreading
    src/torero/thread/manager.cpp
    # Trajectories
    src/torero/trajectory/manager.cpp
    src/torero/trajectory/trajectories.cpp
    # vehicle
    src/torero/vehicle/manager.cpp
    # Main
    src/torero/core.cpp
  )

  #resource files
  set(RESOURCE_FILES
    resources/shaders/brdf.frag
    resources/shaders/brdf.vert
    resources/shaders/irradiance.frag
    resources/shaders/cubemap.vert
    resources/shaders/cubemap_generator.frag
    resources/shaders/cubemap_generator.vert
    resources/shaders/ground.frag
    resources/shaders/ground.geom
    resources/shaders/ground.vert
    resources/shaders/gui_2D.frag
    resources/shaders/gui_2D.vert
    resources/shaders/gui_mouse.frag
    resources/shaders/gui_mouse.vert
    resources/shaders/gui_PBR.frag
    resources/shaders/gui_PBR.vert
    resources/shaders/gui_text.frag
    resources/shaders/gui_text.geom
    resources/shaders/gui_text.vert
    resources/shaders/lines.frag
    resources/shaders/lines.vert
    resources/shaders/objects.frag
    resources/shaders/objects.vert
    resources/shaders/PBR.frag
    resources/shaders/PBR.vert
    resources/shaders/PBR_complex.vert
    resources/shaders/point_cloud.frag
    resources/shaders/point_cloud.vert
    resources/shaders/prefilter.frag
    resources/shaders/skybox.frag
    resources/shaders/skybox.vert
    resources/shaders/text.frag
    resources/shaders/text.geom
    resources/shaders/text.vert
    resources/shaders/trajectory.frag
    resources/shaders/trajectory.geom
    resources/shaders/trajectory.vert
  )

  # // ------------------------------------------------------------------------------------ //
  # // --------------------------------------- ROS ---------------------------------------- //
  # // ------------------------------------------------------------------------------------ //
  if(DEFINED CATKIN_DEVEL_PREFIX)
    #––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––#
    #      adding all the ROS elements inside this IF conditional      #
    #––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––#
    ## Find catkin macros and libraries
    ## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
    ## is used, also find other catkin packages
    find_package(catkin REQUIRED COMPONENTS
      rosbag
      roscpp
      roslib
    )
    ###################################
    ## catkin specific configuration ##
    ###################################
    ## The catkin_package macro generates cmake config files for your package
    ## Declare things to be passed to dependent projects
    ## INCLUDE_DIRS: uncomment this if you package contains header files
    ## LIBRARIES: libraries you create in this project that dependent projects also need
    ## CATKIN_DEPENDS: catkin_packages dependent projects also need
    ## DEPENDS: system dependencies of this project that dependent projects also need
    catkin_package(
      #INCLUDE_DIRS include
      CATKIN_DEPENDS roscpp rosbag
      LIBRARIES ${TORERO_NAME}
    )

    set(ROS_HPP_FILES
      include/torero/ros/bag_controller.h
    )
    set(ROS_CPP_FILES
      src/torero/ros/bag_controller.cpp
    )

    #ros directory
    set(ROS_INC_DIR /opt/ros/kinetic)

    #set include directories
    set(INC_DIRS
      ${ROS_INC_DIR}/include
      ${catkin_INCLUDE_DIRS}
    )

    #set lib directories
    set(LIB_DIRS
      ${ROS_INC_DIR}/lib
    )

    #include directories
    include_directories(${INC_DIRS})
    link_directories(${LIB_DIRS})

  endif(DEFINED CATKIN_DEVEL_PREFIX)
  # // ------------------------------------------------------------------------------------ //
  # // ------------------------------------- END ROS -------------------------------------- //
  # // ------------------------------------------------------------------------------------ //

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
  add_subdirectory(lib/gps_coordinate_conversor)

  # ++++++++++++++++++++++++++++++++ stb_image loader ++++++++++++++++++++++++++++++++
  add_definitions(-DSTB_IMAGE_STATIC)
  add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
  add_definitions(-DSTB_IMAGE_WRITE_IMPLEMENTATION)
#  add_definitions(-DSTBI_NO_HDR)
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
    ${SDL2_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    include
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

  file(GLOB RESOURCES
    LIST_DIRECTORIES true
    ${PROJECT_SOURCE_DIR}/resources/[!.]*
  )
  foreach(RESOURCE ${RESOURCES})
    if(IS_DIRECTORY ${RESOURCE})
      file(COPY ${RESOURCE} DESTINATION ${PROJECT_BINARY_DIR}/resources)
    endif(IS_DIRECTORY ${RESOURCE})
  endforeach(RESOURCE)

  #add every file to the executable
  add_library(${TORERO_NAME}
    ${GLAD_FILES}
    ${STB_FILES}
    ${HPP_FILES}
    ${CPP_FILES}
    ${ROS_HPP_FILES}
    ${ROS_CPP_FILES}
    ${RESOURCE_FILES}
  )

  target_compile_definitions(${TORERO_NAME}
    PUBLIC
    TORERO_RESOURCE_DIR="${PROJECT_BINARY_DIR}"
  )

  #indicates which libraries to use in the executable
  target_link_libraries(${TORERO_NAME}
    ${SDL2_LIBRARIES}
    ${ALGEBRAICA_LIB}
    ${COORDINATE_LIB}
    # Multithread management library
    Threads::Threads
    # Boost libraries
    ${Boost_LIBRARIES}
    # CMake utilities libraries
    ${CMAKE_DL_LIBS}
    # OpenGL libraries
    ${OPENGL_LIBRARIES}
    # Catkin
    ${catkin_LIBRARIES}
  )

#  if(DEFINED CATKIN_DEVEL_PREFIX)
#    add_dependencies(${TORERO_NAME}
#      ${catkin_EXPORTED_TARGETS}
#    )
#  endif(DEFINED CATKIN_DEVEL_PREFIX)

  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-function -pedantic")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wundef -Wpointer-arith -Wcast-align")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code -Wno-ignored-qualifiers")

  # Color sets:
  if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[0m")
    set(BoldGreen   "${Esc}[1;32m")
  endif()
  # This will print the folder where the "resources" folder is located
  # Remember to execute you program inside the folder below, so, torero would be able to
  # find all 3D models and images
  message(
    "\n\n${BoldGreen}" # Coloring text to bold green (ignore both "\n")
    "THIS IS THE TORERO BUILD FOLDER: ${PROJECT_BINARY_DIR} HERE... HERE..."
    "\n\n${ColourReset}" # Restarting color back to normal (ignore both "\n")
  )

endif(NOT TARGET ${TORERO_LIB})
