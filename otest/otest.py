from cyclops import *
import random

scene = getSceneManager()

geom = ModelGeometry.create('mypoints')

for i in range(0, 100):
	geom.addVertex(Vector3(2*random.random()-1.0, 2*random.random()-1.0, 2*random.random()-1.0))
	geom.addColor(Color(random.random(), random.random(), random.random(), 1.0))

getSceneManager().addModel(geom)

points = StaticObject.create('mypoints')
geom.addPrimitive(PrimitiveType.Points, 0, 100)

pointProgram = ProgramAsset()
pointProgram.name = "mypoints"
pointProgram.vertexShaderName = "shaders/simple.vert"
pointProgram.fragmentShaderName = "shaders/simple.frag"

points.getMaterial().setProgram(pointProgram.name)
points.getMaterial().setPointSprite(True)

cam = getDefaultCamera()
cam.setPosition(Vector3(-0.11, 0.58, 3.89))
cam.setOrientation(Quaternion().new_rotate_euler(-0.010328111238777636, -1.734723475976807e-18, -0.6566406065830961))
