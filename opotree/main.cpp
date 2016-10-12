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
	cam->getController()->setSpeed(10);
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
	//pointcloud = new PointCloud("ripple/");
	//pointcloud = new PointCloud("srsota/");
	//pointcloud = new PointCloud("grass/");
	//pointcloud = new PointCloud("P100/");
	pointcloud = new PointCloud("century/");
	
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

		Vector3f cp = context.camera->convertLocalToWorldPosition(Vector3f(0, 0, 0));
		float campos[3] = {cp[0], cp[1], cp[2]};

		float* MVP = (context.projection*context.modelview).cast<float>().data();
		pointcloud->updateVisibility(MVP, campos);
		pointcloud->draw(MVP);

		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}
