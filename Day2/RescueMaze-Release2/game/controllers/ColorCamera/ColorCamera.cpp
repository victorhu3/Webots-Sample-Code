/* The color camera is a camera acting as a makeshift color sensor.
   It points straight down and the camera image is made of only one pixel,
   just like any normal color sensor.
   This code probes the color camera and displays the rgb value it sees.
   */

#include <webots/Robot.hpp>
#include <webots/Camera.hpp>
#include <iostream>

using namespace webots;
using namespace std;

int main(int argc, char **argv) {
  
  Robot *robot = new Robot();

  Camera *colorCam = robot->getCamera("colour_sensor");  //Get pointer to camera

  int timeStep = (int)robot->getBasicTimeStep();
  colorCam->enable(timeStep);   //use regular enable, not recognitionEnable because want image pixels

  while (robot->step(timeStep) != -1) {
      const unsigned char* img = colorCam->getImage();  //pointer to one pixel, represented by four bytes: r, g, b, and a
      cout << "R: " << (int)(img[0]) << " G: " << (int)(img[1]) << " B: " << (int)(img[2]) << endl;
  };

  delete robot;
  return 0;
}
