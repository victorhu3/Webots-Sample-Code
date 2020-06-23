/*
 * VictimRetrievalNoTrig.cpp:
 *
 * Date: June 8, 2020
 * Author: Victor Hu and Jeffrey Cheng
 * Facilitator: Storming Robots
 * Version: 1.0 initial release
 * Description:
 * Sample code to demonstrate how to perform victim recognition and retrieval using the camera and basic trigonometry.
 * The purpose of this sample is to serve as a starting point to learn from. It does not exhibit a fully functioning robot.
 *
 * Note this sample does not distinguish between multiple victims in a single camera frame. This may lead to odd behavior where the robot tracks multiple victims.
 * For the purpose of learning, you may try expanding on this sample by figuring out how to filter victims based off of other characteristics, e.g.,
 * the closest victim.
 *
 * License: You may use this code as you wish, provided you give credit where it's due.
 */

// Webots include statements 
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/Gyro.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Gps.hpp>
#include <webots/Camera.hpp>
#include <webots/DistanceSensor.hpp>
using namespace webots;

// C++ include statements for input/output and string manipulation
#include <iostream>
#include <string>
using namespace std;

// Instantiate new Robot object
Robot *robot = new Robot();

// Gets pointers to the motor and sensors from the robot
Motor *leftMotor = robot->getMotor("left wheel"), *rightMotor = robot->getMotor("right wheel");
Gyro *gyro = robot->getGyro("gyro");
PositionSensor *leftEnc = leftMotor->getPositionSensor(), *rightEnc = rightMotor->getPositionSensor();
GPS *gps = robot->getGPS("gps");
Camera *cam = robot->getCamera("camera");
// Create an array to store pointers to 8 ultrasonic sensors
DistanceSensor *distSensor[8];

double angle = 0;
// The recognition colors of victims is (0, 0, 0)
double victimColor[] = { 0,0,0 };

/*
 * Webots gyros return angular velocity along the 3 axes. To find angle, we need to integrate
 * angular velocity by multiplying by the timeStep. We only care about rotation around the y-axis AKA yaw.
 * The result is stored in the variable "angle" in radians.
 */
void updateGyro(int timeStep) {
	angle += (timeStep / 1000.0) * (gyro->getValues())[1]; //x, y, z
}

// This utility function checks if two arrays arr1 and arr2 are equal to each other for numElem number of elements.
// If they are equal, then return true, otherwise return false.
bool arrEquals(double *arr1, double *arr2, int numElem) {
	for (int x = 0; x < numElem; x++)
		if (arr1[x] != arr2[x])
			return false;
	return true;
}

int main() {

	/* Initialization */

	// Retrieves the basic world time step and stores it in variable "timeStep". 
	int timeStep = (int)robot->getBasicTimeStep();

	// we enable all the sensors
	// we need to pass the timeStep as the refresh rate of the sensor
	gyro->enable(timeStep);
	leftEnc->enable(timeStep);
	rightEnc->enable(timeStep);
	gps->enable(timeStep);
	cam->enable(timeStep);
	cam->recognitionEnable(timeStep);

	// Gets width, in pixels, of the camera.
	const int CAM_WIDTH = cam->getWidth();
	
	// setPosition tells the motor to run until it gets to the position we pass to it.  
	// Passing INFINITY sets the motors to velocity control mode, where the motors will continue running.
	leftMotor->setPosition(INFINITY);
	rightMotor->setPosition(INFINITY);

	// setVelocity sets the speed in rad/s.  0 means stop.
	leftMotor->setVelocity(0);
	rightMotor->setVelocity(0);

	/*
	 * Initialize and enable the 8 forward facing ultrasonic sensors. The names of the ultrasonic sensors
	 * are prefixed with "so" followed by a number from 0-15. Sensors 0-7 are the forward facing sensors,
	 * while sensors 8-15 are the backward facing sensors.
	 */
	for (int x = 0; x < 8; x++) {
		distSensor[x] = robot->getDistanceSensor("so" + to_string(x));
		distSensor[x]->enable(timeStep);
	}

	/* End of Initialization */

	/*
	 * This is the main loop of the program, where the robot calls step() and passes in the timeStep.
	 * This loop should be periodically called. Failing to call step() for an extended period of time
	 * will break the controller. Motors and sensors only update following a step() call. However, you
	 * should only be calling step() at one place in your program, inside of the condition of your main loop.
	 */
	while (robot->step(timeStep) != -1) {

		// Get the number of objects the current camera frame recognizes.
		int numObj = cam->getRecognitionNumberOfObjects();

		// Retrieve a pointer to the list of objects recognized in the current camera frame.
		const CameraRecognitionObject *objects = cam->getRecognitionObjects();

		// Iterate through the number of recognized objects.
		for (int i = 0; i < numObj; i++) {

			// Check if the current object's recognition colors is equal to the colors of a victim (0,0,0).
			// If so then we see a victim.
			if (arrEquals(objects[i].colors, victimColor, 3)) {

				cout << "See victim" << endl;

				// Gets the position of the victim. We only care about the horizontal pixel position
				// since we wish to center our robots camera on the victim, which is why we do position_on_image[0].
				int posX = objects[i].position_on_image[0];

				// If the center of the victim is to the right of the camera, turn the robot right.
				// We add 1 so that the horizontal position of the victim does not have to be perfectly in the
				// center, giving it a "buffer".
				if (posX > (CAM_WIDTH / 2) + 1) {

					// The left motor will move forward at 1 rad/s, while the right motor will move backward at 1 rad/s.
					leftMotor->setVelocity(1);
					rightMotor->setVelocity(-1);
				}

				// Otherwise, if the center of the victim is to the left of the camera, turn the robot left.
				// We similarly subtract 1 to give it a buffer. 
				else if (posX < (CAM_WIDTH / 2) - 1) {

					// The left motor will move backward at 1 rad/s, while the right motor will move forward at 1 rad/s.
					leftMotor->setVelocity(-1);
					rightMotor->setVelocity(1);

				}

				//If the victim's position on the camera frame is in the center, then move forward. Eventually, we will move close
				// enough to the victim and collect it.
				else {

					leftMotor->setVelocity(5);
					rightMotor->setVelocity(5);
				}
			}
		}
	}

	return 0;
}