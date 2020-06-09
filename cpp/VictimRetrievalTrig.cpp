/*
 * VictimRetrievalTrig.cpp:
 *
 * Date: June 8, 2020
 * Author: Victor Hu and Jeffrey Cheng
 * Facilitator: Storming Robots
 * Version: 1.0 initial release
 * Description:
 * Sample code to demonstrate how to perform victim recognition and retrieval using the camera and basic trigonometry.
 * The purpose of this sample is to serve as a starting point to learn from. It does not exhibit a fully functioning robot.
 *
 * Note that the doTurn() function and the trigonometric calculations are limited to clockwise motion in this sample. Additionally,
 * this sample does not distinguish between multiple victims in a single camera frame. For the purpose of learning, you may try expanding
 * on this sample by figuring out how to implement counterclockwise rotation as well as filter victims based off of other characteristics,
 * e.g., the closest victim.
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

double angle = 0, vicAngle, vicDist, initAngle = 0, setAngle = 0, targetEnc = 0;

// The recognition colors of victims is (0, 0, 0)
double victimColor[] = { 0,0,0 };

// Robot state begins in 0. The state variable keeps track of whether the robot is scanning for victims,
// turning, or moving forward.
int state = 0;

/*
 * Number of radians the motors must turn to move the robot forward 1 meter.
 * This is found by taking 100cm (1m) and dividing it by pi * 19cm, the
 * cirumference of the wheel of the pioneer3DX robot found at
 * https://cyberbotics.com/doc/guide/pioneer-3dx which yields wheel revolutions per meter.
 * Then multiply by 2pi to convert to radians per meter.
 */
const double radPerMeter = 10.5;

// Converts the current angle from radians into degrees.
// 180 degrees = pi radians
double getAngle() {

	return angle * 180 / 3.1415;
}

/*
 * Webots gyros return angular velocity along the 3 axes. To find angle, we need to integrate
 * angular velocity by multiplying by the timeStep. We only care about rotation around the y-axis AKA yaw.
 * The result is stored in the variable "angle" in radians.
 */
void updateGyro(int timeStep) {
	angle += (timeStep / 1000.0) * (gyro->getValues())[1];
}

// This utility function checks if two arrays arr1 and arr2 are equal to each other for numElem number of elements.
// If they are equal, then return true, otherwise return false.
bool arrEquals(double *arr1, double *arr2, int numElem) {
	for (int x = 0; x < numElem; x++)
		if (arr1[x] != arr2[x])
			return false;
	return true;
}

// Makes the robot go forward for a setpoint of "dist" meters.
void goForward(double dist) {

	// Calculates the target encoder value by converting "dist" into radians
	// and adding it onto the current encoder value.
	targetEnc = leftEnc->getValue() + radPerMeter * dist;

	// Sets the left and right motors to move forward at 5 rad/s
	leftMotor->setVelocity(5);
	rightMotor->setVelocity(5);
}

// Makes the robot do a clockwise point turn for "ang" degrees
void doTurn(double ang) {

	// Updates initAngle to be the current angle (in degrees).
	initAngle = getAngle();

	// Sets setAngle to be equal to the desired turn angle
	setAngle = ang;

	// Sets the left motor to move forward at 1 rad/s
	// Sets the right motor to move backward at 1 rad/s
	leftMotor->setVelocity(1);
	rightMotor->setVelocity(-1);
}

// Function that checks if the robot is in the process of moving forward.
// true -> still moving forward
// false -> robot has finished
bool ifForward() {

	// If the current encoder value is within .1 radians of the target encoder value
	if (abs(leftEnc->getValue() - targetEnc) < .1) {

		// Stop the robot and return false
		leftMotor->setVelocity(0);
		rightMotor->setVelocity(0);
		return false;
	}

	// Otherwise, return true
	return true;
}

// Function that checks if the robot is in the process of turning the degrees specified by setAngle
// true -> still turning
// false -> robot has finished
bool ifTurn() {

	if (abs(getAngle() - initAngle) >= setAngle) {

		// Stop the robot and return false
		leftMotor->setVelocity(0);
		rightMotor->setVelocity(0);
		return false;
	}

	// Otherwise, return true
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

	// Also enable camera recognition
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

		// Update the angle heading by integrating the gyro's current reading
		updateGyro(timeStep);

		// The robot starts in state 0. In this state, the robot is searching for a victim to retrieve.
		if (state == 0) {

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

					// Get the victim's position
					const double* vicPos = objects[i].position;

					// Calculate the angle that we need to turn to face the victim using trig. Note that we use
					// vicPos[0] and vicPos[2] because that refers to the x and z positions, respectively. The x and z
					// axes make up the two horizontal axes. We do not care about the y axis, which is the vertical axis.
					// convert the result of atan() from radians into degrees.
					vicAngle = atan(abs(vicPos[0]) / abs(vicPos[2])) / 3.1415 * 180;

					// Use the pythagorean theorm to calculate the distance to the victim. Subtract .3 meters to account for the
					// radius of the victim object itself.
					vicDist = sqrt(vicPos[2] * vicPos[2] + vicPos[0] * vicPos[0]) - 0.3;

					cout << vicAngle << endl;

					// Transition into state 1 and begin turning.
					state = 1;
					doTurn(vicAngle);
				}
			}
		}

		// If we are in state 1 (the turn state) and ifTurn() tells us we are finished turning, then transition to the moving forward state
		else if (state == 1 && !ifTurn()) {

			cout << "moving forward" << endl;

			// Go forward the distance we calculated to the victim
			goForward(vicDist);
			// Transition to state 2
			state = 2;
		}

		// If we are in state 2 and ifForward() tells us we are finished moving forward, then we are done
		else if (state == 2 && !ifForward()) {

			cout << "done" << endl;

			// Stop the motors
			leftMotor->setVelocity(0);
			rightMotor->setVelocity(0);
		}
	}

	return 0;
}