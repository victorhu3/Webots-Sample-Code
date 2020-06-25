#include <cstring>
using namespace std;

#include <webots/Robot.hpp>
#include <webots/Emitter.hpp>
#include <webots/GPS.hpp>
#include <webots/LightSensor.hpp>

using namespace webots;

int main(int argc, char **argv) {
  // create the Robot instance.
  Robot *robot = new Robot();
  
  // The ID of the robot is robot1
  const int robotID = 0;
  
  /*
   * This character array will be used to send our message to the scoring engine
   * when we find a victim. It is 13 bytes long because it follows the pattern:
   * int, int, int, char. The first integer is the robotID. the second and third
   * integers are the X and Z position of the predicted victim. The character at
   * the end denotes the type of victim.
   */
  char message[13];
  
  // Copy the robotID into the first 4 bytes of the array using memcpy.
  memcpy(&message[0], &robotID, 4);
  
  Emitter *emitter = robot->getEmitter("emitter");
  GPS *gps = robot->getGPS("gps");
  LightSensor *leftHeat = robot->getLightSensor("left_heat_sensor");

  // get the time step of the current world.
  int timeStep = (int)robot->getBasicTimeStep();

  gps->enable(timeStep);
  leftHeat->enable(timeStep);

  while (robot->step(timeStep) != -1) {

    /*
     * Get the position of the robot using the GPS sensor. The scoring engine
     * is expecting the position to be given in centimeters as an integer, so multiply
     * by 100 to convert meters to centimeters.
     */
    int PosX = gps->getValues()[0]*100, PosZ = gps->getValues()[2]*100;

    /*
     * If the robot sees a heat victim on the left side, send a message to the scoring
     * engine using the emitter. The message must be in the form of a character array 
     * containing robotID, XPos, ZPos, VictimType. 
     */
    if(leftHeat->getValue() > 35) {

      // Pack the message array with our X pos and Z pos using memcpy.
      // Both positions are integers, so they take up 4 bytes each.
      memcpy(&message[4], &PosX, 4);
      memcpy(&message[8], &PosZ, 4);
      
      /*
       * Possible values for victims are:
       * 'T' = temperature victim
       * 'H' = H letter
       * 'S' = S letter
       * 'U' = U letter
       */
      message[12] = 'T';
      
      // Send the packed message with a length of 13 bytes
      emitter->send(message, 13);
    }
    
    /*
     * Send a message to the scoring engine when the robot returns home,
     * once the robot is finished exploring the maze.
     * This will allow the robot to receive an exit-bonus. 
     */
    //memcpy(&message[4], &PosX, 4);
    //memcpy(&message[8], &PosZ, 4);
    //message[12] = 'E';
    //emitter->send(message, 13);
  };

  delete robot;
  return 0;
}
