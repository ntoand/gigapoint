#####################################################################################################################

# (C) 2013 - Jason Leigh, Electronic Visualization Laboratory, University of Illinois at Chicago

# Version 9/1/2013

#

# Module of useful routines to make writing CAVE applications with Omegalib a little easier. The aim of the module

# is not flexibility but to enable the rapid development of applications with less code.

#

from math import *

from euclid import *

from omega import *

from cyclops import *

from omegaToolkit import *

import time



## Class consisting of static member functions to perform a number of frequently used operations in CAVE applications.

# The purpose of these functions is convenience over flexibility. There are things that almost everyone who writes a CAVE application needs to do

# and these functions are intended to save you time re-writing the same code that others have written in the past.

# Read the API for details.

class caveutil:

	__minDistance = 0

	__closestObject = None

	WAND1 = 1

	WAND2 = 2

	__wandPosition1 = Vector3(0,0,0)

	__wandPosition2 = Vector3(0,0,0)

	__wandOrientation1 = Quaternion()

	__wandOrientation2 = Quaternion()

	__headlight = None

	__light1=None

	__light2=None

	__light3=None

	__light4=None

	__light5=None

	__wand1JoystickX=0

	__wand1JoystickY=0

	__wand2JoystickX=0

	__wand2JoystickY=0

	

	@staticmethod

	## Convert feet to meters. Omegalib typically operates in meters but we in America still think in feet!

	# @param feet feet

	# @param return Returns meters

	def feet2meters(feet):

		return feet*0.305

	

	@staticmethod

	## Determine if your app is running in the CAVE or not (i.e. on your laptop in a simulator).

	# @return Returns True if in CAVE, else False

	def isCAVE():

		return getBoolSetting("config/platform","CAVE2",False)

		

	@staticmethod

	## Retrieves the head position in local coordinates.

	# @return Returns a Vector3

	def getHeadLocalPosition(currentCamera):

		return currentCamera.getHeadOffset()



	## Retrieves the head position in world coordinates.

	# @return Returns a Vector3

	@staticmethod

	def getHeadWorldPosition(currentCamera):

		return currentCamera.localToWorldPosition(currentCamera.getHeadOffset())



	## Retrieves the head orientation in local space.

	# @return Returns a Quaternion()		

	@staticmethod

	def getHeadLocalOrientation(currentCamera):

		return currentCamera.getOrientation()



	## Retrieves the head orientation in world space.

	# @return Returns a Quaternion()	

	@staticmethod

	def getHeadWorldOrientation(currentCamera):

		return currentCamera.getOrientation()*currentCamera.getHeadOrientation()

		

	@staticmethod

	## Retrieves the head ray in world space. 

	# @param currentCamera usually receives getDefaultCamera()

	# @return Returns a Vector3.

	def getHeadRay(currentCamera):

		ray = currentCamera.getOrientation()*currentCamera.getHeadOrientation()*Vector3(0,0,-1)

		#print "RAY: " + str(ray.x) + " " + str(ray.y) + " " + str(ray.z) + "\n"

		return ray



	@staticmethod

	## Place an scene object at the position of the head and offset at a distance in front of the head. Positioning is in terms of world space.

	# @param object should be of type SceneNode	

	# @param currentCamera usually receives getDefaultCamera()

	# @param offset offset distance from the front of the head.

	def positionAtHead(currentCamera, object, offset):

		object.setPosition(currentCamera.localToWorldPosition(currentCamera.getHeadOffset()) + caveutil.getHeadRay(currentCamera) * offset )



	@staticmethod

	## Orient an scene object in accordance with head orientation. Positioning is in terms of world space.

	# @param object should be of type SceneNode	

	# @param CurrentCamera is usually given getDefaultCamera()

	def orientWithHead(currentCamera, object):

		orient = currentCamera.getOrientation()*currentCamera.getHeadOrientation()

		object.setOrientation(orient)

		

	@staticmethod

	## Retrieves the wand's location in local coordinates.

	# @return Returns a Vector3.

	def getWandLocalPosition(currentCamera, wandId):

		if (wandId == caveutil.WAND1):

			return caveutil.__wandPosition1

		if (wandId == caveutil.WAND2):

			return caveutil.__wandPosition2

		print "caveutil: WAND ID: ", wandId, " unknown"



		

	@staticmethod

	## Retrieves the wand's location in world coordinates.

	# @return Returns a Vector3.	

	def getWandWorldPosition(currentCamera, wandId):

		if (wandId == caveutil.WAND1):

			return currentCamera.localToWorldPosition(caveutil.__wandPosition1)

		if (wandId == caveutil.WAND2):

			return currentCamera.localToWorldPosition(caveutil.__wandPosition2)

		print "caveutil: WAND ID: ", wandId, " unknown"



	@staticmethod

	## Retrieves the wand's orientation in local space.	

	# @return Returns a Quaternion()

	def getWandLocalOrientation(currentCamera, wandId):

		if (wandId == caveutil.WAND1):

			return caveutil.__wandOrientation1

		if (wandId == caveutil.WAND2):

			return caveutil.__wandOrientation2

		print "caveutil: WAND ID: ", wandId, " unknown"



		

	@staticmethod

	## Retrieves the wand's orientation in world space.	

	# @return Returns a Quaternion().	

	def getWandWorldOrientation(currentCamera, wandId):

		if (wandId == caveutil.WAND1):

			return currentCamera.getOrientation()* caveutil.__wandOrientation1

		if (wandId == caveutil.WAND2):

			return currentCamera.getOrientation()* caveutil.__wandOrientation2

		print "caveutil: WAND ID: ", wandId, " unknown"

		return Quaternion()

		

	@staticmethod

	## CurrentCamera is usually given getDefaultCamera()

	# @param currentCamera usually receives getDefaultCamera()

	# @param wandId is either caveutil.WAND1 or caveutil.WAND2

	# @return Returns a Vector3.	

	def getWandRay(currentCamera, wandId):

		return caveutil.getWandWorldOrientation(currentCamera, wandId)*Vector3(0,0,-1)





	@staticmethod

	## Place an scene object at the position of the wand and offset at a distance in front of the wand. Positioning is in terms of world space.

	# @param object should be of type SceneNode	

	# @param currentCamera usually receives getDefaultCamera()

	# @param offset offset distance from the front of the wand.

	# @param wandId is either caveutil.WAND1 or caveutil.WAND2

	def positionAtWand(currentCamera, object, wandId, offset):

		object.setPosition(caveutil.getWandWorldPosition(currentCamera, wandId) + caveutil.getWandRay(currentCamera, wandId) * offset)



	@staticmethod

	## Orient a scene object in accordance with wand's orientation in world space.

	# @param object should be of type SceneNode	

	# @param CurrentCamera is usually given getDefaultCamera()

	# @param wandId is either caveutil.WAND1 or caveutil.WAND2

	def orientWithWand(currentCamera, object, wandId):

		orient = caveutil.getWandWorldOrientation(currentCamera, wandId)

		object.setOrientation(orient)

		

	@staticmethod

	def __rayCallback(node, distance):

		if caveutil.__closestObject == None:

			if node != None:

				caveutil.__closestObject = node

				caveutil.__minDistance = distance

			return

			

		# save away the closest object you have intersected with

		if (node != None):

			if (distance < caveutil.__minDistance):

				caveutil.__minDistance = distance

				caveutil.__closestObject = node



	@staticmethod

	## Returns object of type SceneNode

	# NOTE: Object must have setSelectable() flag to True to be searchable

	# @param position Location as a Vector3 in world space to initiate an intersection ray. (e.g. using getWandWorldPosition())

	# @param theRay Normalized direction to fire a ray as a Vector3 in world space. (e.g. using getWandRay())

	# @return Returns the closest object (a SceneNode) and its distance. Retrieve as: obj, distance = caveutil.getNearestIntersectingObject(......)

	def getNearestIntersectingObject(position, theRay):

		caveutil.__closestObject = None

		querySceneRay(position, theRay, caveutil.__rayCallback)

		return (caveutil.__closestObject, caveutil.__minDistance)



	@staticmethod

	## Load a 3D object with one line of code. The created object is placed at the origin.

	# @param sceneManager usually given getSceneManager()

	# @param objectName String label for the object

	# @param filename Filename for the object including path relative to the running directory

	# @param billboard Force the object to always face the viewer

	# @param animated Set to True or False depending on whether the object is static or contains animation

	# @param selectable Set to True to make this object selectable via ray intersection

	# @param colored Set to  True to apply color info inherent in the file

	# @param textured Set to True to enable texture maps inherent in the file

	# @return Returns the created object.

	def loadObject(sceneManager, objectName, filename, animated = False, billboard=False, selectable=True, colored=True, textured=True):

		obj = ModelInfo()

		obj.name = objectName

		obj.path = filename

		obj.optimize = True
		
		sceneManager.loadModel(obj)



		# Create a scene object using the loaded model

		if animated == False:

			tobj = StaticObject.create(objectName)

		else:

			tobj = AnimatedObject.create(objectName)

		

		if billboard == True:

			tobj.setFacingCamera(getDefaultCamera())

		tobj.setSelectable(selectable)

		tobj.setPosition(Vector3(0, 0,0))

		if colored==True:

			tobj.setEffect("colored")

		if textured == True:

			tobj.setEffect("textured")

		return tobj

	

	## Add a simple white headlight to the scene

	# @param currentCamera Usually passed getDefaultCamera(). The light is made a child of the camera.

	@staticmethod

	def addHeadLight(currentCamera):

		if caveutil.__headlight != None:

			print "CAVEUTIL: Headlight already created.\n"

			return

			

		caveutil.__headlight = Light.create()

		caveutil.__headlight.setColor(Color("white"))

		caveutil.__headlight.setEnabled(True)

		caveutil.__headlight.setPosition(Vector3(0,1.5,2))

		currentCamera.addChild(caveutil.__headlight)

		#caveutil.__headlight.followTrackable(0)

		#headlight.followTrackable(0)

	

	## Turn headlight on or off.

	# @param bool Set to True for On and False for Off

	@staticmethod

	def enableHeadLight(bool):

		if caveutil.__headlight != None:

			caveutil.__headlight.setEnabled(bool)

		

	## Force an update of the Smart Lights. You do not normally have to do this. Smart lights are updated  by caveutil.update() whenever events arrive.

	# @param scene Scene of the world. Normally given getScene()

	@staticmethod

	def updateSmartLights(scene):

		if (caveutil.__light1 != None):

			center = scene.getBoundCenter()

			min = scene.getBoundMinimum()

			max = scene.getBoundMaximum()

			radius = scene.getBoundRadius()*2

			caveutil.__light1.setPosition(Vector3(center.x-radius,center.y+radius,center.z-radius))

			caveutil.__light2.setPosition(Vector3(center.x+radius,center.y+radius,center.z-radius))

			caveutil.__light3.setPosition(Vector3(center.x-radius,center.y+radius,center.z+radius))

			caveutil.__light4.setPosition(Vector3(center.x+radius,center.y+radius,center.z+radius))

			caveutil.__light5.setPosition(Vector3(center.x,center.y-radius,center.z))

	

	

	## Add a set of 4 lights to the scene to provide sufficient generic illumination of the world.

	# As more objects are added to the scene, the lights will automatically reposition themselves.

	# Smart lights always keeps track of the bounding radius of the entire CAVE scene and moves the lights so that

	# everything is always illuminated.

	# @param scene Normally pass it getScene()

	# @param currentCamera Normally pass it getDefaultCamera()

	@staticmethod

	def addSmartLights(scene, currentCamera):

		

		if (caveutil.__light1 != None):

			print "CAVEUTIL: Smart lights already created.\n"

			return

			

		# minimum would be set at size of CAVE2

		caveutil.__light1 = Light.create()

		caveutil.__light2 = Light.create()

		caveutil.__light3 = Light.create()

		caveutil.__light4 = Light.create()

		caveutil.__light5 = Light.create()

		caveutil.__light1.setEnabled(True)

		caveutil.__light2.setEnabled(True)

		caveutil.__light3.setEnabled(True)

		caveutil.__light4.setEnabled(True)

		caveutil.__light5.setEnabled(True)

		caveutil.__light1.setColor(Color(0.7,0.7,0.7,1))

		caveutil.__light2.setColor(Color(0.3,0.3,0.3,0.3))

		caveutil.__light3.setColor(Color(0.3,0.3,0.3,0.3))

		caveutil.__light4.setColor(Color(0.6,0.6,0.6,1))

		caveutil.__light5.setColor(Color(0.2,0.2,0.2,0.2))

		

		currentCamera.addChild(caveutil.__light1)

		currentCamera.addChild(caveutil.__light2)

		currentCamera.addChild(caveutil.__light3)

		currentCamera.addChild(caveutil.__light4)		

		currentCamera.addChild(caveutil.__light5)

		

		caveutil.updateSmartLights(scene)



	## Turn on/off Omegalib's default use of Joystick for navigation

	# @param bool Set to either True or False

	@staticmethod

	def setDefaultJoystickNavigation(bool):

		getDefaultCamera().setControllerEnabled(bool)

		

	## Turn smartlights on or off.

	# @param bool Set to True for On and False for Off

	@staticmethod

	def enableSmartLights(bool):

		if caveutil.__light1 != None:

			caveutil.__light1.setEnabled(bool)

			caveutil.__light2.setEnabled(bool)

			caveutil.__light3.setEnabled(bool)

			caveutil.__light4.setEnabled(bool)

			caveutil.__light5.setEnabled(bool)

		

	## Fit a scene object into the CAVE. Function will scale the object down to a 2 meter radius and put it at the center of the CAVE.

	# @param obj Pass a 3D scene node.

	@staticmethod

	def fitInCAVE(obj):

		center = obj.getBoundCenter()

		min = obj.getBoundMinimum()

		max = obj.getBoundMaximum()

		radius = obj.getBoundRadius()

		obj.setPosition(Vector3(-center.x*(2/radius), -center.y*(2/radius), -center.z)*(2/radius))

		#print "FIT: " + str(radius) + " ", center

		if radius != 0:

			obj.setScale(Vector3(2/radius, 2/radius, 2/radius))



	##

	# @param wandId Provide either caveutil.WAND1 or caveutil.WAND2

	# @return Returns a tuple of the wand joystick's x and y value. Usage: x,y = caveutil.getWandJoystick(caveutil.WAND1)

	@staticmethod

	def getWandJoystick(wandId):

		if wandId == caveutil.WAND1:

			return caveutil.__wand1JoystickX, caveutil.__wand1JoystickY

		if wandId == caveutil.WAND2:

			return caveutil.__wand2JoystickX, caveutil.__wand2JoystickY

		print "caveutil.getWandJoystick: WandId unknown."

		return 0,0

		

	## Place this in the CAVE's onEvent() call to update caveutil whenever events from the tracker are received.

	# @param event should be passed getEvent()

	@staticmethod

	def update(event, scene):

	

		caveutil.updateSmartLights(scene)

		if (event == None):

			return

		

		# Have to get the wand position and orientation manually by processing events

		# because Omegalib has no equivalent of getHeadOrientation() for the wand.

		if (event.getSourceId() == caveutil.WAND1):

			if (event.getServiceType() == ServiceType.Mocap):

				caveutil.__wandPosition1 = event.getPosition()

				caveutil.__wandOrientation1 = event.getOrientation()

			if event.getServiceType()== ServiceType.Wand:

				caveutil.__wand1JoystickX=event.getAxis(0)

				caveutil.__wand1JoystickY=event.getAxis(1)

		

		if (event.getSourceId() == caveutil.WAND2):

			if (event.getServiceType() == ServiceType.Mocap):

				caveutil.__wandPosition2 = event.getPosition()

				caveutil.__wandOrientation2 = event.getOrientation()	

			if event.getServiceType()== ServiceType.Wand:

				caveutil.__wand2JoystickX=event.getAxis(0)

				caveutil.__wand2JoystickY=event.getAxis(1)				



## The FlipbookActor class allows you to play back an animation sequence that is encapsulated as independent SceneNode objects.

# Begin by adding all the objects in your animation sequence to a single parent node.

# Then create a FlipbookActor object and give it your parent scene node.

# Lastly, use one of the FlipbookActor member functions to control the animation- e.g. play(), stop(), setFrameRate(), loop()

class FlipbookActor(Actor):

	__actors = []

	__rootNode = None

	__currentFrame = 0

	FORWARD = 1

	BACKWARD = -1

	STOP = 0

	__playDirection = STOP

	__playRate = 1.0/30.0 # 30 FPS

	__accumdt = 0

	__loop = True

	

	## This is stuff you normally need to do in a constructor for an Omegalib Actor

	# @param rootNode - root scene node containing children to be flipped through

	def __init__(self, rootNode):

		super(FlipbookActor, self).__init__("Flipbook")

		FlipbookActor.__actors.append(self)

		self.__rootNode = rootNode

		self.setUpdateEnabled(True)

		

	def dispose(self):

		actors.remove(self)



	## Return number of frames in animation

	def getNumFrames(self):

		if self.__rootNode != None:

			return self.__rootNode.numChildren()

		else:

			return 0

	

	## Jump to specific frame of the animation

	# @param frame Must be a number between 0 and number of frames minus 1

	def setCurrentFrame(self, frame):

		if self.__rootNode != None:

			if (frame >= 0) and (frame < self.getNumFrames()):

				self.__currentFrame = frame

				

	## Play animation

	# @param direction Set to either FlipbookActor.FORWARD, FlipbookActor.BACKWARD, or FlipbookActor.STOP

	def play(self, direction = FORWARD):

		self.__playDirection = direction

	

	## Stop an animation

	# You can also call play(FlipbookActor.STOP)

	def stop(self):

		self.playDirection(self.STOP)

	

	## Turn on or off animation looping

	# @param bool Set to either True or False

	def loop(self, bool):

		self.__loop = bool

	

	## Set the frame rate at which to play the animation. It will try its best to honor the chosen frame rate. But if your scene it can't

	# then it will play back at whatever fastest rate it can manage, so you at least don't miss seeing an animation frame.

	# i.e. it does not drop animation frames and skip ahead. 

	# @param rate Specified in frames per second.

	def setFrameRate(self, rate):

		if rate > 0:

			self.__playRate = 1.0/rate

	

	## Hide all the frames of the animation if needed

	def hideAllFrames(self):

		if self.__rootNode != None:

			for f in range(0,self.getNumFrames()):

				self.__rootNode.getChildByIndex(f).setVisible(False)

				

	## Show all the frames of the animation if needed

	def showAllFrames(self):

		if self.__rootNode != None:

			for f in range(0,self.getNumFrames()):

				self.__rootNode.getChildByIndex(f).setVisible(True)

	

	## Single step forward one frame. Note that once you call this the animation will stop playing since it is now in single step mode.

	def nextFrame(self):

		self.__playDirection = self.STOP

		self.__rootNode.getChildByIndex(self.__currentFrame).setVisible(True)

		

		self.__currentFrame = self.__currentFrame + 1



		if self.__currentFrame >= self.getNumFrames():

			if self.__loop == True:

				self.__currentFrame = 0

			else:

				self.__currentFrame = self.getNumFrames()-1

		if self.__currentFrame < 0:

			if self.__loop == True:

				self.__currentFrame = self.getNumFrames()-1

			else:

				self.__currentFrame = 0

				

	## Single step backward one frame. Note that once you call this the animation will stop playing since it is now in single step mode.	

	def previousFrame(self):

		self.__playDirection = self.STOP

		self.__rootNode.getChildByIndex(self.__currentFrame).setVisible(True)

		

		self.__currentFrame = self.__currentFrame -1



		if self.__currentFrame >= self.getNumFrames():

			if self.__loop == True:

				self.__currentFrame = 0

			else:

				self.__currentFrame = self.getNumFrames()-1

		if self.__currentFrame < 0:

			if self.__loop == True:

				self.__currentFrame = self.getNumFrames()-1

			else:

				self.__currentFrame = 0

				

	# Called on each update cycle to perform the interpolation.

	def onUpdate(self, frame, time, dt):

		self.hideAllFrames()

		self.__rootNode.getChildByIndex(self.__currentFrame).setVisible(True)

		

		self.__accumdt = self.__accumdt + dt

		if self.__accumdt >= self.__playRate:

			self.__accumdt = self.__accumdt - self.__playRate

			self.__currentFrame = self.__currentFrame + self.__playDirection



			if self.__currentFrame >= self.getNumFrames():

				if self.__loop == True:

					self.__currentFrame = 0

				else:

					self.__currentFrame = self.getNumFrames()-1

			if self.__currentFrame < 0:

				if self.__loop == True:

					self.__currentFrame = self.getNumFrames()-1

				else:

					self.__currentFrame = 0

		

## Interpolation Actor for scene nodes. It will perform linear or ease-in, ease-out interpolation of position, orientation and scale of a given node.

# To use it create an InterpolActor object and pass the Scene Node as the parameter in the constructor. Scene Nodes can include the camera (obtained from getDefaultCamera()).

# This will then let you interpolate between camera viewpoints in the CAVE- useful for creating smooth transitions between the viewpoints.

class InterpolActor(Actor):

	__actors = []

	__sceneObject = None

	__position = Vector3(0,0,0)

	__orient = Quaternion()

	__scale = Vector3(0,0,0)

	__targetPosition = Vector3(0,0,0)

	__targetScale = Vector3(0,0,0)

	__targetOrient = Quaternion()

	__startTime = 0

	__duration = 1

	__startInterp = False

	__endFunc = None

	__currentTime = 0

	

	# Types of interpolation

	LINEAR = 0

	SMOOTH = 1

	

	# Bit codes for selecting which parameters to interpolate

	POSITION = 1

	SCALE = 2

	ORIENT = 4

	__interpOper = POSITION | ORIENT | SCALE

	__interpolationType = LINEAR

	

	## Constructor for InterpolActor. Pass it the SceneNode object to interpolate.

	# @param object - a SceneNode object.

	def __init__(self, object):

		super(InterpolActor, self).__init__("InterpolActor")

		InterpolActor.__actors.append(self)

		self.__sceneObject = object

		self.setUpdateEnabled(True)

		

	def dispose(self):

		actors.remove(self)

	

	## Set the duration of the interpolation

	# @param duration is in seconds. If you do not set this then the default is 1 second.

	def setDuration(self,duration=1):

		self.__duration = duration

	

	## Choose the interpolation "curve"

	# @param interpType Use InterpolActor.LINEAR or InterpolActor.SMOOTH. Smooth uses a sine curve to ease-in and ease-out the interpolation.

	def setTransitionType(self,interpType):

		self.__interpolationType = interpType

	

	## Start performing the interpolation.

	def startInterpolation(self):

		self.__position = self.__sceneObject.getPosition()

		self.__scale = self.__sceneObject.getScale()

		self.__orient = self.__sceneObject.getOrientation()

		self.__startInterp=True

		self.__startTime = self.__currentTime

		#self.__startTime = time.clock()

		#self.setUpdateEnabled(True)

	

	## Give InterpolActor a callback function to call whenever an interpolation

	# has just completed. 

	# @param callback The callback should include only 1 parameter, which will

	# receive the InterpolActor object that just triggered the callback.

	# Every InterpolActor can have its own callback function if desired.

	def setEndOfInterpolationFunction(self, callback):

		self.__endFunc = callback

		

	# Perform the interpolation at each update cycle.

	def __interpolate(self):

	

		# The interpolator works by keeping track of elapsed time and from that determines how far along you should be in the animation.

		# This ensures that animations are updated at a rate independent of the frame rate. So if your frame rate is high you will

		# see more interpolation steps. If your frame rate is low you will see fewer.

		if (self.__startInterp == True):

			#elapsedTime = time.clock() - self.__startTime

			elapsedTime = self.__currentTime - self.__startTime

			#print "ELAPSED TIME " + str(elapsedTime) + "\n"

			if elapsedTime > self.__duration:

				elapsedTime = self.__duration

				self.__startInterp = False

				#self.setUpdateEnabled(False)

				if self.__endFunc != None:

					self.__endFunc(self)

					return

					

			# Uses a sin curve to do an ease-in and ease-out interpolation.

			if self.__interpolationType == self.SMOOTH:

				smoothFunction = sin(radians(-90+(180*elapsedTime/self.__duration)))/2.0 + 0.5

			else:

				# Otherwise the default is linear

				smoothFunction = elapsedTime/self.__duration

			

			# Check the interpOper bits to figure out which interpolation operation is desired.

			if (self.__interpOper & self.POSITION) != 0 :

				currentPosition = self.__position + (self.__targetPosition-self.__position) * smoothFunction

			if (self.__interpOper & self.ORIENT) != 0 :

				currentOrientation = Quaternion.new_interpolate( self.__orient, self.__targetOrient,  smoothFunction)

			if (self.__interpOper & self.SCALE) != 0:

				currentScale = self.__scale + (self.__targetScale-self.__scale) * smoothFunction	



			# Now assign the interpolation to the actual object being manipulated

			if (self.__sceneObject != None):

				if (self.__interpOper & self.POSITION) != 0 :

					self.__sceneObject.setPosition(currentPosition)

				if (self.__interpOper & self.ORIENT) != 0 :

					self.__sceneObject.setOrientation(currentOrientation)

				if (self.__interpOper & self.SCALE) != 0:

					self.__sceneObject.setScale(currentScale)



	## Set the ending position of the interpolation. The start point is always the current position of the object.

	# @param pos a position of type Vector3()

	def setTargetPosition(self,pos):

		self.__targetPosition = pos

		

	## Set the ending orientation of the interpolation. The start point is always the current orientation of the object.

	# @param orient a position of type Quaternion()	

	def setTargetOrientation(self, orient):

		self.__targetOrient = orient



	## Set the ending scale of the interpolation. The start point is always the current scale of the object.

	# @param scale a position of type Vector3()		

	def setTargetScale(self, scale):

		self.__targetScale = scale

	

	## Choose the interpolation operation to perform.

	# @oper Options are: InterpolActor.POSITION, InterpolActor.ORIENT, InterpolActor.SCALE. You can perform one or more of these operations

	# by using the "|" operator. E.g. InterpolActor.POSITION | InterpolActor.ORIENT means you want to interpolate both position and orientation

	# at the same time.

	def setOperation(self, oper):

		self.__interpOper = oper

	

	# Called on each update cycle to perform the interpolation.

	def onUpdate(self, frame, time, dt):

		self.__currentTime = time

		self.__interpolate()



		
