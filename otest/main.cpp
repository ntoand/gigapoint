#include <omega.h>
#include <omegaGl.h>

#include <iostream>
#include <list>
#include <string>

#include "Viewer.h"

using namespace omega;
using namespace std;

class OPotreeRenderPass: public RenderPass
{
public:
	OPotreeRenderPass(Renderer* client, Viewer* viewer): RenderPass(client, "OPotreeRenderPass"), viewer(viewer) {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	Viewer* viewer;
	int framecount;
};

class OPotreeApplication: public EngineModule
{
public:
	Viewer* viewer;

public:
	OPotreeApplication(): EngineModule("OPotreeApplication") {}

	virtual void initialize();

	virtual void initializeRenderer(Renderer* r) 
	{ 
		viewer = new Viewer();
		r->addRenderPass(new OPotreeRenderPass(r, viewer));
	}

	virtual void handleEvent(const Event& evt);
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
	
	viewer->init();

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

		glEnable(GL_LIGHT0);
		if(oglError) return;
		glEnable(GL_COLOR_MATERIAL);
		if(oglError) return;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Color(1.0, 1.0, 1.0).data());
		if(oglError) return;
		glLightfv(GL_LIGHT0, GL_POSITION, Vector3s(0.0f, 0.0f, 1.0f).data());
		if(oglError) return;

		// Test and draw
		// get camera location in world coordinate
		viewer->draw();
		if(oglError) return;	
		
		framecount++;
		if(framecount > 2000) {
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

void OPotreeApplication::handleEvent(const Event& evt) {
	if(evt.getServiceType() == Service::Pointer ) {
		int x = evt.getPosition().x();
		int y = evt.getPosition().y();
		int flags = evt.getFlags();

		if(evt.getType() == Event::Down && flags & 2) {
			DisplaySystem* ds = SystemManager::instance()->getDisplaySystem();
		    Ray r;
		    bool res = ds->getViewRayFromEvent(evt, r);
		    Vector3f pos = r.getOrigin();
		    Vector3f ori = r.getDirection();
		    cout << "Get ray " << x << " " << y << " pos: " << pos[0] << " " << pos[1] << " " << pos[2] << " ori: "
		    		<< ori[0] << " " << ori[1] << " " << ori[2] << endl;
		    viewer->setCheckCollision(true, r);
		}
		
	}
}
