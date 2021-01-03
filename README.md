# OCV_CameraCalibration

Calibration of cameras via Chessboard pattern images.

Created for Bachelor's Degree Final Project in Industrial Technologies Engineering, at Universidad de Zaragoza, of title "ORBSLAM Performance Evaluation in Medical Endoscope Sequences".

Enhanced for Bachelor's Degree Final Project in Mechanical Engineering, at Universidad de Zaragoza, of title "Deformable Loop_Closure Detection in Medical Endoscope Sequences".

Input: minimum of 6 frames from a calibration pattern (chessboard).
Output: camera matrix and distortion coefficients.

HowTo:

 1 - Clone and compile:

    git clone https://github.com/icirauqui/OCV_CameraCalibration.git OCV_CameraCalibration
    
    cd OCV_CameraCalibration
    mkdir build
    cd build
    cmake ..
    make

 2 - Usage:
 
   Place a minimum of 6 pictures within folder "inputFrames" (an example set is provided).
 
   Run the program: 
   
    .calibration
 
   Follow the instructions in the console.
