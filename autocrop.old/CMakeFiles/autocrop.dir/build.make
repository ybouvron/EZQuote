# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ybouvron/Documents/final_project/code/autocrop

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ybouvron/Documents/final_project/code/autocrop

# Include any dependencies generated for this target.
include CMakeFiles/autocrop.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/autocrop.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/autocrop.dir/flags.make

CMakeFiles/autocrop.dir/autocrop.o: CMakeFiles/autocrop.dir/flags.make
CMakeFiles/autocrop.dir/autocrop.o: autocrop.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ybouvron/Documents/final_project/code/autocrop/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/autocrop.dir/autocrop.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/autocrop.dir/autocrop.o -c /home/ybouvron/Documents/final_project/code/autocrop/autocrop.cpp

CMakeFiles/autocrop.dir/autocrop.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/autocrop.dir/autocrop.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ybouvron/Documents/final_project/code/autocrop/autocrop.cpp > CMakeFiles/autocrop.dir/autocrop.i

CMakeFiles/autocrop.dir/autocrop.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/autocrop.dir/autocrop.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ybouvron/Documents/final_project/code/autocrop/autocrop.cpp -o CMakeFiles/autocrop.dir/autocrop.s

CMakeFiles/autocrop.dir/autocrop.o.requires:
.PHONY : CMakeFiles/autocrop.dir/autocrop.o.requires

CMakeFiles/autocrop.dir/autocrop.o.provides: CMakeFiles/autocrop.dir/autocrop.o.requires
	$(MAKE) -f CMakeFiles/autocrop.dir/build.make CMakeFiles/autocrop.dir/autocrop.o.provides.build
.PHONY : CMakeFiles/autocrop.dir/autocrop.o.provides

CMakeFiles/autocrop.dir/autocrop.o.provides.build: CMakeFiles/autocrop.dir/autocrop.o

# Object files for target autocrop
autocrop_OBJECTS = \
"CMakeFiles/autocrop.dir/autocrop.o"

# External object files for target autocrop
autocrop_EXTERNAL_OBJECTS =

autocrop: CMakeFiles/autocrop.dir/autocrop.o
autocrop: CMakeFiles/autocrop.dir/build.make
autocrop: /usr/local/lib/libopencv_videostab.so.2.4.10
autocrop: /usr/local/lib/libopencv_video.so.2.4.10
autocrop: /usr/local/lib/libopencv_ts.a
autocrop: /usr/local/lib/libopencv_superres.so.2.4.10
autocrop: /usr/local/lib/libopencv_stitching.so.2.4.10
autocrop: /usr/local/lib/libopencv_photo.so.2.4.10
autocrop: /usr/local/lib/libopencv_ocl.so.2.4.10
autocrop: /usr/local/lib/libopencv_objdetect.so.2.4.10
autocrop: /usr/local/lib/libopencv_nonfree.so.2.4.10
autocrop: /usr/local/lib/libopencv_ml.so.2.4.10
autocrop: /usr/local/lib/libopencv_legacy.so.2.4.10
autocrop: /usr/local/lib/libopencv_imgproc.so.2.4.10
autocrop: /usr/local/lib/libopencv_highgui.so.2.4.10
autocrop: /usr/local/lib/libopencv_gpu.so.2.4.10
autocrop: /usr/local/lib/libopencv_flann.so.2.4.10
autocrop: /usr/local/lib/libopencv_features2d.so.2.4.10
autocrop: /usr/local/lib/libopencv_core.so.2.4.10
autocrop: /usr/local/lib/libopencv_contrib.so.2.4.10
autocrop: /usr/local/lib/libopencv_calib3d.so.2.4.10
autocrop: /usr/lib/i386-linux-gnu/libGLU.so
autocrop: /usr/lib/i386-linux-gnu/libGL.so
autocrop: /usr/lib/i386-linux-gnu/libSM.so
autocrop: /usr/lib/i386-linux-gnu/libICE.so
autocrop: /usr/lib/i386-linux-gnu/libX11.so
autocrop: /usr/lib/i386-linux-gnu/libXext.so
autocrop: /usr/local/lib/libopencv_nonfree.so.2.4.10
autocrop: /usr/local/lib/libopencv_ocl.so.2.4.10
autocrop: /usr/local/lib/libopencv_gpu.so.2.4.10
autocrop: /usr/local/lib/libopencv_photo.so.2.4.10
autocrop: /usr/local/lib/libopencv_objdetect.so.2.4.10
autocrop: /usr/local/lib/libopencv_legacy.so.2.4.10
autocrop: /usr/local/lib/libopencv_video.so.2.4.10
autocrop: /usr/local/lib/libopencv_ml.so.2.4.10
autocrop: /usr/local/lib/libopencv_calib3d.so.2.4.10
autocrop: /usr/local/lib/libopencv_features2d.so.2.4.10
autocrop: /usr/local/lib/libopencv_highgui.so.2.4.10
autocrop: /usr/local/lib/libopencv_imgproc.so.2.4.10
autocrop: /usr/local/lib/libopencv_flann.so.2.4.10
autocrop: /usr/local/lib/libopencv_core.so.2.4.10
autocrop: CMakeFiles/autocrop.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable autocrop"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/autocrop.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/autocrop.dir/build: autocrop
.PHONY : CMakeFiles/autocrop.dir/build

CMakeFiles/autocrop.dir/requires: CMakeFiles/autocrop.dir/autocrop.o.requires
.PHONY : CMakeFiles/autocrop.dir/requires

CMakeFiles/autocrop.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/autocrop.dir/cmake_clean.cmake
.PHONY : CMakeFiles/autocrop.dir/clean

CMakeFiles/autocrop.dir/depend:
	cd /home/ybouvron/Documents/final_project/code/autocrop && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ybouvron/Documents/final_project/code/autocrop /home/ybouvron/Documents/final_project/code/autocrop /home/ybouvron/Documents/final_project/code/autocrop /home/ybouvron/Documents/final_project/code/autocrop /home/ybouvron/Documents/final_project/code/autocrop/CMakeFiles/autocrop.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/autocrop.dir/depend

