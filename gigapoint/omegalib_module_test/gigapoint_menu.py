import gigapoint
from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *
import json

config_filename = "gigapoint_resource/config/gigapoint_century_local.json"

# pointcloud
gp = gigapoint.initialize()
gp.initPotree(config_filename)

#control
with open(config_filename) as json_file:    
    config = json.load(json_file)

cam = getDefaultCamera()

# menu
def updateMaterial(material):
	print material
	b11.getButton().setChecked(False)
	b12.getButton().setChecked(False)
	if material == "rgb":
		b11.getButton().setChecked(True)
	else:
		b12.getButton().setChecked(True)
	gp.updateMaterial(material)

def updateQuality(quality):
	print quality
	b21.getButton().setChecked(False)
	b22.getButton().setChecked(False)
	if quality == "square":
		b21.getButton().setChecked(True)
	else:
		b22.getButton().setChecked(True)
	gp.updateQuality(quality)

def updateSizeType(type):
	print type
	b31.getButton().setChecked(False)
	b32.getButton().setChecked(False)
	if type == "fixed":
		b31.getButton().setChecked(True)
	else:
		b32.getButton().setChecked(True)
	gp.updateSizeType(type)

def updatePointScale(value):
	global pscale_min
	global pscale_max
	val = ((float(value) / 100) * (pscale_max - pscale_min) + pscale_min) / 100
	lscale.setText('Point scale: ' + str(val))
	gp.updatePointScale(val)


mm = MenuManager.createAndInitialize()
menu = mm.getMainMenu()
mm.setMainMenu(menu)
campos = config["cameraPosition"]
camori = config["cameraOrientation"]
cmd = 'cam.setPosition(Vector3(' + str(campos[0]) + ',' + str(campos[1]) + ',' + str(campos[2]) + ')),' + \
		'cam.setOrientation(Quaternion(' + str(camori[0]) + ',' + str(camori[1]) + ',' + str(camori[2]) + ',' + str(camori[3]) + '))'
menu.addButton("Go to cameara 1", cmd)

l1 = menu.addLabel("Material")
#l1.getWidget().setStyleValue('border-top', '1 white')
b11 = menu.addButton("rgb", "updateMaterial('rgb')")
b12 = menu.addButton("elevation", "updateMaterial('elevation')")
b11.getButton().setCheckable(True)
b12.getButton().setCheckable(True)
updateMaterial(str(config["material"]))

l2 = menu.addLabel("Quality")
#l2.getWidget().setStyleValue('border-top', '1 white')
b21 = menu.addButton("square", "updateQuality('square')")
b22 = menu.addButton("circle", "updateQuality('circle')")
b21.getButton().setCheckable(True)
b22.getButton().setCheckable(True)
updateQuality(str(config["quality"]))

l3 = menu.addLabel("Size type")
#l2.getWidget().setStyleValue('border-top', '1 white')
b31 = menu.addButton("fixed", "updateSizeType('fixed')")
b32 = menu.addButton("adaptive", "updateSizeType('adaptive')")
b31.getButton().setCheckable(True)
b32.getButton().setCheckable(True)
updateSizeType(str(config["sizeType"]))

lscale = menu.addLabel("Point scale")
#l3.getWidget().setStyleValue('border-top', '1 white')
pscale = config["pointScale"]
pscale_value = int(100*float(pscale[0]))
pscale_min = int(100*float(pscale[1]))
pscale_max = int(100*float(pscale[2]))
val = int( float(pscale_value - pscale_min) / (pscale_max-pscale_min) * 100 )
pointscale = menu.addSlider(100, "updatePointScale(%value%)")
pointscale.getSlider().setValue(val)
pointscale.getWidget().setWidth(200)
updatePointScale(val)