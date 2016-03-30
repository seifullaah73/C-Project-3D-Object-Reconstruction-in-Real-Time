MSc Individual Project: 3D Object Reconstruction in Real Time using C++
-----------------------------------------------------------------------------------------------------------------------------

The task was to create a 3D reconstruction software in real time using C++ and openGL to render it, it captures an image via a
webcam and coverts it into its digital equivalent.

NOTE: This was completed during the course but some errors have crept up and will need debugging and some polishing. 
But the code is as it was when completed.

Requirements: 
----------------------------------
1. Calibration chart (a dark green surface with 4 light green circles in each corner)
2. an object 
3. green background
4. webcam
5. opengl and glut library files may be required

Instructions:
----------------------------------
Place the object in the middle of the chart, which should be placed on a green coloured surface in front of a green coloured 
background.

Once the program has opened, a terminal window will appear for some input from the user.

The user enters the number of corners to detect, which must be greater than four, preferrably 5.

Next the user is prompted to enter the distance between the two marked corners in both the x and y direction as well as the 
height of the object in mm.

Also a note is included, stating to press ‘r’ when ready to render, which should be pressed once you see a silhoette of the 
object and no other artefacts i.e. parts of the sheet or green screen.

This will then display the 3D temporary visual, displaying the current state of the visual hull, so as to help the user 
identify which parts have not been constructed yet.

The user should then move the camera around the object, with the chart still in sight without covering the circles in the 
corner, the smaller the movements the more accurate the constructed 3D model

Once satisfied with the temporary visual appearance of the visual hull, the user has to press ‘esc’, which will close the
current program that is running and display the fully rendered digital image.
