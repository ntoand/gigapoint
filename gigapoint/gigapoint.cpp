#include <omega.h>
#include <omegaGl.h>
#include <iostream>

#include "PointCloud.h"

using namespace std;
using namespace omega;

///////////////////////////////////////////////////////////////////////////////
class GigapointRenderModule : public EngineModule
{
public:
    GigapointRenderModule() :
        EngineModule("GigapointRenderModule"), pointcloud(0), option(0), visible(true)
    {
    	
    }

    virtual void initializeRenderer(Renderer* r);

    virtual void update(const UpdateContext& context)
    {
        // After a frame all render passes had a chance to update their
        // textures. reset the raster update flag.
       
    }
    
    virtual void dispose()
    {
    	if(pointcloud)
            delete pointcloud;
        if(option)
            delete option;
    }

    void initPotree(const string& option_file)
    {
    	//init values here
    	//option = gigapoint::Utils::loadOption("opotree.json");
    	option = gigapoint::Utils::loadOption(option_file);
    	pointcloud = new gigapoint::PointCloud(option, SystemManager::instance()->isMaster());
    	pointcloud->initPointCloud();

    	//Camera
		Camera* cam = getEngine()->getDefaultCamera();
		cam->getController()->setSpeed(option->cameraSpeed);
		cam->getController()->setFreeFlyEnabled(true);
		cam->setNearFarZ(1.0, 10000.0);

		if(option->cameraUpdatePosOri) {
			cam->setPosition(Vector3f(option->cameraPosition[0], option->cameraPosition[1], option->cameraPosition[2]));
			cam->setOrientation(Quaternion(option->cameraOrientation[0], option->cameraOrientation[1], 
											option->cameraOrientation[2], option->cameraOrientation[3]));
		}	
    }

    void updateMaterial(const string& material)
    {  
        if(material.compare("rgb") == 0)
            option->material = MATERIAL_RGB;
        else if (material.compare("elevation") == 0)
            option->material = MATERIAL_ELEVATION;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updateQuality(const string& quality)
    {
        if(quality.compare("square") == 0)
            option->quality = QUALITY_SQUARE;
        else if (quality.compare("circle") == 0)
            option->quality = QUALITY_CIRCLE;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updateSizeType(const string& type)
    {
        if(type.compare("fixed") == 0)
            option->sizeType = SIZE_FIXED;
        else if (type.compare("adaptive") == 0)
            option->sizeType = SIZE_ADAPTIVE;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updatePointScale(const float val)
    {
        option->pointScale[0] = val;
        pointcloud->setReloadShader(false);
    }

    void updateVisible(const bool b)
    {
	visible = b;
    }

    void printInfo()
    {
	if(pointcloud)
		pointcloud->setPrintInfo(true);
    }

    gigapoint::PointCloud* pointcloud;
    gigapoint::Option* option; 
    bool visible;
};

///////////////////////////////////////////////////////////////////////////////
class GigapointRenderPass : public RenderPass
{
public:
    GigapointRenderPass(Renderer* client, GigapointRenderModule* prm) : 
        RenderPass(client, "GigapointRenderPass"), 
        module(prm) {}
    
    virtual void initialize()
    {
        RenderPass::initialize();

        //graphics
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }

    virtual void render(Renderer* client, const DrawContext& context)
    {
    	if(context.task == DrawContext::SceneDrawTask)
        {
            glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
            client->getRenderer()->beginDraw3D(context);

	    if(module->visible)
	    { 
			// Test and draw
			// get camera location in world coordinate
            	//if(context.eye == DrawContext::EyeLeft || context.eye == DrawContext::EyeCyclop)
            	{
                	Vector3f cp = context.camera->getPosition();
                	float campos[3] = {cp[0], cp[1], cp[2]};
                	float* MVP = (context.projection*context.modelview).cast<float>().data();
                	module->pointcloud->updateVisibility(MVP, campos);
            	}

		module->pointcloud->draw();
		if(oglError) return;
	    }
            client->getRenderer()->endDraw();
            glPopAttrib();
        }
        
    }

private:
    GigapointRenderModule* module;

};

///////////////////////////////////////////////////////////////////////////////
void GigapointRenderModule::initializeRenderer(Renderer* r)
{
    r->addRenderPass(new GigapointRenderPass(r, this));
}

///////////////////////////////////////////////////////////////////////////////
GigapointRenderModule* initialize()
{
    GigapointRenderModule* prm = new GigapointRenderModule();
    ModuleServices::addModule(prm);
    prm->doInitialize(Engine::instance());
    return prm;
}

///////////////////////////////////////////////////////////////////////////////
// Python API
#include "omega/PythonInterpreterWrapper.h"
BOOST_PYTHON_MODULE(gigapoint)
{
    //
    PYAPI_REF_BASE_CLASS(GigapointRenderModule)
        PYAPI_METHOD(GigapointRenderModule, initPotree)
        PYAPI_METHOD(GigapointRenderModule, updateMaterial)
        PYAPI_METHOD(GigapointRenderModule, updateQuality)
        PYAPI_METHOD(GigapointRenderModule, updateSizeType)
        PYAPI_METHOD(GigapointRenderModule, updatePointScale)
	PYAPI_METHOD(GigapointRenderModule, updateVisible)
	PYAPI_METHOD(GigapointRenderModule, printInfo)
        ;

    def("initialize", initialize, PYAPI_RETURN_REF);
}
