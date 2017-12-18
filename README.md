
# Torero
*(spanish: [\[ʈɒˈɾɛɾɔ\]](https://translate.google.com/#es/de/Torero%20torea%20al%20toro%20en%20el%20toreo.). Improved visualization with no Qt's Libraries dependecy*

**Torero** is a multi-platform graphical visualizer based in OpenGL, GLSL and GLFW. It was developed performance-wise and it should achieve maximum performance peak if a multi-core processor and Graphics Card are available in your system. This current version only supports configuration through code.

There is a [wiki](https://github.com/DroidDrive/torero/wiki)!

You should read the [installation guide](https://github.com/DroidDrive/torero/wiki/Installation-guide) to setup all the necessary libraries and get your Visualizer ready to run.

Don't forget to observe which [coordinate systems](https://github.com/DroidDrive/torero/wiki/Coordinate-systems) the visualizer uses.

There are guides for each section of the API:

  * [Core](https://github.com/DroidDrive/torero/wiki/3-Core) – Main window handler, controls the program's screen where everything is displayed.
  * Point cloud manager – Handles **point clouds**, it could accept different data structure inputs compatible with ROS, PCL and onwer defined data structure.
  * Object manager – Handles objects suchlike **boxes**, **cylinders**, **squares** and **circles**.
  * 3D Models manager – Handles **three-dimensional models** created with design software, suchlike, *blender*, *SolidWorks*, etc.
