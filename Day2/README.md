# Day2 Worlds and Samples

### Worlds

The C++ day two world is a slightly modified version of the world used in day two of the [python tutorials](https://github.com/Shadow149/RescueMaze/wiki/Tutorial-2:-Sensing-&-Victim-Detection) with gyro sensors and a heat victim added in. (Credits to Alfred Roberts and Robbie Goldman for creation of the base world). 

**day2map.wbt:** Example rescue maze containing robots, visual victims, swamps, checkpoints, and black zones.

### Samples

Sample code is now stored inside of the Webots project itself in the **RescueMaze-Release2/game/controllers**. Therefore, all you have to do is open the world in Webots and you will have access to the sample controllers. Before running the samples, be sure to build them first.

**RescueMaze-Release2/game/controllers/Camera:** Sample code for using the camera sensor and Webots built-in camera recognition.

**RescueMaze-Release2/game/controllers/sensorSample:** Sample code demonstrating the usage of distance sensors, gyro, GPS, and the emulated heat sensors.
