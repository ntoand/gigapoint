#include <omega.h>
#include <omegaGl.h>

#include <iostream>
#include <list>
#include <string>

#include "Cube.h"

using namespace omega;
using namespace std;

class OPotreeRenderPass: public RenderPass
{
public:
	OPotreeRenderPass(Renderer* client): RenderPass(client, "OPotreeRenderPass") {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	Cube* cube;
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
	/*
	Camera* cam = getEngine()->getDefaultCamera();
	cam->getController()->setSpeed(option.cameraSpeed);
	cam->getController()->setFreeFlyEnabled(true);
	*/
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
	cube = new Cube();
	
	//graphics
	//glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
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
		cube->draw();
		
		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}
