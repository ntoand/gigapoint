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
	//MyTriangle* triangle;
	PointCloud* pointcloud;
};

class OPotreeApplication: public EngineModule
{
public:
	OPotreeApplication(): EngineModule("OPotreeApplication") {}

	virtual void initializeRenderer(Renderer* r) 
	{ 
		r->addRenderPass(new OPotreeRenderPass(r));
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
	pointcloud = new PointCloud("ripple/");
	
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
		//triangle->draw(context);
		//node->draw(context);
		float* MVP = (context.projection*context.modelview).cast<float>().data();
		pointcloud->updateVisibility(MVP);
		pointcloud->draw(MVP);

		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}
