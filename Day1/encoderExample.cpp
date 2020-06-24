#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>

using namespace webots;

int main(int argc, char **argv) {
  
  // create the Robot instance.
  Robot *robot = new Robot();
  Motor *leftMotor = robot->getMotor("left wheel");
  Motor *rightMotor = robot->getMotor("right wheel");
  PositionSensor *leftEncoders = leftMotor->getPositionSensor();
  /*
   * Number of radians the motors must turn to move the robot forward 1 meter.
   * This is found by taking 100cm (1m) and dividing it by pi * 19cm, the 
   * cirumference of the wheel of the pioneer3DX robot found at
   * https://cyberbotics.com/doc/guide/pioneer-3dx which yields wheel revolutions per meter.
   * Then multiply by 2pi to convert to radians per meter.
   */
  const double radPerMeter = 10.5;
  
  /*
   * Number of radians the motors must turn to rotate the robot by 1 degree in a point turn.
   * This is found by taking the circumference of the robot's turn radius, .381*PI, and dividing it by
   * the circumference of the weheels, .19*PI. This cancels out the PI's in order to get .381/.19
   * Convert radians to degrees by multiplying by 3.14 and dividing by 180. 
   */
  const double radPerRobotDegrees = 0.031;

  // Will store the desired encoder value
  double encoderTarget = 0;

  // get the time step of the current world.
  int timeStep = (int)robot->getBasicTimeStep();

  leftEncoders->enable(timeStep);

  leftMotor->setPosition(INFINITY);
  rightMotor->setPosition(INFINITY);
  
  // Set the target encoder value to travel two meters
  encoderTarget = radPerMeter*2;
  leftMotor->setVelocity(5);
  rightMotor->setVelocity(5);
  
  // Uncomment this block of code to set the target encoder value to travel 90 degrees.
  // Notice how the right motor has a negative velocity. This will perform a point turn (one motor
  // moves forward, one motor moves backward).
  
  encoderTarget = radPerRobotDegrees*90;
  leftMotor->setVelocity(1);
  rightMotor->setVelocity(-1);
  
  
  while (robot->step(timeStep) != -1) {

    // If the encoder value exceeds the encoder target, then stop the motors.
    if(leftEncoders->getValue() >= encoderTarget) {
    
      leftMotor->setVelocity(0);
      rightMotor->setVelocity(0);
    }
  };

  delete robot;
  return 0;
}
