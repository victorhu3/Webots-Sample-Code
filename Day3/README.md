# Day3 Worlds and Samples

### Worlds

Day three uses a world very similar to the day two world, except with more heat victims added in. This world can be used for testing in preparation for the mini-challenge on Saturday.

**day3map.wbt:** The world configuration file containing an updated version of the day two maze. Please put this file in the same folder as **day2map.wbt** under **Day2/RescueMaze-Release2/game/worlds** so that the world will have access to all the assets that it needs. Otherwise the world will not load.

### Samples

**emitterDemo.cpp:** Sample controller that demonstrates how to report detected victims to the scoring engine in order to be scored. Also demonstrates how to report robot exit in order to receive the exit bonus once the robot is done traversing the maze.
