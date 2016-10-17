#include <omega.h>
#include <omegaGl.h>

#include <iostream>
#include <list>
#include <string>

#include "PointCloud.h"

using namespace omega;
using namespace std;

class OPotreeRenderPass: public RenderPass
{
public:
	OPotreeRenderPass(Renderer* client): RenderPass(client, "OPotreeRenderPass") {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	PointCloud* pointcloud;
	int framecount;
};

class OPotreeApplication: public EngineModule
{
public:
	OPotreeApplication(): EngineModule("OPotreeApplication") {}

	virtual void initializeRenderer(Renderer* r)  { 
		r->addRenderPass(new OPotreeRenderPass(r));
	}

	virtual void initialize() {
		Option option;
		Utils::loadOption("opotree.json", option);
		Camera* cam = getEngine()->getDefaultCamera();
		cam->getController()->setSpeed(option.cameraSpeed);
		cam->getController()->setFreeFlyEnabled(true);
		cam->setNearFarZ(1.0, 10000.0);

		if(option.cameraUpdatePosOri) {
			cam->setPosition(Vector3f(option.cameraPosition[0], option.cameraPosition[1], option.cameraPosition[2]));
			cam->setOrientation(Quaternion(option.cameraOrientation[0], option.cameraOrientation[1], 
											option.cameraOrientation[2], option.cameraOrientation[3]));
		}	
	}

	virtual void handleEvent(const Event& evt) {
        if(evt.getServiceType() == Service::Keyboard) {
            if(evt.isKeyDown('i')) {
            	if(!SystemManager::instance()->isMaster())
            		return;
            	Camera* cam = getEngine()->getDefaultCamera();
            	Vector3f cp = cam->getPosition();
				Quaternion q = cam->getOrientation();
				cout << "pos: " << cp[0] << " " << cp[1] << " " << cp[2] << " ";
				cout << "orientation: " << q.w() << " " << q.x() << " " << q.y() << " " << q.z() << endl;
            }
        }
    }
	
};


int main(int argc, char** argv)
{
    Application<OPotreeApplication> app("OPotree");
    return omain(app, argc, argv);
}


void OPotreeRenderPass::initialize()
{
	RenderPass::initialize();

	// Initialize
	pointcloud = new PointCloud("opotree.json", SystemManager::instance()->isMaster());
	
	//graphics
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	framecount = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OPotreeRenderPass::render(Renderer* client, const DrawContext& context)
{
	if(context.task == DrawContext::SceneDrawTask)
	{
		client->getRenderer()->beginDraw3D(context);
		if(oglError) return;

		// Enable depth testing and lighting.
		glEnable(GL_DEPTH_TEST);
		if(oglError) return;
		glEnable(GL_LIGHTING);
		if(oglError) return;
	
		// Test and draw
		// get camera location in world coordinate

		Vector3f cp = context.camera->getPosition();
		float campos[3] = {cp[0], cp[1], cp[2]};

		float* MVP = (context.projection*context.modelview).cast<float>().data();
		pointcloud->updateVisibility(MVP, campos);
		pointcloud->draw(MVP);

		framecount++;
		if(framecount > 500) {
			if(SystemManager::instance()->isMaster()) {
				Quaternion q = context.camera->getOrientation();
				cout << "Camara pos: " << cp[0] << " " << cp[1] << " " << cp[2] << " ";
				cout << "orientation: " << q.w() << " " << q.x() << " " << q.y() << " " << q.z() << endl;
			}
			framecount = 0;
		}   		

		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}
