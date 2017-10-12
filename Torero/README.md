
# 1 Getting started

## 1.1 Installing Boost

Boost libraries must be installed. To install them use the following commands in ubuntu terminal:

	```
	sudo apt-get install libboost-all-dev
	sudo apt-get install aptitude
	aptitude search boost
	```

## 1.2 Installing Eigen

It is necessa to install Eigen 3 version 3.3.4 or change the version at CMakeLists.txt in the main folder of "Torero":

### 1.2.1 Changing the Eigen version

	1.Open the file CMakeLists.txt located in the "Torero" main folder
	2.Go to the line
		```
		find_package(Eigen3 3.3.4 COMPONENTS Core Geometry REQUIRED)
		```
	3.Change the version of Eigen, example:
		```
		find_package(Eigen2 2.1.1 COMPONENTS Core Geometry REQUIRED)
		```
	4.Don't forget to change also the lines:
		```
		include_directories(${EIGEN3_INCLUDE_DIR})
		...
		Eigen3::Eigen
		```
	5.Change all the Eigen3 #includes if is necessary
		```
		#include "Eigen/Core"
		#include "Eigen/Geometry"
		```

### 1.2.2 Downloading and installation of Eigen

	1.Download eigen from [http://eigen.tuxfamily.org](http://eigen.tuxfamily.org)
	2.Unzip the file
	3.Open that folder and create another folder called "build"
		-Ubuntu terminal:
		```
			cd eigen_folder
			mkdir buiild
		```
	4.Open the "build" folder
		-Ubuntu terminal:
		```
			cd build
		```
	5.Execute cmake and compile
		-Ubuntu terminal:
		```
			cmake ..
			make
		```
	6.Install
		-Ubuntu terminal:
		```
			sudo make install
		```
	7.Clean unused file
		-Ubuntu terminal:
		```
			make clean
		```
	8.You can delete the unzipped folder

## 1.3 Installing DevIl for image loading

For load images it is necessary to install DevIl (Developer's Image Library)

	1.Download DevIl from [http://openil.sourceforge.net](http://openil.sourceforge.net)
	2.Unzip the file
	3.Open that folder and its child folder called "DevIL" and create inside a folder called "build"
		-Ubuntu terminal:
		```
			cd devil_folder
			cd DevIL
			mkdir buiild
		```
	4.Open the "build" folder
		-Ubuntu terminal:
		```
			cd build
		```
	5.Execute cmake and compile
		-Ubuntu terminal:
		```
			cmake ..
			make
		```
	6.Install
		-Ubuntu terminal:
		```
			sudo make install
		```
	7.Clean unused file
		-Ubuntu terminal:
		```
			make clean
		```
	8.You can delete the unzipped folder


