# TargetLOS - Target Line Of Sight Calculator
This program demonstrates some computationally intensive calcualtions commonly performed in aviation, such as for
airplanes and multicopters both piloted and remote controlled, using CUDA and, to a lesser extent, cuBLAS.

Program Premise:
Imagine this code would be executing in real time on quadcopter with exceptional computing capability for instance.
This quadcopter would probably have a camera attached via gimbal and you'd probably want to know the location you're
looking at for surveying or collision avoidance for example.

Known Inputs:
The aircraft's location (via GPS) and orientation (via acceleromters) are known beforehand. The aircraft has Digital
Terrain Elevation Data (DTED) available as a file to model the terrain beneath.

This program demonstrates three different calculations using CUDA:
1) Matrix multiplication to create a rotational matrix for rotating the frame of reference of a LOS vector from 
  the aircraft to Earth using cuBLAS. This vector is a necessary component of later computations.
2) A Slant Range and Target Location calculation which uses CUDA parallel threads to test each point along the LOS
  vector relative to Earth's surface to find the point at which it intersects with the terrain using elevation
  data from DTED.
3) A visibility scan algorithm which uses CUDA parallel threads and dynamic parallelism (launching kernel from a
  kernel) to test each location represented by DTED for whether or not that location could be visible from the 
  aircraft's current position. (Currently does not work fully. Some instances it works and for others it doesn't)
  
Build Environment:
This project was built using Visual Studio 2019 Community edition with CUDA toolkit 10.2. The GPU device must have
Compute Capability 3.5 compatibility due the use of dynamic parallelism.  If this criteria is met, simply open the
project and build.  A compiled executable is included.

How to Use:
Make sure the executable, DTED file (.DT0) and inputs.txt are in the same directory.  You can set aircraft parameters 
in the input.txt file. Then run the executable. Be aware that if your hardware is not compatible, it could result in
system or device crash including your display driver crashing.  I recommend disabling the visibility calculation initially
by setting 'Compute_Visibility' to 0.  The program will wait after each execution alolowing you to change the input
values in the file and run the calculation again.



