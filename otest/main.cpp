#include <omega.h>
#include <omegaGl.h>

#include <iostream>
#include <list>
#include <string>

#include "Points.h"

using namespace omega;
using namespace std;

class OPotreeRenderPass: public RenderPass
{
public:
	OPotreeRenderPass(Renderer* client): RenderPass(client, "OPotreeRenderPass") {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	Points* points;
	int framecount;
};

class OPotreeApplication: public EngineModule
{
public:
	OPotreeApplication(): EngineModule("OPotreeApplication") {}

	virtual void initialize();

	virtual void initializeRenderer(Renderer* r) 
	{ 
		r->addRenderPass(new OPotreeRenderPass(r));
	}
};

void OPotreeApplication::initialize() {
	Camera* cam = getEngine()->getDefaultCamera();
	cam->getController()->setFreeFlyEnabled(true);
	cam->setNearFarZ(0.1, 1000.0);

	cam->setPosition(Vector3f(0.14951, -0.435601, 3.82368));
	cam->setOrientation(Quaternion(0.981666, -0.179113, -0.0641332, -0.0117016));	
}

int main(int argc, char** argv)
{
    Application<OPotreeApplication> app("OPotree");
    return omain(app, argc, argv);
}


void OPotreeRenderPass::initialize()
{
	RenderPass::initialize();

	// Initialize
	points = new Points();
	
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
		//glEnable(GL_DEPTH_TEST);
		//if(oglError) return;
		glEnable(GL_LIGHTING);
		if(oglError) return;
		glEnable(GL_BLEND);
		if(oglError) return;
	
		// Test and draw
		// get camera location in world coordinate
		points->draw();

		
		framecount++;
		if(framecount > 500) {
			if(SystemManager::instance()->isMaster()) {
				Vector3f cp = context.camera->getPosition();
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
