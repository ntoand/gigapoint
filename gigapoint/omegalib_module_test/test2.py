import gigapoint
from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *


# pointcloud
gp = gigapoint.initialize()
gp.initPotree("gigapoint_resource/config/gigapoint_century_local.json")


# model
scene = getSceneManager()

light = Light.create()
light.setColor(Color("#505050"))
light.setAmbient(Color("#202020"))
light.setPosition(Vector3(0, 50, -5))
light.setEnabled(True)

light2 = Light.create()
light2.setColor(Color("#505030"))
light2.setPosition(Vector3(50, 0, 50))
light2.setEnabled(True)

models = []
# Queue models for loading
mi = ModelInfo()
model = 'capsule'
mi.name = model
#mi.optimize = True
mi.optimize = False
mi.path = "gigapoint_resource/testdata/capsule/capsule.obj"
scene.loadModelAsync(mi, "onModelLoaded('" + model + "')")

# Model loaded callback: create objects
def onModelLoaded(name):
    global models
    model = StaticObject.create(name)
    if(model != None):
		model.setPosition(Vector3(0, 0, -180))
		#model.setEffect("colored -g 1.0 -d red")
		model.setScale(Vector3(20, 20, 20))
		model.setVisible(True)
		models.append(model)


# menu


# move camera back a bit
#getDefaultCamera().translate(Vector3(0, 0, 20), Space.Local)
#getDefaultCamera().getController().setSpeed(10)