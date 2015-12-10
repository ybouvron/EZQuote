### EZQuote System ### 

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
This is the parent directory for the EZQuote system. In here, you will notice
there are several subdirectories as a script. Below is a description of each
folder and the script.
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
### Description of Files###

autocrop - This is the subsystem that is responsible for the cropping out 
	the image and saving it in the images directory for the eztimate 
	subsystem to access.

dimensions.json - Contains the dimensions of the window. This will be passed
	back to the server for price calculations.

ezquote - Script that contains all run commands for the subsystems.

eztimate - This subsystem is responsible for the overlay portion as well as
	getting the dimensions of the windows. It saves the updated image in
	the images directory and outputs the dimensions to the dimensions.json 
	file.

images - Directory which contains all the images that were downloaded, cropped
	overlayed, and what was uploaded. We've implemented each step of the
	process for debugging purposes.

logs - Contains all the logs for the standard error from the screen. Again,
	this was implemented for debugging purposes. Can be disabled. 

mat_code - Contains the server part for downloading and uploading the images.
	This main script to run is ./test with the option of either upload or 
	download.

------------------------------------------------------------------------------
### Directions to Run ###

In order to run this, make sure all the executables were created in each 
subdirectory. If these weren't created for each directory from your computer, 
they will not work. Enter each subdirectory and run "cmake ." followed by 
"make". Then come back up to this directory. We are implementing a script to 
do, but it's not 100% functional yet.

Then, to run the entire system, simply run ./ezquote and you will see outputs
from each subsystem. To remove these outputs, rename the "no output" cpp files
to the normal name, re-compile the projects and run again.

Thank you for using our EZQuote system.
