#include <opencv2/opencv.hpp>
using namespace cv;

#include <webots/Robot.hpp>
#include <webots/Camera.hpp>

using namespace webots;

int main(int argc, char **argv) {
  // create the Robot instance.
  Robot *robot = new Robot();

  Camera *cam = robot->getCamera("camera");

  // get the time step of the current world.
  int timeStep = (int)robot->getBasicTimeStep();
 
  cam->enable(timeStep);
 
  while (robot->step(timeStep) != -1) {

    /*
     * Convert the camera image into an openCV Mat. You must pass in the height
     * and width of the camera in pixels, as well as specify CV_8UC4, meaning that
     * it is in RGBA format.
     */
    Mat frame(cam->getHeight(), cam->getWidth(), CV_8UC4, (void*)cam->getImage());
    
    imshow("frame", frame);
    
    //cvtColor(frame,frame,COLOR_BGR2GRAY); //grayscale
    //imshow("grayScale", frame);
    
    //threshold(frame,frame,80,255,THRESH_BINARY); //threshold
    //imshow("thresh", frame);
    
    waitKey(1);
  };

  delete robot;
  return 0;
}
