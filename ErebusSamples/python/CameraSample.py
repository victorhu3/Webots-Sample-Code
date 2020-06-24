'''
Author: Jeffrey Cheng and Victor Hu
This is a python script for the Erebus world
This program will obtain a list all objects with Recognition Colors that the Camera can see
And iterate through each object until it finds one with Recognition Colors matching the 
recognitionColor list, which is set to the Recognition Color of a human
'''

from controller import Robot
from controller import Camera
import math

#returns list in string form with number rounded to 2 decimals
def roundedList(myList):
    string = "[ "
    for i in myList:
        string += str(round(i, 2)) + ", "
    string += "]"
    return string

recognitionColor = [0, 0, 0]            #Recognition Color of a human (the object we are looking for)
timeStep = 32
myRobot = Robot()
camera = myRobot.getCamera("camera")    #Creates Camera object called "camera"
                                        #Use this object to call camera related methods

camera.recognitionEnable(timeStep)      #recognitionEnable allows camera to detect Recognition Colors

while myRobot.step(timeStep) != -1:
    objects = camera.getRecognitionObjects()    #returns list of all objects in camera's view with a Recognition Color (walls, humans, etc.)
    for obj in objects:
        if obj.get_colors() == recognitionColor:    #checks if Recognition Color of obj matches Recognition Color of a human
            print("Recognized object!")
            print("Position: " + roundedList(obj.get_position()))   #obj.get_position() returns position of detected object relative to robot
            break