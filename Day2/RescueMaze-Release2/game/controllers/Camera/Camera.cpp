/* The Camera has two ways of functioning - returning matrices or using recognition colors
   * If you want the matrix (matrix of rgb pixels representing the image), you can look at
   * the Webots documentation: https://cyberbotics.com/doc/reference/camera
   * This example uses recognition colors: All objects in the Webots world can be assigned a 
   * recognition color, which is completely separate from its actual color in the world
   * It acts as a tag, so whenever the object comes into the camera's view, the camera can
   * recognize it through its recognition color
   * This robot controller searches for object with recognition color victimColor, and prints
   * out the position of the recognized object
   */

#include <webots/Robot.hpp>
#include <webots/Camera.hpp>
#include <iostream>
#include <iomanip>

using namespace webots;
using namespace std;

// This utility function checks if two arrays arr1 and arr2 are equal to each other for numElem number of elements.
// If they are equal, then return true, otherwise return false.
bool arrEquals(double *arr1, double *arr2, int numElem) {
	for (int x = 0; x < numElem; x++)
		if (arr1[x] != arr2[x])
			return false;
	return true;
}

int main(int argc, char **argv) {
  
  Robot *robot = new Robot();

  Camera *cam = robot->getCamera("camera");   // Get pointer to Camera

  int timeStep = (int)robot->getBasicTimeStep();
  double victimColor[] = { 1,1,1 };   // The recognition colors of victims are (1, 1, 1)

  cam->recognitionEnable(timeStep);   // Enables camera to detect recognition colors

  while (robot->step(timeStep) != -1) {

    // Get the number of objects the current camera frame recognizes.
    int numObj = cam->getRecognitionNumberOfObjects();

    // Retrieve a pointer to the list of objects recognized in the current camera frame.
    const CameraRecognitionObject *objects = cam->getRecognitionObjects();

    // Iterate through the number of recognized objects.
    for (int i = 0; i < numObj; i++) {

      // Check if the current object's recognition colors is equal to the colors of a victim (1,1,1).
      // If so then we see a victim.
      if (arrEquals(objects[i].colors, victimColor, 3)) {

        cout << "See victim" << endl;

        // Get the victim's position
        const double* vicPos = objects[i].position;
        cout << "Victim Position: (" << setprecision(2) << vicPos[0] << ", " 
                << vicPos[1] << ", " << vicPos[2] << ")" << endl;
      }
    }
  };

  delete robot;
  return 0;
}
