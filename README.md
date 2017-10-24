
# 1 Getting started

## 1.1 Installing Boost

Boost libraries must be installed. To install them, use the following commands in ubuntu terminal:

```
sudo apt-get install libboost-all-dev
sudo apt-get install aptitude
aptitude search boost
```

## 1.2 Installing Eigen

It is necessary to install Eigen 3 version 3.3.4 or change the version at CMakeLists.txt in the main folder of "Torero":

### 1.2.1 Changing the Eigen version

1. Open the file CMakeLists.txt located in the "Torero" main folder
2. Go to the line
   ```CMake
   find_package(Eigen3 3.3.4 COMPONENTS Core Geometry REQUIRED)
   ```
3. Change the version of Eigen, example:
   ```CMake
   find_package(Eigen2 2.1.1 COMPONENTS Core Geometry REQUIRED)
   ```
4. Don't forget to change also the lines (if necessary):
   ```CMake
   include_directories(${EIGEN3_INCLUDE_DIR})

   ...

   Eigen3::Eigen
   ```
5. Change all the Eigen3 `#includes` inside the project (if is necessary)
   ```C++
   #include "Eigen/Core"
   #include "Eigen/Geometry"
   ```

### 1.2.2 Downloading and installation of Eigen

1. Download eigen from [http://eigen.tuxfamily.org](http://eigen.tuxfamily.org)
2. Unzip the file
3. Open that folder and create inside a folder called "build"
   - Ubuntu terminal:
     ```
     cd <eigen_folder>
     mkdir buiild
     ```
4. Open the "build" folder
   - Ubuntu terminal:
     ```
     cd build
     ```
5. Execute cmake and compile
   - Ubuntu terminal:
     ```
     cmake ..
     make
     ```
6. Install
   - Ubuntu terminal:
     ```
     sudo make install
     ```
7. Clean unused files
   - Ubuntu terminal:
     ```
     make clean
     ```
8. You can delete the unzipped folder

## 1.3 Installing DevIL for image loading

For load images it is necessary to install DevIL (Developer's Image Library)

1. Download DevIL from [http://openil.sourceforge.net](http://openil.sourceforge.net)
2. Unzip the file
3. Open that folder and its child folder called "DevIL" and create inside a folder called "build"
   - Ubuntu terminal:
     ```
     cd <devil_folder>
     cd DevIL
     mkdir buiild
     ```
4. Open the "build" folder
   - Ubuntu terminal:
     ```
     cd build
     ```
5. Execute cmake and compile
   - Ubuntu terminal:
     ```
     cmake ..
     make
     ```
6. Install
   - Ubuntu terminal:
     ```
     sudo make install
     ```
7. Clean unused files
   - Ubuntu terminal:
     ```
     make clean
     ```
8. You can delete the unzipped folder

## 1.4 Install OpenGL

If is the first time using OpenGL it is necessary to install mesa and other libraries.

1. Open ubuntu terminal and introduce:
   ```
   sudo apt-get install mesa-common-dev mesa-utils mesa-utils-extra libgl1-mesa-dev libglapi-mesa xorg-dev libglu1-mesa-dev

   ```
### 1.4.1 Installation of the latest Graphics' card drivers

This visualization version works only with OpenGL 4.2 or supperior, most of new processors support it. **To increase performance** always use the graphics' card if you have one.

#### 1.4.1.1 Steps to install latest drivers

1. Go to ubuntu terminal and introduce the next command:
   ```
   sudo add-apt-repository ppa:graphics-drivers/ppa
   ```
2. Go to System __settings > Software & Updates > Additional Drivers__
3. Choose the newest available version
4. Click in the button __Apply changes__
5. Restart the computer



