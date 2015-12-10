#### Autocrop Subsystem ###
This section is responsible for cropping the image that was downloaded from the server. It corrects the angle at which the window may have been taken by and crops out just the window. 

The only output for this section is the cropped image. It is saved under the images directory which is one directory up.


Instructions needed before the system can be run:

1. Edit CMakeLists.txt to the name of .cpp file to be executed (ie "autocrop" if .cpp file is autocrop.cpp)
2. Edit "imread" function in .cpp file to contain the exact name and location of image
3. In terminal, cd to location of files and run command "cmake ." then "make"
4. Execute the newly created executable.
