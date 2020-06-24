#include <iostream>
#include <string>
using namespace std;

#include <webots/Robot.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Gyro.hpp>
#include <webots/GPS.hpp>
#include <webots/LightSensor.hpp>

using namespace webots;

// create the Robot instance.
Robot *robot = new Robot();
  
// Create an array of pointers to 8 infrared proximity sensors
DistanceSensor *infrared[8];
  
// Create the pointers for all the other sensors
Gyro *gyro = robot->getGyro("gyro");
GPS *gps = robot->getGPS("gps");
  
// Heat sensors and heat victims are modeled by light sensors and light sources
LightSensor *leftHeat = robot->getLightSensor("left_heat_sensor");
LightSensor *rightHeat = robot->getLightSensor("right_heat_sensor");

// Variable used to store our heading.
double angle = 0;

/*
 * Webots gyros return angular velocity along the 3 axes. To find angle, we need to integrate
 * angular velocity by multiplying by the timeStep. We only care about rotation around the y-axis AKA yaw.
 * The result is stored in the variable "angle" in radians.
 */
void updateGyro(int timeStep) {
    angle += (timeStep / 1000.0) * (gyro->getValues())[1];
}

// Converts the current angle from radians into degrees.
// 180 degrees = pi radians
double getAngle() {
    return angle * 180 / 3.1415;
}

int main(int argc, char **argv) {

  int timeStep = (int)robot->getBasicTimeStep();

  // Loop through to get all 8 distance sensors and enable them
  for(int i = 0; i < 8; i++) {
  
    infrared[i] = robot->getDistanceSensor("ps" + to_string(i));
    infrared[i]->enable(timeStep);
  }
  
  // Enable all the other sensors
  gyro->enable(timeStep);
  gps->enable(timeStep);
  leftHeat->enable(timeStep);
  rightHeat->enable(timeStep);

  while (robot->step(timeStep) != -1) {

    /*
     * Uncomment each block of code to get an idea of what the sensor does.
     */
     
    /*
     * The IR proximity sensors on this robot return a value between 0-4095.
     * The returned value depends on the distance away from a wall or obstacle and
     * increases the shorter the distance is. IR sensors may also be affected by
     * ambient light. The mapping between sensor readings and distance can be found
     * on the Webots documentation page: https://cyberbotics.com/doc/guide/epuck
     */
     
    cout << "Proximity sensors: " << infrared[0]->getValue() << " " << infrared[2]->getValue() << " " << infrared[4]->getValue() << " " << infrared[6]->getValue() << endl;
  
    /* Webots gyroscopes function like real life gyros, returning angular velocity instead of
     * directly returning angle heading. Therefore, we need to update our angle heading manually
     * by integrating the angular velocity at each timestep using the updateGyro(timeStep) function.
     * The units for the gyroscope is in radians per second. 
     */
     
    //Must call updateGyro() to update heading!
    //updateGyro(timeStep);
    //cout << "Angular velocity: " << gyro->getValues()[1] << " Angle heading:" << getAngle() << endl;
  
    /*
     * The GPS sensor gives you your position relative to the world origin. This is the exact same
     * position as the transform of the robot. It returns 3 values for the X, Y, and Z position.
     */
    
    //cout << "X: " << gps->getValues()[0] << " Y: " << gps->getValues()[1] << " Z: " << gps->getValues()[2] << endl;
  
    /*
     * The heat sensors and heat victims are simulated using light sensors and light sources.
     * They are configured to return temperature in degrees celsius. Ambient world temperature
     * will be around 20C-25C, while right next to a heat victim will be around 37C. There is a 
     * left and right heat sensor for each robot.
     */
     //cout << "Left heat sensor: " << leftHeat->getValue() << " Right heat sensor: " << rightHeat->getValue() << endl;
     
  };

  delete robot;
  return 0;
}
