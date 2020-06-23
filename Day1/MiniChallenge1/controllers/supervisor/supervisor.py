from controller import Supervisor
import math

timeStep = 32
markedColor = [.9, .2, .3]
unmarkedColor = [.3, .3, .9]


def setWaypointColor(waypoint, color):
	waypoint.getField("children").getMFNode(0).getField("appearance").getSFNode().getField("material").getSFNode().getField("diffuseColor").setSFColor(color)

def findDist(pos1, pos2):
	return math.sqrt((pos1[0]-pos2[0])**2 + (pos1[2]-pos2[2])**2)

supervisor = Supervisor()

waypoints = supervisor.getFromDef("WAYPOINTS").getField("children")
waypointCount = waypoints.getCount()

robot = supervisor.getFromDef("ROBOT")

print("Move to the next red marker.")

for i in range(waypointCount):
	setWaypointColor(waypoints.getMFNode(i), unmarkedColor)

currWaypoint = 0
setWaypointColor(waypoints.getMFNode(currWaypoint), markedColor)

while supervisor.step(timeStep) != -1:
	if findDist(robot.getPosition(), waypoints.getMFNode(currWaypoint).getPosition()) < .3:
		setWaypointColor(waypoints.getMFNode(currWaypoint), unmarkedColor)

		print("Waypoint " + str(currWaypoint + 1) + " visited. (" + str(currWaypoint + 1) + "/" + str(waypointCount) + ")")

		if currWaypoint + 1 == waypointCount:
			print("Finished!")
			supervisor.simulationSetMode(supervisor.SIMULATION_MODE_PAUSE)
			break

		currWaypoint+=1
		setWaypointColor(waypoints.getMFNode(currWaypoint), markedColor)

