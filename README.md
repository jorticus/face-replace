Face-Replace
============

This project uses the Kinect face-tracking SDK to replace a user's face with another, warping it to match the user. 
It was developed for my computer vision research project at the University of Canterbury.

![Screenshot](http://i.imgur.com/sdFM6qv.png "Screenshot")


Download
========

A stand-alone version is available here:

[face-replace.zip](http://jared.geek.nz/media/face-replace.zip) (12MB)

You'll need to download the [Microsoft Kinect SDK Runtime](http://www.microsoft.com/en-nz/download/details.aspx?id=40277) which installs the required device drivers for the Kinect.
Alternatively you can use a different depth sensor, as long as it works with OpenNI2.

Compiling
=========

I developed this project in Visual Studio 2013 (VC++12), but it should work in other versions provided you can compile the required libraries.

Before you can compile this, make sure you set up the following libraries:

- [Kinect Developer Toolkit + Kinect SDK](http://www.microsoft.com/en-nz/download/details.aspx?id=40276)
- [OpenNI2](https://github.com/OpenNI/OpenNI2)
- [Boost](http://www.boost.org/) (Just the headers)
- [OpenCV](http://opencv.org/downloads.html)
- [SFML 2.0](http://www.sfml-dev.org/download.php) ([Github](https://github.com/LaurentGomila/SFML))
 

Operation
=========

The program performs the following steps:

1. Capture RGB + Depth video frame
2. Detect head pose and face features using Kinect SDK
3. Deform the Candide-3 mesh to the given head pose and face features
4. Process the RGB + Depth frames using OpenCV
4. Draw the RGB video frame
5. Draw the texture-mapped candide-3 model in OpenGL, using a custom blend shader.

Side-note: This project uses a custom candide-3 face model instead of the Kinect SDK's internal model, 
since it's not easy to match vertices with tex coords using the internal model. 
This functionality is provided through the [WinCandide-3](http://www.bk.isy.liu.se/candide/wincandide/) project 
(all source code named 'eru' is part of this project).

![Texture Mapping](http://i.imgur.com/EOUdcLA.png "Texture Mapping")


Future Work
===========

It's probably unlikely I'll do much more on this project since I have other commitments, but here's a list of things that could be improved upon in the future:

- Write a plugin for blender that can read and write the candide-3 model, so textures can be more accurately mapped. (I'm currently using the WinCandide-3 utility to approximately map the texture)
- Add support for multiple people
- Decrease tracking latency/improve face location. (Perhaps something like meanshift/optical flow + a kalman filter?)

If anyone improves upon this project, I'm happy to accept any pull requests!
