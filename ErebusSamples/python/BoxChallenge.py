from controller import Robot
from controller import Gyro

#function definitions
'''
Updates angle variable according to angular velocity from gyro
AngleCurrent = anglePast + integral of angular velocity over one timeStep since last updated angle
Should be called every time main loop repeats
'''
def updateGyro():
    global angle
    angle += (timeStep / 1000.0) * (gyro.getValues())[1]

'''
Returns current angle of robot relative to starting angle
Angle does not drop to 0 after exceeding 360
Angle % 360 will yield relative angle with maximum 360
'''
def getAngle():
    return angle * 180 / 3.1415

'''
Sets motors to go forward dist meters
'''
def goForward(leftMotor, rightMotor, dist):
    global state, targetEnc
    if encoders.getValue() == encoders.getValue():  #checks if encoder value is not naN (enocder = naN before motor first moves)
        targetEnc = encoders.getValue() + radPerMeter * dist
    else:
        targetEnc = radPerMeter * dist
    state = forward
    leftMotor.setVelocity(maxVelocity * 0.5)
    rightMotor.setVelocity(maxVelocity * 0.5)

'''
Checks if encoders have not reached the target amount of encoders (targetEnc) while going forward
Stops motors and returns false if reached
Returns true otherwise
'''
def ifForward(leftMotor, rightMotor):
    if encoders.getValue() - targetEnc > -0.01:
        leftMotor.setVelocity(0)
        rightMotor.setVelocity(0)
        return False
    return True

'''
Sets motors to turn
Notes starting angle so ifTurn90() can find degrees turned so far
'''
def doTurn(leftMotor, rightMotor):
    global state, initAngle
    initAngle = round(getAngle() / 90.0) * 90.0     #Saves angle before turn rounded to the neared 90 degrees
    state = turn
    leftMotor.setVelocity(maxVelocity * 0.1)        #10% of maxVelocity or else overshoots - change ifTurn90() to help overshoot
    rightMotor.setVelocity(-maxVelocity * 0.1)

'''
Checks compass angle to see if robot must turn more to complete 90 degree turn
Stops motors and returns false if reached
Returns true otherwise
'''
def ifTurn90():
    if abs(getAngle() - initAngle) >= 86.0:     #Increase 86.0 to help undershoot, decrease for overshoot
        leftMotor.setVelocity(0)
        rightMotor.setVelocity(0)
        return False
    return True

#variable definitions
timeStep = 32           ''' timeStep is the interval in milliseconds between iterations of the
                        main Webots loop  (while myRobot.step(timeStep) != -1)
                        Code inside of main loop should not take longer than timeStep milliseconds '''
targetEnc = 0           ''' target amount of radians for wheel to spin when robot goes forward
                            robot position sensor (encoder counter) cannot be reset, so always add to targetEnc '''
numEdgesTravel = 0      #Increases every time robot travels a side of the box 
angle = 0               #Stores current angle of robot relative to start - updated by updateGyro
initAngle = 0           #Stores starting angle of a turn so ifTurn90() knows when 90 degrees has been turned
radPerMeter = 10.5      #Converts target distance in meters into radians the wheel must spin to travel that distance


state = 0               ''' Webots robot controller require that the main loop (while myRobot.step(timeStep) != -1)
                            run once every timeStep milliseconds -> cannot run commands that take 
                            time sequentially e.g. motor movements (go forward, turn, etc.)
                            Use state variable to store what movement is currently being executed so
                            main loop knows when to change movements and which movement to change to
                            e.g. if state == forward (value of 1, see below), main loop should check if
                            encoder target is reached and begin turning if it is, upon which state changed to turn '''
                            
forward = 1             '''state values for specific motor movements'''
turn = 2                '''e.g. state = 1 if robot is currently moving forward'''

'''
Robot, motors, and all sensors (devices) need to be represented by objects
Use get____ method, passing the name of the device as a parameter, to get objects
Use the objects to call methods related to that device
'''
myRobot = Robot()
leftMotor = myRobot.getMotor('left wheel')
rightMotor = myRobot.getMotor('right wheel')
encoders = leftMotor.getPositionSensor()         '''Position sensor stores radians turned since beginning of the program
                                                    Counter cannot be reset, so targetEnc must always be updated 
                                                    relative to cummulative radians turned'''
gyro = myRobot.getGyro('gyro')                  '''Gyro sensor returns angular velocity in 3 directions, must integrate 
                                                    for true angle updateGyro() function provided to update angle 
                                                    variable to horizontal compass value'''

'''
Initialize motors and sensors
timeStep must be passed to sensor.enable to show how often probing will occur
'''
leftMotor.setPosition(float("inf"))     '''setPosition sets motors to turn a target radians that you pass in'''
rightMotor.setPosition(float("inf"))    '''Motors will not move once target is reached, no matter what velocity is set to
                                            Pass it infinity so motors will never reach target and stop automatically,
                                            so now motors are controlled by only setVelocity.
                                            We can still change the target position by calling setPosition later in the program'''
leftMotor.setVelocity(0.0)              #Sets motors to rotate at certain speed in radians per second (max velocity = 10)
rightMotor.setVelocity(0.0)
gyro.enable(timeStep)
encoders.enable(timeStep)
maxVelocity = leftMotor.getMaxVelocity()

#main code
''' 
Take note of state variable
    Code must check current state and also if it is done with that state
    If so, sends new commands to motor and switches state appropriately
    Necessary to deal with the main loop that must iterate every 32 ms 
'''
state = forward
goForward(leftMotor, rightMotor, 2.0)
while myRobot.step(timeStep) != -1:     #main loop - runs every 32 ms (because timeStep = 32)
    updateGyro()
    if state == forward and not ifForward(leftMotor, rightMotor):   #if robot is finished going forward
        doTurn(leftMotor, rightMotor)
    elif state == turn and not ifTurn90():      #if robot is finished turning
        if numEdgesTravel < 3:
            numEdgesTravel += 1
            goForward(leftMotor, rightMotor, 2.0)
        