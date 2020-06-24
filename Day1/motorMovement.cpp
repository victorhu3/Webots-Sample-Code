#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>

using namespace webots;

int main(int argc, char **argv) {

  /*  - Creates a robot object, two motor objects, and a position sensor object
      - Use these objects to call robot/motor related methods   
      - Pass name of motor (viewable in Webots scene tree) to the getMotor method
      - Position sensor simply returns radians turned by motor (encoder count)
        - Amount cannot be reset, always relative to beginning of program  */
  Robot *robot = new Robot();
  Motor *leftMotor = robot->getMotor("left wheel");
  Motor *rightMotor = robot->getMotor("right wheel");
  PositionSensor *leftEncoders = leftMotor->getPositionSensor();

  /*  - timeStep is the interval in milliseconds between iterations of the
      main Webots loop  (while myRobot.step(timeStep) != -1)
      - Code inside of main loop should not take longer than timeStep milliseconds
      - Default is about 32 milliseconds  */
  int timeStep = (int)robot->getBasicTimeStep();

  leftEncoders->enable(timeStep); //parameter specifies time duration between sensor probes (in milliseconds)

  /*  - Motors in Webots work off of two methods: setVelocity and setPosition
      - setVelocity sets motor to spin at given velocity (radians per second)
        - Maximum value for setVelocity is 10
      - setPosition sets target radians for motors to spin
        - If n is passed to the setPosition method, the motor will stop after spinning n radians
        - Target is always relative to motor position at beginning of program - target cannot be reset
        - leftMotor->setPosition(INFINITY) means motor will never reach target radians  */

  leftMotor->setPosition(20.0);   //Motors will stop after spinning 20 radians
  rightMotor->setPosition(20.0);
	
  leftMotor->setVelocity(10);   //Set motors to spin at 10 radians per second
  rightMotor->setVelocity(10);
  
  while (robot->step(timeStep) != -1) {
    if (leftEncoders->getValue() - leftMotor->getTargetPosition() > 0) {  //if radians turned in left motor - target radians of left motor > 0
      printf("Robot has reached target amount of radians and should have stopped\n");

      /* Uncomment code below to add 20 radians to target radians everytime motors reach their target radians
         Remember setting target radians is always relative to beginning of program */
      //leftMotor->setPosition(leftMotor->getTargetPosition() + 20.0);
      //rightMotor->setPosition(rightMotor->getTargetPosition() + 20.0);
    }
  };

  delete robot;
  return 0;
}
