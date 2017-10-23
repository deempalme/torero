
# Visualization sections:

  ### Important section

  1. #### Core

     This section is mandatory for each one of the "libraries" that may be created to separate all the elements in the visualization system. It is the GUI window that contains the OpenGL framework necessary to display any of the following elements.

  ### End of important section

  2. #### LiDar visualizer

     This displays point clouds, it could accept different data structure inputs compatible with ROS, PCL and our defined data structure.

  3. #### Object visualizer

     This displays objects suchlike boxes and cylinders (with of without arrows indicating direction)

  4. #### Trajectory visualizer

     This displays trajectories (paths or lines) width diffent colors, widths and designs.

  5. #### Ground visualizer

     This displays a matrix of boxes or squares in a 2D plane, it could be used to simulate occupancy grids.

  6. #### Camera visualizer

     This display data from images or raw data

  7. #### Model loader

     This load and displays 3D models from OBJ files and its texture images

  8. #### Map loader/installer

     This loads, installs and displays GPS maps obtained from openStreetMap, it shows streets and signals

  9. #### GUI manager

     This allows the user to create new GUI elements to show information in the screen

  10. #### Coordinate conversor

      This is a GPS coordinate conversor, it converts the latitude and longitude coordinates into a Map projection (3D coordinates). The map projection used was Azimuthal equidistant projection.

  11. #### Text visualizer

      This displays text in the screen, it has 3D coordinates

## Kitti dataset loaders

  1. #### LiDar loader

     This will load point clouds from raw data obtained from Kitti datasets

  2. #### Object loader

     This loads the objects from the file tracklet_labels.xml obtained from kitti

  3. #### IMU data loader

     This loads information such; latitude, longitude, altitude, speed, acceleration, pitch, yaw, roll, etc.


